#pragma once

#include "raylib.h"

#include "ECS.h"
#include <vector>

#define QUADTREE_MAX_DEPTH 5
#define QUADTREE_MAX_OBJECTS 10

namespace Collision
{

    struct CollidedBoxes
    {
        std::vector<size_t> entity;
        std::vector<Vector2> pos;
        std::vector<Vector2> offset;
        std::vector<Vector2> _size;
        std::vector<bool> solid;

        CollidedBoxes();
        void add(size_t e, Rectangle rect, Vector2 offset, bool solid);
        void clear();
        size_t size() const
        {
            return entity.size();
        }
    };

    struct Box
    {
        Vector2 size;
        Vector2 offset;
        bool solid;

        Box(Vector2 size, Vector2 offset, bool solid);
    };


    class Boxes : public ECS::Storage
    {
    private:
        std::vector<Vector2> _size;
        std::vector<Vector2> _offset;
        std::vector<bool> _solid;

    public:
        Boxes();

        void add(size_t entity, Vector2 size, Vector2 offset, bool solid);
        void remove(size_t entity) override;

        void set_entity(size_t entity, Vector2 size, Vector2 offset, bool solid)
        {
            size_t idx = dense_index(entity);
            _size[idx] = size;
            _offset[idx] = offset;
            _solid[idx] = solid;
        }

        void set_at(size_t idx, Vector2 size, Vector2 offset, bool solid)
        {
            _size[idx] = size;
            _offset[idx] = offset;
            _solid[idx] = solid;
        }

        //use for single entity access
        Vector2 get_size(size_t entity)
        {
            return _size[dense_index(entity)];
        }

        Vector2 get_offset(size_t entity)
        {
            return _offset[dense_index(entity)];
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

        Vector2 offset_at(size_t idx)
        {
            return _offset[idx];
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
 
        void search(size_t idx, ECS::World& world, CollidedBoxes& colli_boxes);

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

        void search(size_t idx, ECS::World& world, CollidedBoxes& colli_boxes);
    };

}
