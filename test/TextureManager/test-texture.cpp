#include <iostream>
#include "..\..\TextureManager.h"
#include "..\..\Arena.h"

void run_texture_test()
{
    InitWindow(800, 600, "Texture Manager Test");
    SetTargetFPS(60);
    using TextureManager::Sprite;
    TextureManager::TextureManager& t_manager = TextureManager::TextureManager::get_instance();


    Memory::Arena arena(1024);
    auto sprite_map = t_manager.load_texture("Sprites/ghost.png", "Sprites/ghost_atlas.json", arena);
    
    for (auto pair : sprite_map)
    {
        std::cout << "Sprite name: " << pair.first << " Size: " << pair.second->width << "x" << pair.second->height << "\n";
    }

    std::cout << "Texture atlas loaded successfully\n";

    while (!WindowShouldClose())
    {
        BeginDrawing();
        ClearBackground(BLACK);
        // Draw sprites from the atlas
        int x = 100;
        int y = 100;
        for (auto pair : sprite_map)
        {
            Sprite* sprite = pair.second;

            std::cout << "Sprite values: " << sprite->texture << ", " << sprite->x << ", " << sprite->y << ", " << sprite->width << ", " << sprite->height << "\n";
            
            DrawTextureRec(*sprite->texture, { (float)sprite->x, (float)sprite->y, (float)sprite->width, (float)sprite->height }, {(float)x, (float)y}, WHITE);
            x += 100;
            y += 100;
        }
        EndDrawing();
    }

}