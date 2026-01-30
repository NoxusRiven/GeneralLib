#include "..\Graphics.h"
#include "..\JSON_Parser.h"

namespace Graphics
{
    // -------------------------------- SPRITE --------------------------------

    Sprite::Sprite(Texture& texture)
    {
        this->texture = &texture;
        x = 0;
        y = 0;
        width = texture.width;
        height = texture.height;
    }

    Sprite::Sprite(Texture& texture, float x, float y, float w, float h)
    {
        this->texture = &texture;
        this->x = x;
        this->y = y;
        width = w;
        height = h;
    }


    Sprites::Sprites()
    {
        _texture.reserve(256);
        _position.reserve(256);
        _size.reserve(256);

        _sparse_indices.resize(1024, -1); //initialize sparse indices to -1 (invalid)
        _dense_entities.reserve(1024);
    }

    //TODO: might be needed fix where you can fill unused sprase indices
    void Sprites::add(size_t entity, Texture2D& t, Vector2 pos, Vector2 size)
    {
        if (contains(entity))
            return;

        if (entity >= _sparse_indices.size()) 
        {
            size_t new_size = std::max(_sparse_indices.size() * 2, entity + 1);
            _sparse_indices.resize(new_size, -1);
        }



        size_t dense = _dense_entities.size();
        _sparse_indices[entity] = dense;
        

        _texture.push_back(t);
        _position.push_back(pos);
        _size.push_back(size);

        _dense_entities.push_back(entity);
    }

    void Sprites::remove(size_t entity)
    {
        if (!contains(entity))
            return;

        size_t sprite_idx = _sparse_indices[entity];
        size_t last_sprite_idx = _dense_entities.size() - 1;
        size_t last_entity = _dense_entities[last_sprite_idx];

        //move last sprite to removed sprite position
        _texture[sprite_idx] = _texture[last_sprite_idx];
        _position[sprite_idx] = _position[last_sprite_idx];
        _size[sprite_idx] = _size[last_sprite_idx];

        //update sparse index for moved entity
        _sparse_indices[last_entity] = sprite_idx;
        _dense_entities[sprite_idx] = last_entity;

        //pop back last sprite data
        _texture.pop_back();
        _position.pop_back();
        _size.pop_back();
        _dense_entities.pop_back();
        
        _sparse_indices[entity] = -1;
    }

    bool Sprites::contains(size_t entity)
    {
        return entity <= _sparse_indices.size() && _sparse_indices[entity] != -1;
    }

     // -------------------------------- TEXTURE MANAGER --------------------------------
    TextureManager::TextureManager()
    {
        _sprites_to_destroy.reserve(256);
    }


    TextureManager& TextureManager::get_instance()
    {
        static TextureManager instance;
        return instance;
    }

    void TextureManager::check_texture_map(std::string& path)
    {
        std::size_t filename_start = path.find_first_of("/\\");
        std::string fullpath = path;
        
        if(filename_start != std::string::npos)
            path = path.substr(filename_start + 1);


        if(_textures.find(path) == _textures.end())
        {
            _textures[path] = LoadTexture(fullpath.c_str());
        }
    }

    Sprite* TextureManager::load_texture(std::string path, Memory::Arena& arena)
    {
        check_texture_map(path);

        //create sprite from full texture
        //allocate memory then use placement new to construct sprite
        void* mem = arena.allocate(sizeof(Sprite), alignof(Sprite));
        Sprite* sprite = new (mem) Sprite(_textures[path]);
        _sprites_to_destroy.push_back(sprite);

        return sprite;

    }

    std::unordered_map<std::string, Sprite*> TextureManager::load_texture(std::string path, std::string atlas_json, Memory::Arena& arena)
    {
        check_texture_map(path);

        //parse json atlas file
        JSON::JSON_Parser j_parser;
        JSON::JsonValue json = j_parser.parse_file(atlas_json.c_str());

        std::unordered_map<std::string, Sprite*> sprite_map;

        //parse data from json and create sprites
        auto frames_arr = json["frames"].get_as<JSON::j_array>();
        for (JSON::JsonValue obj : frames_arr)
        {
            int sprite_x = (int)obj.get_as<JSON::j_object>()["frame"]["x"].get_as<double>();
            int sprite_y = (int)obj.get_as<JSON::j_object>()["frame"]["y"].get_as<double>();
            int sprite_w = (int)obj.get_as<JSON::j_object>()["frame"]["w"].get_as<double>();
            int sprite_h = (int)obj.get_as<JSON::j_object>()["frame"]["h"].get_as<double>();
            
            void* mem = arena.allocate(sizeof(Sprite), alignof(Sprite));
            Sprite* sprite = new (mem) Sprite( _textures[path], sprite_x, sprite_y, sprite_w, sprite_h );
            _sprites_to_destroy.push_back(sprite);

            std::string name = obj.get_as<JSON::j_object>()["filename"].get_as<std::string>();
            sprite_map.insert({ name, sprite });
        }

        return sprite_map;
    }


    void TextureManager::load_texture(size_t entity, std::string path, Sprites& sprites)
    {
        check_texture_map(path);

        //create id
        sprites.add(
            entity, 
            _textures[path], 
            Vector2{ 0, 0 }, 
            Vector2
            {
                static_cast<float>(_textures[path].height), 
                static_cast<float>(_textures[path].width)
            } 
        );
    }

    //user should know how many sprites are created from atlas
    void TextureManager::load_texture(std::vector<size_t> entities, std::string path, std::string atlas_json, Sprites& sprites)
    {
        check_texture_map(path);

        //parse json atlas file
        JSON::JSON_Parser j_parser;
        JSON::JsonValue json = j_parser.parse_file(atlas_json.c_str());

        //parse data from json and create sprites
        auto frames_arr = json["frames"].get_as<JSON::j_array>();
        int counter = 0;
        for (JSON::JsonValue obj : frames_arr)
        {
            int sprite_x = (int)obj.get_as<JSON::j_object>()["frame"]["x"].get_as<double>();
            int sprite_y = (int)obj.get_as<JSON::j_object>()["frame"]["y"].get_as<double>();
            int sprite_w = (int)obj.get_as<JSON::j_object>()["frame"]["w"].get_as<double>();
            int sprite_h = (int)obj.get_as<JSON::j_object>()["frame"]["h"].get_as<double>();

            sprites.add(
                entities[counter++],
                _textures[path], 
                Vector2{ static_cast<float>(sprite_x), static_cast<float>(sprite_y) }, 
                Vector2{ static_cast<float>(sprite_w), static_cast<float>(sprite_h) } 
            );

            //just in case program doesnt crash if not enough entities provided
            if(counter >= entities.size())
                break;
        }

    }

    TextureManager::~TextureManager()
    {
        //destroy all sprites created
        for (Sprite* sprite : _sprites_to_destroy)
        {
            std::cout << "Destroying sprite: " << sprite << "\n";
            sprite->~Sprite();
        }
        _sprites_to_destroy.clear();

        //unload all textures from gpu memory
        for (auto& [key, texture] : _textures)
        {
            UnloadTexture(texture);
        }
        _textures.clear();
    }

//------------------------------- Systems --------------------------------------------

    void draw_sprites(ECS::World & world)
    {
        if (!world.storage_registry.contains<Sprites>())
        {
            return;
        }

        auto& sprites = world.storage_registry.get<Sprites>();
    }

}
