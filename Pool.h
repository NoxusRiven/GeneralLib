#pragma once
#include "Allocator.h"

namespace Memory
{
    template<typename T, size_t N>
    class Pool : public Allocator
    {
    private:
        //alignes memory for type T
        alignas(T) std::byte _buffer[sizeof(T) * N];

        //one way linked list for free nodes
        struct Node 
        {
            Node* next;
        };

        Node* _freeList;

    public:

        Pool()
        {
            //castin node on a raw bytes
            _freeList = reinterpret_cast<Node*>(_buffer);

            //saving first node pointer to set up linking
            Node* current = _freeList;

            //linking all nodes, casting byte buffer to node with offset
            for (size_t i = 1; i < N; ++i)
            {
                current->next =
                    reinterpret_cast<Node*>(_buffer + i * sizeof(T));
                current = current->next;
            }

            //we waste one slot to set nullptr to show the end of the list
            current->next = nullptr;
        }

        //use this to construct object in pool
        //Args simulate T constructor parameters
        template<typename... Args>
        void* allocate(Args&&... args) override
        {
            //no free nodes left, return nullptr
            if (!_freeList)
                return nullptr;

            //take first free node, move free list by one
            Node* node = _freeList;
            _freeList = node->next;

            //construct object T with arguments (Args) proviced
            return new (node) T(std::forward<Args>(args)...);
        }

        void deallocate(T* obj) override
        {
            //distroy object
            obj->~T();

            //free previously occupied slot
            Node* node = reinterpret_cast<Node*>(obj);
            node->next = _freeList;
            _freeList = node;
        }
    };
}
