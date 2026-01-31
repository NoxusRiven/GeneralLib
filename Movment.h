#pragma once
#include <vector>
#include "ECS.h"
#include "Storage.h"

namespace Movement
{
    class Positions : public ECS::Storage
    {
    private:
        std::vector<float> _x;
        std::vector<float> _y;

    public:
        Positions();

        void add(size_t entity, float x, float y);
        void remove(size_t entity) override;

        void set_entity(size_t entity, float x, float y)
        {
            _x[dense_index(entity)] = x;
            _y[dense_index(entity)] = y;
        }

        void set_at(size_t idx, float x, float y)
        {
            _x[idx] = x;
            _y[idx] = y;
        }

        //use for single entity access
        float get_x(size_t entity)
        {
            return _x[dense_index(entity)];
        }

        float get_y(size_t entity)
        {
            return _y[dense_index(entity)];
        }

        //use for iterating over all entities
        float x_at(size_t idx)
        {
            return _x[idx];
        }

        float y_at(size_t idx)
        {
            return _y[idx];
        }
    };

    class Velocities : public ECS::Storage
    {
    private:
        std::vector<float> _vx;
        std::vector<float> _vy;

    public:
        Velocities();

        void add(size_t entity, float vx, float vy);
        void remove(size_t entity) override;

        void set_entity(size_t entity, float vx, float vy)
        {
            _vx[dense_index(entity)] = vx;
            _vy[dense_index(entity)] = vy;
        }

        void set_at(size_t idx, float vx, float vy) 
        {
            _vx[idx] = vx;
            _vy[idx] = vy;
        }

        float get_vx(size_t entity) 
        {
            return _vx[dense_index(entity)];
        }

        float get_vy(size_t entity)
        {
            return _vy[dense_index(entity)];
        }

        float vx_at(size_t idx)
        {
            return _vx[idx];
        }

        float vy_at(size_t idx)
        {
            return _vy[idx];
        }
    };

    void movment(ECS::World& world, float dt);

}
