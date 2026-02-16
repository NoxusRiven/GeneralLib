#include "..\Collision.h"
#include "..\Movment.h"


namespace Collision
{
    //  -------------------------------- COLLIDED BOXES -------------------------------

    CollidedBoxes::CollidedBoxes()
    {
        entity.reserve(1024);
        pos.reserve(1024);
        _size.reserve(1024);
        solid.reserve(1024);
    }

    void CollidedBoxes::add(size_t e, Rectangle rect, Vector2 offset, bool solid)
    {
        entity.push_back(e);
        pos.push_back({ rect.x, rect.y });
        this->offset.push_back(offset);
        _size.push_back({ rect.width, rect.height });
        this->solid.push_back(solid);
    }

    void CollidedBoxes::clear()
    {
        entity.clear();
        pos.clear();
        offset.clear();
        _size.clear();
        solid.clear();
    }


    // -------------------------------- BOX -------------------------------

    Box::Box(Vector2 size, Vector2 offset, bool solid)
    {
        this->size = size;
        this->offset = offset;
        this->solid = solid;
    }

    // -------------------------------- BOXES --------------------------------

    Boxes::Boxes()
    {
        _size.reserve(2048);
        _solid.reserve(2048);

        _sparse_indices.resize(2048, -1); //initialize sparse indices to -1 (invalid)
        _dense_entities.reserve(2048);
    }

    void Boxes::add(size_t entity, Vector2 size, Vector2 offset, bool solid)
    {
        add_entity(entity);

        _size.push_back(size);
        _solid.push_back(solid);
    }

    void Boxes::remove(size_t entity)
    {
        size_t dense = remove_entity(entity);

        if (dense == (size_t)-1) //biggest size_t value
            return; //entity not found

        size_t last = _dense_entities.size();

        _size[dense] = _size[last];
        _solid[dense] = _solid[last];



        //pop back last sprite data
        _size.pop_back();
        _solid.pop_back();
    }

    // -------------------------------- QUAD TREE --------------------------------
    QuadTree::QuadTree(Rectangle rect, ECS::World& world, const std::vector<size_t>& objects)
    {
        //maybe use arena for allocating nodes
        _root = new QuadNode(0);
        _root->build(rect, world, objects);
    }

    QuadTree::~QuadTree()
    {
        _root->free_children();
        delete _root;
    }

    void QuadTree::search(size_t idx, ECS::World& world, CollidedBoxes& colli_boxes)
    {
        _root->search(idx, world, colli_boxes);
    }


    // -------------------------------- QUAD NODE --------------------------------
    QuadNode::QuadNode(size_t depth) : _bounds({0,0,0,0}), _depth(depth)
    {
        _objects_idx.reserve(1024);
    }

    void QuadNode::split(ECS::World& world, const std::vector<size_t>& objects)
    {
        if (!world.storage_registry.contains<Boxes>() ||
            !world.storage_registry.contains<Movement::Positions>())
        {
            printf("QuadNode::split(): No box or posistion storage in world!\n");
            return;
        }
        auto& boxes = world.storage_registry.get<Boxes>();
        auto& positions = world.storage_registry.get<Movement::Positions>();

        float half_w = _bounds.width/2;
        float half_h = _bounds.height/2;

        Rectangle rects[4];

        rects[0] = { _bounds.x, _bounds.y, half_w, half_h }; //left up
        rects[1] = { _bounds.x + half_w, _bounds.y, half_w, half_h }; //right up
        rects[2]= { _bounds.x, _bounds.y + half_h, half_w, half_h }; //left down
        rects[3]= { _bounds.x + half_w, _bounds.y + half_h, half_w, half_h }; //right down

        std::vector<size_t> split_objects[4];

        for (size_t i = 0; i < objects.size(); i++)
        {
            size_t idx = objects[i];

            float pos_x = positions.get_x(idx);
            float pos_y = positions.get_y(idx); 
            Vector2 size = boxes.size_at(idx);

            Rectangle rect = { pos_x, pos_y, size.x, size.y };

            bool inserted = false;

            for (size_t j = 0; j < 4; j++)
            {
                if (CheckCollisionRecs(rects[j], rect))
                {
                    Rectangle colliRect = GetCollisionRec(rects[j], rect);

                    if(colliRect.width == rect.width &&
                       colliRect.height == rect.height)
                    {
                        split_objects[j].push_back(idx);
                        inserted = true;
                        break;
                    }

                }
            }

            if (!inserted)
            {
                _objects_idx.push_back(idx);
            }

        }

        for (size_t i = 0; i < 4; i++)
        {
            _children[i] = new QuadNode(_depth+1);
            _children[i]->build(rects[i], world, split_objects[i]);
        }
    }

    
    void QuadNode::build(Rectangle rect, ECS::World& world, const std::vector<size_t>& objects)
    {
        _bounds = rect;
        _objects_idx.clear();

        if (objects.size() > QUADTREE_MAX_OBJECTS &&
            (_depth + 1) <= QUADTREE_MAX_DEPTH)
        {
            split(world, objects);
        }
        else
        {
            _objects_idx = objects;
        }

        printf("size of object_idx %zu of node %zu depth\n", _objects_idx.size(), _depth);
    }


    void QuadNode::search(size_t idx, ECS::World& world, CollidedBoxes& colli_boxes)
    {
        if (!world.storage_registry.contains<Boxes>() ||
            !world.storage_registry.contains<Movement::Positions>())
        {
            printf("QuadNode::search(): No box or posistion storage in world!\n");
            return;
        }

        auto& boxes = world.storage_registry.get<Boxes>();
        auto& positions = world.storage_registry.get<Movement::Positions>();


        float pos_x = positions.get_x(idx);
        float pos_y = positions.get_y(idx); 
        Vector2 size = boxes.get_size(idx);
        size_t obj_dense = boxes.dense_index(idx);

        Rectangle query_rect = { pos_x, pos_y, size.x, size.y };

        //check objects in current node
        for (size_t i = 0; i < _objects_idx.size(); i++)
        {
            size_t obj_idx = _objects_idx[i];

            if (obj_idx == obj_dense)
            {
                continue;
            }

            pos_x = positions.get_x(obj_idx);
            pos_y = positions.get_y(obj_idx);
            bool solid = boxes.get_solid(obj_idx);
            Vector2 offset = boxes.get_offset(obj_idx);

            size = boxes.size_at(obj_idx);

            Rectangle rect = { pos_x, pos_y, size.x, size.y };

            if (CheckCollisionRecs(query_rect, rect))
            {
                Rectangle colli_rect = GetCollisionRec(query_rect, rect);
                colli_boxes.add(obj_idx, colli_rect, offset, solid);
            }
        }

        //printf("Result size: %zu\n", result.size());

        //it it has no children leave
        if (is_leaf())
        {
            return;
        }


        //check right children
        for (size_t i = 0; i < 4; i++)
        {
            Rectangle child_rect = _children[i]->get_bounds();

            if (CheckCollisionRecs(child_rect, query_rect))
            {
                _children[i]->search(idx, world, colli_boxes);
            }
        }


    }

    void QuadNode::free_children()
    {
        for (size_t i = 0; i < 4; i++)
        {
            if (_children[i])
            {
                _children[i]->free_children();
                delete _children[i];
                _children[i] = nullptr;

            }
        }
    }


}