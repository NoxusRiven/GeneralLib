#include <iostream>
#include "..\..\Arena.h"


void run_arena_test()
{
    using Memory::Arena;
    const std::size_t arena_size = 1024; // 1 KB
    Arena arena(arena_size);


    int* int1 = static_cast<int*>(arena.allocate(sizeof(int), alignof(int)));

    *int1 = 42;

    std::cout << "Allocated int: " << *int1 << "\n"; // Expect: 42

    *int1 = 100;

    std::cout << "Modified int: " << *int1 << "\n"; // Expect: 100

    struct Test
    {
        double a;
        char b;
        int c;
    };

    Test* testObj = static_cast<Test*>(arena.allocate(sizeof(Test), alignof(Test)));
    testObj->a = 3.14;
    testObj->b = 'x';
    testObj->c = 7;

    std::cout << "Allocated Test object: a=" << testObj->a << ", b=" << testObj->b << ", c=" << testObj->c << "\n"; // Expect: a=3.14, b=x, c=7

    testObj->a = 2.71;
    std::cout << "Modified Test object: a=" << testObj->a << ", b=" << testObj->b << ", c=" << testObj->c << "\n"; // Expect: a=2.71, b=x, c=7

}
