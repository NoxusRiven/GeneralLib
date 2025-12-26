#pragma once

#include <iostream>
#include "Allocator.h"


namespace Memory
{
    class Arena : public Allocator
    {
    private:
        char* _buffer;
        std::size_t _capacity;
        std::size_t _offset;

    public:
        explicit Arena(std::size_t size_bytes)
            : _buffer(static_cast<char*>(::operator new(size_bytes))), _capacity(size_bytes), _offset(0) { }

        ~Arena();
        void* allocate(std::size_t size, std::size_t alignment = alignof(std::max_align_t)) override;
        void deallocate(void* ptr = nullptr) override;
        size_t mark() const;
        void rollback(size_t marker);

    };
}

