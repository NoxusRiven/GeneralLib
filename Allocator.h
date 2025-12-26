#pragma once

#include <cstddef>

namespace Memory
{
    class Allocator
    {
    public:
        virtual ~Allocator() = default;
        virtual void* allocate(std::size_t size, std::size_t alignment = alignof(std::max_align_t)) = 0;
        virtual void deallocate(void* ptr = nullptr) = 0;
    };
}