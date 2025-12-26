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

    //works only because of trivial constructor, normaly use placement new
    Test* testObj = static_cast<Test*>(arena.allocate(sizeof(Test), alignof(Test)));
    testObj->a = 3.14;
    testObj->b = 'x';
    testObj->c = 7;

    std::cout << "Allocated Test object: a=" << testObj->a << ", b=" << testObj->b << ", c=" << testObj->c << "\n"; // Expect: a=3.14, b=x, c=7

    testObj->a = 2.71;
    std::cout << "Modified Test object: a=" << testObj->a << ", b=" << testObj->b << ", c=" << testObj->c << "\n"; // Expect: a=2.71, b=x, c=7


    size_t marker = arena.mark();
    std::cout << "Arena marker set at offset: " << marker << "\n";

    Test* anotherTest = new (arena.allocate(sizeof(Test), alignof(Test))) Test{ 1.61, 'y', 42 };

     std::cout << "Allocated Test Anth object: a=" << anotherTest->a << ", b=" << anotherTest->b << ", c=" << anotherTest->c << "\n";
     std::cout << "Another test ptr: " << anotherTest << "\n";

     arena.rollback(marker);
     std::cout << "Arena rolled back to offset: " << marker << "\n";

     Test* overrideTest = new (arena.allocate(sizeof(Test), alignof(Test))) Test{ 1.11, 'h', 11211 };

     std::cout << "Allocated Test Ovr object: a=" << overrideTest->a << ", b=" << overrideTest->b << ", c=" << overrideTest->c << "\n";
     std::cout << "Override test ptr: " << overrideTest << "\n";
     // Expect: same address as anotherTest

     //Should be valid because we just set offset in arena but didnt delete, just overwrote, so u gotta be careful while using arenas
     std::cout << "Another test ptr: " << anotherTest << "\n";
     std::cout << "Allocated Test object: a=" << anotherTest->a << ", b=" << anotherTest->b << ", c=" << anotherTest->c << "\n";
}
