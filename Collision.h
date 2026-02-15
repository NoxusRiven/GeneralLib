#pragma once

#include "raylib.h"

#include "ECS.h"
#include <vector>

#define QUADTREE_MAX_DEPTH 5
#define QUADTREE_MAX_OBJECTS 10

namespace Collision
{

    struct Box
    {
        Vector2 size;
        bool solid;

        Box(Vector2 size, bool solid);
    };


    class Boxes : public ECS::Storage
    {
    private:
        std::vector<Vector2> _size;
        std::vector<bool> _solid;

    public:
        Boxes();

        void add(size_t entity, Vector2 size, bool solid);
        void remove(size_t entity) override;

        void set_entity(size_t entity, Vector2 size, bool solid)
        {
            size_t idx = dense_index(entity);
            _size[idx] = size;
            _solid[idx] = solid;
        }

        void set_at(size_t idx, Vector2 size, bool solid)
        {
            _size[idx] = size;
            _solid[idx] = solid;
        }

        //use for single entity access
        Vector2 get_size(size_t entity)
        {
            return _size[dense_index(entity)];
        }

        bool get_solid(size_t entity)
        {
            return _solid[dense_index(entity)];
        }

        //use for iterating over all entities
        Vector2 size_at(size_t idx)
        {
            return _size[idx];
        }

        bool solid_at(size_t idx)
        {
            return _solid[idx];
        }


    };

    class QuadNode
    {
    private:
        Rectangle _bounds;
        std::vector<size_t> _objects_idx;
        const size_t _depth;

        QuadNode* _children[4] = { nullptr,  nullptr, nullptr, nullptr };

        void split(ECS::World& world, const std::vector<size_t>& objects);
    public:
        QuadNode(size_t depth);

        bool is_leaf()
        {
            return _children[0] == nullptr;
        }

        void build(Rectangle rect, ECS::World& world, const std::vector<size_t>& objects);
 
        void search(size_t idx, ECS::World& world, std::vector<size_t>& result);

        void free_children();

        Rectangle get_bounds()
        {
            return _bounds;
        }
    };

    struct QuadTree
    {
        QuadNode* _root;

        QuadTree(Rectangle bounds, ECS::World& world, const std::vector<size_t>& objects);
        ~QuadTree();

        void search(size_t idx, ECS::World& world, std::vector<size_t>& result);
    };

}
