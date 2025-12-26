#include "..\Arena.h"

namespace Memory
{
    Arena::~Arena()
    {
        ::operator delete(_buffer);
    }

    void* Arena::allocate(std::size_t size, std::size_t alignment)
    {
        char* current_ptr = _buffer + _offset;
        std::size_t space = _capacity - _offset;
        void* aligned_ptr = current_ptr;

        // Align the ptr to the specified alignment
        if (std::align(alignment, size, aligned_ptr, space) == nullptr) 
        {
            std::cerr << "Arena: Out of memory or cannot align\n";
            exit(-1);
        }

        // Bump the offset of the arena
        _offset = static_cast<char*>(aligned_ptr) - _buffer + size;

        return aligned_ptr;
    }

    //skipping ptr parameter, as arena frees all at once
    void Arena::deallocate(void* ptr)
    {
        _offset = 0;
    }

    size_t Arena::mark() const
    {
        return _offset;
    }

    void Arena::rollback(size_t marker)
    {
        _offset = marker;
    }
}
