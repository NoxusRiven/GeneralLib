#pragma once

#include "raylib.h"

#include <unordered_map>
#include <memory>

namespace TextureManager
{
    class TextureManager
    {
    private:
        std::unordered_map<const char*, std::shared_ptr<Texture>> textures;
        TextureManager() {}
    
    public:
        ~TextureManager();

        static TextureManager& get_instance();
        std::shared_ptr<Texture2D> get_texture(const char* path);
        std::shared_ptr<Texture2D> get_texture(const char* path, const char* atlas);

        TextureManager(const TextureManager&) = delete;
        TextureManager& operator=(const TextureManager&) = delete;
    };
}

