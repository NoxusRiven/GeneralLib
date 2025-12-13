#include "TextureManager.h"

namespace TextureManager
{
    TextureManager& TextureManager::get_instance()
    {
        static TextureManager instance;
        return instance;
    }

    std::shared_ptr<Texture2D> TextureManager::get_texture(const char* path)
    {
        if(textures.find(path) != textures.end())
        {
            textures[path] = std::make_shared<Texture2D>(LoadTexture(path));
        }

        return textures[path];

    }

    std::shared_ptr<Texture2D> TextureManager::get_texture(const char* path, const char* atlas)
    {
        std::shared_ptr<Texture2D> texture;
        return texture;
    }

    TextureManager::~TextureManager()
    {
        for (auto& [key, texture] : textures)
        {
            UnloadTexture(*texture);
        }
        textures.clear();
    }
}
