#include "..\TextureManager.h"
#include "..\JSON_Parser.h"

namespace TextureManager
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

    Sprite::Sprite(Texture& texture, int x, int y, int w, int h)
    {
        this->texture = &texture;
        this->x = x;
        this->y = y;
        width = w;
        height = h;
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
        
        if(filename_start != std::string::npos)
            path = path.substr(filename_start + 1);


        if(_textures.find(path) == _textures.end())
        {
            _textures[path] = LoadTexture(path.c_str());
        }
    }

    //TODO: fix ghost pngs not showing in test test/lib issue
    Sprite* TextureManager::load_texture(std::string path, Memory::Allocator& mem_alloc)
    {
        check_texture_map(path);

        //create sprite from full texture
        //allocate memory then use placement new to construct sprite
        void* mem = mem_alloc.allocate(sizeof(Sprite), alignof(Sprite));
        Sprite* sprite = new (mem) Sprite(_textures[path]);
        _sprites_to_destroy.push_back(sprite);

        return sprite;

    }

    std::unordered_map<std::string, Sprite*> TextureManager::load_texture(std::string path, std::string atlas_json, Memory::Allocator& mem_alloc)
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
            
            void* mem = mem_alloc.allocate(sizeof(Sprite), alignof(Sprite));
            Sprite* sprite = new (mem) Sprite( _textures[path], sprite_x, sprite_y, sprite_w, sprite_h );
            _sprites_to_destroy.push_back(sprite);

            std::string name = obj.get_as<JSON::j_object>()["filename"].get_as<std::string>();
            sprite_map.insert({ name, sprite });
        }

        return sprite_map;
    }

    TextureManager::~TextureManager()
    {
        //destroy all sprites created
        for (Sprite* sprite : _sprites_to_destroy)
        {
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
