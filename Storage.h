#pragma once

#include <vector>

namespace ECS
{
    //vix spacing with vim
    class Storage
{
protected:
    std::vector<size_t> _dense_entities;
    std::vector<int32_t> _sparse_indices;
    
    size_t add_entity(size_t e);
    size_t remove_entity(size_t e);

public:
    bool contains(size_t e) const
    {
        return e <= _sparse_indices.size() && _sparse_indices[e] != -1;
    }

    size_t dense_index(size_t e) const
    {
        return _sparse_indices[e];
    }

    size_t entity_at(size_t dense_idx) const
    {
        return _dense_entities[dense_idx];
    }

    size_t size() const
    {
        return _dense_entities.size();
    }
};
}