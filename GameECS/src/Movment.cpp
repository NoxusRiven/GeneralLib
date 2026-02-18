#include "Movment.h"

namespace Movement
{
    //-------------------------- Positions --------------------------

     Positions::Positions()
     {
        _x.reserve(2048);
        _y.reserve(2048);

        _sparse_indices.resize(2048, -1); //initialize sparse indices to -1 (invalid)
        _dense_entities.reserve(2048);
     }

     void Positions::add(size_t entity, float x, float y)
     {
        add_entity(entity);
            
        _x.push_back(x);
        _y.push_back(y);
     }

    void Positions::remove(size_t entity)
    {
        size_t sprite_idx = remove_entity(entity);

        if (sprite_idx == (size_t)-1) //biggest size_t value
            return; //entity not found

        //removed entity in method above so no need to use -1
        size_t last_sprite_idx = _dense_entities.size();

        //move last sprite to removed sprite position
        _x[sprite_idx] = _x[last_sprite_idx];
        _y[sprite_idx] = _y[last_sprite_idx];

        //pop back last sprite data
        _x.pop_back();
        _y.pop_back();
    }


    //-------------------------- Velocities --------------------------

     Velocities::Velocities()
     {
        _vx.reserve(1024);
        _vy.reserve(1024);

        _sparse_indices.resize(2048, -1); //initialize sparse indices to -1 (invalid)
        _dense_entities.reserve(2048);
     }

     void Velocities::add(size_t entity, float x, float y)
     {
         add_entity(entity);

        _vx.push_back(x);
        _vy.push_back(y);
     }

    void Velocities::remove(size_t entity)
    {
        size_t dense = remove_entity(entity);

        if (dense == (size_t)-1) //biggest size_t value
            return; //entity not found

        size_t last = _dense_entities.size();

        //move last sprite to removed sprite position
        _vx[dense] = _vx[last];
        _vy[dense] = _vy[last];

 
        //pop back last sprite data
        _vx.pop_back();
        _vy.pop_back();
    }

    // -------------------------- Movement System --------------------------

    void movment(ECS::World& world, float dt)
    {
        if (!world.storage_registry.contains<Positions>() || 
            !world.storage_registry.contains<Velocities>())
        {
            printf("movment(): No position or velocity storage in world!\n");
            return;
        }

        auto& positions = world.storage_registry.get<Positions>();
        auto& velocities = world.storage_registry.get<Velocities>();

        if (velocities.size() != positions.size())
        {
            printf("movement(): Positions and Velocities storage size mismatch!\n");
            return;
        }

        for (size_t i = 0; i < positions.size(); i++) 
        {
            positions.set_at(
                i,
                positions.x_at(i) + (velocities.vx_at(i) * dt),
                positions.y_at(i) + (velocities.vy_at(i) * dt)
            );
        }
    }
}
