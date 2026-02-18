#include "ECS.h"

namespace ECS
{
    //------------------------ Storage Registry ------------------------

    void StorageRegistry::remove_entity(size_t entity)
    {
        for (auto& [type, storage] : _storages)
        {
            if (storage->contains(entity))
                storage->remove(entity);
        }
    }

    //------------------------ Entity Manager ------------------------


	Entity EntityManager::create()
    {
        uint32_t id;

        if (!_free_ids.empty())
        {
            id = _free_ids.back();
            _free_ids.pop_back();
        } 
        else
        {
            id = static_cast<uint32_t>(_generations.size());
            _generations.push_back(0);
        }

        printf("Created entity { id:%d, gen:%d }\n", id, _generations[id]);

        return { id, _generations[id] };
    }

    void EntityManager::destroy(Entity e) 
    {
        if (!is_alive(e)) return;

        printf("Destroyed entity { id:%d, gen:%d }\n", e.id, _generations[e.id]);


        _generations[e.id]++;        // invalidate old handles
        _free_ids.push_back(e.id);   // allow reuse
    }

    bool EntityManager::is_alive(Entity e) const 
    {
        return e.id < _generations.size()
            && _generations[e.id] == e.generation;
    }


    //------------------------ World ------------------------
    void World::delete_entity(Entity e)
    {
        entity_manager.destroy(e);

        storage_registry.remove_entity(e.id);
    }
}
