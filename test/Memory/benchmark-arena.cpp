#include "Arena.h"
#include <chrono>
#include <iostream>
#include <vector>

//Best use with optimization levels on

struct TestStruct
{
    int a;
    float b;
    double c;
};

#define N 1'000'000

void malloc_sim()
{
    auto start = std::chrono::steady_clock::now();

    std::vector<TestStruct*> ptrs;
    ptrs.reserve(N);

    for (size_t i = 0; i < N; ++i) {
        ptrs.push_back(new TestStruct{1, 2.25f, 3.33});
    }

    for (auto p : ptrs)
    {
        delete p;
    }

    auto end = std::chrono::steady_clock::now();
    std::cout << "new/delete: "
              << std::chrono::duration<double>(end-start).count()
              << "s\n";

}

void arena_sim()
{
    Memory::Arena arena(sizeof(TestStruct) * N);

    auto start = std::chrono::steady_clock::now();

    std::vector<TestStruct*> ptrs;
    ptrs.reserve(N);

    for (size_t i = 0; i < N; ++i)
    {
        void* mem = arena.allocate(sizeof(TestStruct), alignof(TestStruct));
        ptrs.push_back(new (mem) TestStruct{1, 2.25f, 3.33});
    }

    arena.deallocate();

    auto end = std::chrono::steady_clock::now();
    std::cout << "arena alloc/dealloc: "
              << std::chrono::duration<double>(end-start).count()
              << "s\n";
}

void arena_benchmark()
{
    malloc_sim();
    arena_sim();
}