#include "..\Storage.h"

namespace ECS
{
    size_t Storage::add_entity(size_t e)
    {
        if (e >= _sparse_indices.size())
{
    size_t new_size = _sparse_indices.size();
    if (new_size == 0) new_size = 1;

    while (new_size <= e)
        new_size *= 2;

    _sparse_indices.resize(new_size, -1);
}

        size_t dense = _dense_entities.size();
        _dense_entities.push_back(e);
        _sparse_indices[e] = dense;
        return dense;
    }

    size_t Storage::remove_entity(size_t e)
    {
        size_t dense = _sparse_indices[e];
        size_t last = _dense_entities.size() - 1;

        _dense_entities[dense] = _dense_entities[last];
        _sparse_indices[_dense_entities[dense]] = dense;

        _dense_entities.pop_back();
        _sparse_indices[e] = -1;

        return dense;
    }
}