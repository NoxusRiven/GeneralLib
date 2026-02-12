#include "..\Graphics.h"
#include "..\JSON_Parser.h"

namespace Graphics
{
    // -------------------------------- SPRITE --------------------------------

    Sprite::Sprite(Texture2D& texture)
    {
        this->texture = &texture;
        posistion = { 0, 0 };
        size = { (float)texture.width, (float)texture.height };
    }

    Sprite::Sprite(Texture2D& texture, Vector2 pos, Vector2 size)
    {
        this->texture = &texture;
        posistion = pos;
        this->size = size;
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
         add_entity(entity);

        _texture.push_back(t);
        _position.push_back(pos);
        _size.push_back(size);
    }

    void Sprites::remove(size_t entity)
    {
        size_t dense = remove_entity(entity);

        if (dense == (size_t)-1) //biggest size_t value
            return; //entity not found

        size_t last = _dense_entities.size();

        _texture[dense] = _texture[last];
        _position[dense] = _position[last];
        _size[dense] = _size[last];



        //pop back last sprite data
        _texture.pop_back();
        _position.pop_back();
        _size.pop_back();
    }

     // -------------------------------- TEXTURE MANAGER --------------------------------
    TextureManager::TextureManager()
    {
        _sprites_to_destroy.reserve(256);
    }


    TextureManager& TextureManager::instance()
    {
        static TextureManager instance;
        return instance;
    }

    //use it every time u want to check data in texture map
    void TextureManager::check_texture_map(std::string& path)
    {
        std::size_t filename_start = path.find_first_of("/\\");
        std::string fullpath = path;
        
        //alters path so that it contains only file name
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
            float sprite_x = obj.get_as<JSON::j_object>()["frame"]["x"].get_as<double>();
            float sprite_y = obj.get_as<JSON::j_object>()["frame"]["y"].get_as<double>();
            float sprite_w = obj.get_as<JSON::j_object>()["frame"]["w"].get_as<double>();
            float sprite_h = obj.get_as<JSON::j_object>()["frame"]["h"].get_as<double>();
            
            void* mem = arena.allocate(sizeof(Sprite), alignof(Sprite));
            Sprite* sprite = new (mem) Sprite(_textures[path], Vector2{ sprite_x, sprite_y }, Vector2{ sprite_w, sprite_h });
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

     Sprite TextureManager::load_texture(std::string path, std::string atlas_json)    
     {
        check_texture_map(path);

        return Sprite
        { 
            _textures[path],
            Vector2{ 0, 0 },
            Vector2
            {
                static_cast<float>(_textures[path].height),
                static_cast<float>(_textures[path].width)
            }
        };
     }

    //user should know how many sprites are created from atlas
    void TextureManager::load_texture(std::string path, std::string atlas_json, std::vector<Sprite>& sprites)
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

            sprites.push_back(
                Sprite
                {
                    _textures[path],
                    Vector2{ static_cast<float>(sprite_x), static_cast<float>(sprite_y) },
                    Vector2{ static_cast<float>(sprite_w), static_cast<float>(sprite_h) } 
                }
            );
        }
    }

    Texture2D& TextureManager::get_texture(std::string path)
    {
        check_texture_map(path);

        //constains only file name
        return _textures[path];
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
}
