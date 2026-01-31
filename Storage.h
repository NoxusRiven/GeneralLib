#pragma once
#include <vector>

namespace ECS
{
    class Storage
    {
    protected:
        std::vector<size_t> _dense_entities;
        std::vector<int32_t> _sparse_indices;
        
        size_t add_entity(size_t entity);
        size_t remove_entity(size_t entity);

    public:
        bool contains(size_t entity) const
        {
            return entity <= _sparse_indices.size() && _sparse_indices[entity] != -1;
        }

        size_t dense_index(size_t entity) const
        {
            return _sparse_indices[entity];
        }

        size_t entity_at(size_t dense_idx) const
        {
            return _dense_entities[dense_idx];
        }

        size_t size() const
        {
            return _dense_entities.size();
        }

        virtual void remove(size_t entity) = 0;
        virtual ~Storage() = default;
    };
}