#pragma once

#include <map>
#include <vector>
#include <typeindex>
#include <typeinfo>


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
		std::map<std::type_index, void*> _storages;
	
	public:
		template<typename T>
		void add()
		{
			//for now im using new but later try change it for arena allocations
			//(prolly most storages have same life span but gotta double check)
			_storages[typeid(T)] = new T{};
		}

		template<typename T>
        T& get()
        {
            return *static_cast<T*>(_storages.at(typeid(T)));
        }

        template<typename T>
        bool contains() const
        {
            return _storages.contains(typeid(T));
        }
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
	};
}	
