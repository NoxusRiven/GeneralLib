#pragma once

#include "Movment.h"
#include "Collision.h"
#include "Graphics.h"

#include <raylib.h>

void draw_sprites(ECS::World &world);
void draw_debug(ECS::World& world, std::vector<ECS::Entity> entities, Color color);
void draw_collision_boxes(ECS::World& world);
ECS::Entity create_player(ECS::World& world, std::vector<Graphics::Sprite>& sprites);
ECS::Entity create_enemy(ECS::World& world, Vector2 pos);
void handle_user_inputs(ECS::World& world, std::vector<Graphics::Sprite> sprites, ECS::Entity &player, std::vector<ECS::Entity>& enemies);
void handle_solid_collision(ECS::World& world, Collision::QuadTree& q_tree);

