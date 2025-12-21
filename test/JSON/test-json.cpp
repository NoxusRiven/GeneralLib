#include <iostream>
#include "..\..\src\JSON_Parser.h"

namespace JSON
{
    int run_test()
    {
        JSON_Parser parser("test.json");
        JsonValue json = parser.parse_file();

        // --- META ---
        std::string name = json["meta"]["name"].get_as<std::string>();
        int version = (int)json["meta"]["version"].get_as<double>();
        bool valid = json["meta"]["valid"].get_as<bool>();

        std::cout << name << " " << version << " " << valid << "\n";
        // EXPECT: test-file 1 1


        // --- ITEMS ---
        int firstId = (int)json["items"][0]["id"].get_as<double>();
        std::string tag1 = json["items"][0]["tags"][1].get_as<std::string>();
        double y = json["items"][0]["data"]["y"].get_as<double>();

        std::cout << firstId << " " << tag1 << " " << y << "\n";
        // EXPECT: 10 b 3.14


        // --- EMPTY STRUCTURES ---
        // Should not crash
        auto& emptyObj = json["emptyObj"];
        auto& emptyArr = json["emptyArr"];

        std::cout << "Empty structures OK\n";


        // --- FLAGS ARRAY ---
        bool f0 = json["flags"][0].get_as<bool>();
        bool f1 = json["flags"][1].get_as<bool>();

        std::cout << f0 << " " << f1 << "\n";
        // EXPECT: 1 0


        // --- NEGATIVE NUMBER ---
        int x = (int)json["items"][0]["data"]["x"].get_as<double>();
        std::cout << x << "\n";
        // EXPECT: -5


        std::cout << "ALL TESTS PASSED\n";
        return 0;
    }
}

