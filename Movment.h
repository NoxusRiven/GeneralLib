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

        /*std::vector<int32_t> _sparse_indices;
        std::vector<size_t> _dense_entities;*/

    public:
        Positions();

        void add(size_t e, float x, float y);
        void remove(size_t e);
        bool contains(size_t e);

        size_t size_x() {
            return _x.size();
        }

        size_t size_y() {
            return _y.size();
        }

        void set(size_t entity, float x, float y) {
            _x[_sparse_indices[entity]] = x;
            _y[_sparse_indices[entity]] = y;
        }

        float get_x(size_t idx) {
            return _x[_sparse_indices[idx]];
        }

        float get_y(size_t idx) {
            return _y[_sparse_indices[idx]];
        }

        size_t get_entity(size_t idx) {
            return _dense_entities[idx];
        }

    };

    class Velocities
    {
    private:
        std::vector<size_t> _vx;
        std::vector<size_t> _vy;

        std::vector<int32_t> _sparse_indices;
        std::vector<size_t> _dense_entities;
    public:
        Velocities();

        void add(size_t e, float x, float y);
        void remove(size_t e);
        bool contains(size_t e);

        size_t size_vx() {
            return _vx.size();
        }

        size_t size_vy() {
            return _vy.size();
        }

        void update(size_t idx, float vx, float vy) {
            _vx[_dense_entities[idx]] = vx;
            _vy[_dense_entities[idx]] = vy;
        }

        float get_x(size_t idx) {
            return _vx[_dense_entities[idx]];
        }

        float get_y(size_t idx) {
            return _vy[_dense_entities[idx]];
        }

        size_t get_entity(size_t idx) {
            return _dense_entities[idx];
        }
    };

    void movment(ECS::World& world, float dt);

}
