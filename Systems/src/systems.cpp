#include "systems.h"

#define ENEMY_COUNT 30


//TODO: where ever you can remove new keyword and use arena or pool data allocators

void draw_sprites(ECS::World &world)
{
    if (!world.storage_registry.contains<Graphics::Sprites>() &&
        !world.storage_registry.contains<Movement::Positions>())
    {
        printf("draw_sprites(): No sprite or posistion storage in world!\n");
        return;
    }

    auto& sprites = world.storage_registry.get<Graphics::Sprites>();
    auto& positions = world.storage_registry.get<Movement::Positions>();

    for (size_t i = 0; i < sprites.size(); i++)
    {
        //to know position of this entity i need to find its id, then i can see what pos it has
        size_t e = sprites.entity_at(i);

        //little faster operating, optional if needed more speed
        //size_t idx = sprites.dense_index(e);

        Rectangle rect = 
        {
           sprites.position_at(i).x,
           sprites.position_at(i).y,
           sprites.size_at(i).x,
           sprites.size_at(i).y
        };

        //TODO: add to sprite scale and change this to draw texture ex or maybe add field "scale" to sprites and scale manualy when drawing or smth
        DrawTextureRec(
            sprites.texture_at(i),
            rect,
            { positions.get_x(e), positions.get_y(e) },
            WHITE
        );
    }
}

void draw_debug(ECS::World& world, std::vector<ECS::Entity> entities, Color color)
{
    if (!world.storage_registry.contains<Movement::Positions>())
    {
        printf("draw_sprites(): No Posistion storage in world!\n");
        return;
    }

    auto& positions = world.storage_registry.get<Movement::Positions>();

    for (size_t i = 0; i < entities.size(); i++)
    {
        size_t idx = positions.dense_index(entities[i].id);

        Rectangle rect = 
        {
           positions.x_at(idx),
           positions.y_at(idx),
           20,
           20
        };

        DrawRectangleRec(rect, color);
    }
}

void draw_collision_boxes(ECS::World& world)
{
    if (!world.storage_registry.contains<Collision::Boxes>() ||
        !world.storage_registry.contains<Movement::Positions>())
    {
        printf("draw_collision_boxes(): No Box or Posistion storage in world!\n");
        return;
    }

    auto& boxes = world.storage_registry.get<Collision::Boxes>();
    auto& positions = world.storage_registry.get<Movement::Positions>();


    for (size_t i = 0; i < boxes.size(); i++)
    {
        Vector2 size = boxes.size_at(i);
        Vector2 offset = boxes.offset_at(i);
        Rectangle rect = 
        {
           positions.x_at(i) + offset.x,
           positions.y_at(i) + offset.y,
           size.x,
           size.y
        };

        DrawRectangleRec(rect, GREEN);
    }
}

//TODO: figure out default offset for colli boxes and add custom offset and size to player
ECS::Entity create_player(ECS::World& world, std::vector<Graphics::Sprite>& sprites)
{
    ECS::Entity player = world.entity_manager.create();

    Vector2 box_size = { sprites[1].size.x - 3, sprites[1].size.y - 3 };
    Vector2 offset = { (sprites[1].size.x - box_size.x) / 2 , (sprites[1].size.y - box_size.y) / 2};

    world.storage_registry.get<Movement::Positions>().add(player.id, 100, 100);
    world.storage_registry.get<Movement::Velocities>().add(player.id, 0, 0);
    world.storage_registry.get<Graphics::Sprites>().add(player.id, *sprites[1].texture, sprites[1].posistion, sprites[1].size);
    world.storage_registry.get<Collision::Boxes>().add(player.id, box_size, offset, true);


    return player;
}

ECS::Entity create_enemy(ECS::World& world, Vector2 pos)
{
    ECS::Entity enemy =  world.entity_manager.create();

    world.storage_registry.get<Movement::Positions>().add(enemy.id, pos.x, pos.y);
    world.storage_registry.get<Movement::Velocities>().add(enemy.id, 0, 0);
    //world.storage_registry.get<Graphics::Sprites>().add(enemy.id, *sprites[1].texture, sprites[1].posistion, sprites[1].size);
    world.storage_registry.get<Collision::Boxes>().add(enemy.id, { 20, 20 }, { 0, 0 }, true);


    return enemy;

}

void handle_user_inputs(ECS::World& world, std::vector<Graphics::Sprite> sprites, ECS::Entity &player, std::vector<ECS::Entity>& enemies)
{
    if (!world.storage_registry.contains<Movement::Positions>() || 
        !world.storage_registry.contains<Movement::Velocities>())
    {
        printf("handle_inputs(): No position or velocity storage in world!\n");
        return;
    }

    float speed = 100.0f;
    float vx = 0, vy = 0;

    if (world.entity_manager.is_alive(player))
    {
        if (IsKeyDown(KEY_D))
        {
            vx += speed; 
            world.storage_registry.get<Graphics::Sprites>().set_entity(player.id, *sprites[3].texture, sprites[3].posistion, sprites[3].size);
        }
        if (IsKeyDown(KEY_A))
        {
            vx -= speed;
           world.storage_registry.get<Graphics::Sprites>().set_entity(player.id, *sprites[2].texture, sprites[2].posistion, sprites[2].size);
        }
        if (IsKeyDown(KEY_W))
        { 
            vy -= speed;
            world.storage_registry.get<Graphics::Sprites>().set_entity(player.id, *sprites[0].texture, sprites[0].posistion, sprites[0].size);
        }
        if (IsKeyDown(KEY_S))
        { 
            vy += speed;
            world.storage_registry.get<Graphics::Sprites>().set_entity(player.id, *sprites[1].texture, sprites[1].posistion, sprites[1].size);
        }
    }

    //DEBUG: creating and destroying entities (mostly player) on command
    // but this is good way to handle killing/creating new player
    if (IsKeyPressed(KEY_K))
    {
        if (world.entity_manager.is_alive(player))
        {
            world.delete_entity(player);
        }
        else
        {
            player = create_player(world, sprites);
        }
    }
    if(IsKeyPressed(KEY_R))
    {
        world.entity_manager.create();
    }
    if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON))
    {
        Vector2 mouse_pos = GetMousePosition();
        enemies.push_back(create_enemy(world, mouse_pos));
    }

    if(!world.entity_manager.is_alive(player))
        return;

    world.storage_registry.get<Movement::Velocities>().set_entity(
        player.id,
        vx,
        vy
    );

    
    /*float x = world.storage_registry.get<Movement::Positions>().get_x(player.id);
    float y = world.storage_registry.get<Movement::Positions>().get_y(player.id);

    std::printf("Position (X: %f Y: %f)\n", x, y);*/
}

void handle_solid_collision(ECS::World& world, Collision::QuadTree& q_tree)
{
    auto& velocities = world.storage_registry.get<Movement::Velocities>();
    auto& positions = world.storage_registry.get<Movement::Positions>();
    auto& boxes = world.storage_registry.get<Collision::Boxes>();

    Collision::CollidedBoxes colli_boxes;

    for (size_t i = 0; i < velocities.size(); i++)
    {
        size_t entity = velocities.entity_at(i);

        float vx = velocities.get_vx(entity);
        float vy = velocities.get_vy(entity);



        q_tree.search(entity, world, colli_boxes);

        for (size_t j = 0; j < colli_boxes.size(); j++)
        {
            float x = positions.get_x(entity);
            float y = positions.get_y(entity);

            Rectangle colli_rect = { colli_boxes.pos[j].x, colli_boxes.pos[j].y, colli_boxes._size[j].x, colli_boxes._size[j].y };
            bool solid = colli_boxes.solid[j];

            if (!solid)
            {
                continue;
            }

            if (vx != 0 && vy != 0)
            {
                if (colli_rect.width < colli_rect.height)
                {
                    if (vx > 0)
                    {
                        positions.set_entity(entity, x - colli_rect.width, y);
                    }
                    else
                    {
                        positions.set_entity(entity, x + colli_rect.width, y);
                    }
                }
                else
                {
                    if (vy > 0)
                    {
                        positions.set_entity(entity, x, y - colli_rect.height);

                    }
                    else
                    {
                        positions.set_entity(entity, x, y + colli_rect.height);

                    }
                }
            }
            else if (vx != 0)
            {
                if (vx > 0)
                {
                    positions.set_entity(entity, x - colli_rect.width, y);
                }
                else
                {
                    positions.set_entity(entity, x + colli_rect.width, y);
                }
            }
            else if (vy != 0)
            {
                if (vy > 0)
                {
                    positions.set_entity(entity, x, y - colli_rect.height);

                }
                else
                {
                    positions.set_entity(entity, x, y + colli_rect.height);

                }
            }
        }

        //always reset structure holding collision data
        colli_boxes.clear();
    }
}