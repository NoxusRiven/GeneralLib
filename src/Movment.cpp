#include "..\Movment.h"

namespace Movement
{
    //-------------------------- Positions --------------------------

     Positions::Positions()
     {
        _x.reserve(256);
        _y.reserve(256);

        _sparse_indices.resize(1024, -1); //initialize sparse indices to -1 (invalid)
        _dense_entities.reserve(1024);
     }

     void Positions::add(size_t entity, float x, float y)
     {
         if (contains(entity))
                return;

         if (entity >= _sparse_indices.size())
         {
             size_t new_size = std::max(_sparse_indices.size() * 2, entity + 1);
             _sparse_indices.resize(new_size, -1);
         }

        size_t dense = _dense_entities.size();
        _sparse_indices[entity] = dense;
            
        _x.push_back(x);
        _y.push_back(y);

        _dense_entities.push_back(entity);
     }

    void Positions::remove(size_t entity)
    {
        if (!contains(entity))
            return;

        size_t sprite_idx = _sparse_indices[entity];
        size_t last_sprite_idx = _dense_entities.size() - 1;
        size_t last_entity = _dense_entities[last_sprite_idx];

        //move last sprite to removed sprite position
        _x[sprite_idx] = _x[last_sprite_idx];
        _y[sprite_idx] = _y[last_sprite_idx];

        //update sparse index for moved entity
        _sparse_indices[last_entity] = sprite_idx;
        _dense_entities[sprite_idx] = last_entity;

        //pop back last sprite data
        _x.pop_back();
        _y.pop_back();
        _dense_entities.pop_back();
        
        _sparse_indices[entity] = -1;
    }

    bool Positions::contains(size_t entity)
    {
        return entity <= _sparse_indices.size() && _sparse_indices[entity] != -1;
    }


    //-------------------------- Velocities --------------------------

     Velocities::Velocities()
     {
        _vx.reserve(256);
        _vy.reserve(256);

        _sparse_indices.resize(1024, -1); //initialize sparse indices to -1 (invalid)
        _dense_entities.reserve(1024);
     }

     void Velocities::add(size_t entity, float x, float y)
     {
         if (contains(entity))
                return;

         if (entity >= _sparse_indices.size())
         {
             size_t new_size = std::max(_sparse_indices.size() * 2, entity + 1);
             _sparse_indices.resize(new_size, -1);
         }

        size_t dense = _dense_entities.size();
        _sparse_indices[entity] = dense;
            
        _vx.push_back(x);
        _vy.push_back(y);

        _dense_entities.push_back(entity);
     }

    void Velocities::remove(size_t entity)
    {
        if (!contains(entity))
            return;

        size_t sprite_idx = _sparse_indices[entity];
        size_t last_sprite_idx = _dense_entities.size() - 1;
        size_t last_entity = _dense_entities[last_sprite_idx];

        //move last sprite to removed sprite position
        _vx[sprite_idx] = _vx[last_sprite_idx];
        _vy[sprite_idx] = _vy[last_sprite_idx];

        //update sparse index for moved entity
        _sparse_indices[last_entity] = sprite_idx;
        _dense_entities[sprite_idx] = last_entity;

        //pop back last sprite data
        _vx.pop_back();
        _vy.pop_back();
        _dense_entities.pop_back();
        
        _sparse_indices[entity] = -1;
    }

    bool Velocities::contains(size_t entity)
    {
        return entity <= _sparse_indices.size() && _sparse_indices[entity] != -1;
    }


    // -------------------------- Movement System --------------------------

    void movment(ECS::World& world, float dt)
    {
        auto& positions = world.storage_registry.get<Positions>();
        auto& velocities = world.storage_registry.get<Velocities>();

        if(velocities.size_vx() != positions.size_x() ||
           velocities.size_vy() != positions.size_y() )
            return; //error handling

        for (size_t i = 0; i < positions.size_x(); i++) 
        {
            positions.set(
                positions.,
                positions.get_x(i) + (velocities.get_vx(i) * dt),
                positions.get_entity_y(i) + (velocities.get_vy(i) * dt)
            );
        }
    }
}
