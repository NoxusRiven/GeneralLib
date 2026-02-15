#pragma once

#include "raylib.h"

#include <unordered_map>
#include <memory>
#include <string>
#include "Arena.h"
#include <vector>
#include "ECS.h"

//refractor sprite to not be a signle class but component
namespace Graphics
{
    struct Sprite
    {
        Texture2D* texture;
        Vector2 posistion;
        Vector2 size;

        Sprite(Texture2D& texture);
        Sprite(Texture2D& texture, Vector2 pos, Vector2 size);
    };


    class Sprites : public ECS::Storage
    {
    private:
        std::vector<Texture2D> _texture;
        std::vector<Vector2> _position;
        std::vector<Vector2> _size;

    public:
        Sprites();

        void add(size_t entity, Texture2D& t, Vector2 pos, Vector2 size);
        void remove(size_t entity) override;

        void set_entity(size_t entity, Texture2D& t, Vector2 pos, Vector2 size)
        {
            size_t idx = dense_index(entity);
            _texture[idx] = t;
            _position[idx] = pos;
            _size[idx] = size;
        }

        void set_at(size_t idx, Texture2D& t, Vector2 pos, Vector2 size)
        {
            _texture[idx] = t;
            _position[idx] = pos;
            _size[idx] = size;
        }

        //use for single entity access
        Texture2D& get_texture(size_t entity)
        {
            return _texture[dense_index(entity)];
        }

        Vector2 get_position(size_t entity)
        {
            return _position[dense_index(entity)];
        }

        Vector2 get_size(size_t entity)
        {
            return _size[dense_index(entity)];
        }

        //use for iterating over all entities
        Texture2D& texture_at(size_t idx)
        {
            return _texture[idx];
        }

        Vector2 position_at(size_t idx)
        {
            return _position[idx];
        }

        Vector2 size_at(size_t idx)
        {
            return _size[idx];
        }
    };

    class TextureManager
    {
    private:
        //contains only stripped file name from path as key
        std::unordered_map<std::string, Texture2D> _textures;
        std::vector<Sprite*> _sprites_to_destroy;
        void check_texture_map(std::string& path);

        TextureManager();

    public:
        ~TextureManager();

        static TextureManager& instance();

        //allocationg is up to user, not new as a default allocator
        Sprite* load_texture(std::string path, Memory::Arena& mem_alloc);
        std::unordered_map<std::string, Sprite*> load_texture(std::string path, std::string atlas_json, Memory::Arena& mem_alloc);

        void load_texture(size_t entity, std::string path, Sprites& sprites);

        Sprite load_texture(std::string path, std::string atlas_json);
        void load_texture(std::string path, std::string atlas_json, std::vector<Sprite>& sprites);

        Texture2D& get_texture(std::string path);

        TextureManager(const TextureManager&) = delete;
        TextureManager& operator=(const TextureManager&) = delete;
    };
}


