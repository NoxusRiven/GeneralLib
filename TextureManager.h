#pragma once

#include "raylib.h"

#include <unordered_map>
#include <memory>
#include <string>
#include "Allocator.h"

namespace TextureManager
{
    struct Sprite
    {
        Texture2D* texture;
        int x, y, width, height;

        Sprite(Texture& texture);
        Sprite(Texture& texture, int x, int y, int w, int h);
    };

    class TextureManager
    {
    private:
        std::unordered_map<std::string, Texture2D> _textures;
        std::vector<Sprite*> _sprites_to_destroy;
        void check_texture_map(std::string& path);

        TextureManager();

    public:
        ~TextureManager();

        static TextureManager& get_instance();

        //allocationg is up to user, not new as a default allocator
        Sprite* load_texture(std::string path, Memory::Allocator& mem_alloc);
        std::unordered_map<std::string, Sprite*> load_texture(std::string path, std::string atlas_json, Memory::Allocator& mem_alloc);

        TextureManager(const TextureManager&) = delete;
        TextureManager& operator=(const TextureManager&) = delete;
    };
}


