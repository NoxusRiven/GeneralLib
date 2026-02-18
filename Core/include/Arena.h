#pragma once

#include <iostream>


namespace Memory
{
    class Arena
    {
    private:
        char* _buffer;
        std::size_t _capacity;
        std::size_t _offset;

    public:
        explicit Arena(std::size_t size_bytes)
            : _buffer(static_cast<char*>(::operator new(size_bytes))), _capacity(size_bytes), _offset(0) { }

        ~Arena();
        void* allocate(std::size_t size, std::size_t alignment = alignof(std::max_align_t));
        void reset();
        size_t mark() const;
        void rollback(size_t marker);

    };
}

