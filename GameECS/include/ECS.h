#pragma once

#include <map>
#include <vector>
#include <typeindex>
#include <typeinfo>
#include "Storage.h"


namespace ECS
{
	struct Entity
	{
		uint32_t id;
		uint32_t generation;
    };

	//class that helps dynamicly store storages for wolrd objects
	class StorageRegistry
	{
	private:
		std::map<std::type_index, Storage*> _storages;
	
	public:
		template<class Storage>
		void add()
		{
			//TODO: for now im using new but later try change it for arena allocations
			//(prolly most storages have same life span but gotta double check)
			_storages[typeid(Storage)] = new Storage{};
		}

		template<class Storage>
        Storage& get()
        {
            return *static_cast<Storage*>(_storages.at(typeid(Storage)));
        }

        template<class Storage>
        bool contains() const
        {
            return _storages.contains(typeid(Storage));
        }

        void remove_entity(size_t entity);
	};

    class EntityManager
	{
    private:
        std::vector<uint32_t> _generations;
        std::vector<uint32_t> _free_ids;

    public:
        Entity create();
        void destroy(Entity e);
        bool is_alive(Entity e) const;
    };


	struct World
	{
        EntityManager entity_manager;
		StorageRegistry storage_registry;

        void delete_entity(Entity entity);
	};
}	
