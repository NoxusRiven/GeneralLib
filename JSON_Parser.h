#pragma once
#include <iostream>
#include <map>
#include <vector>
#include <variant>
#include <fstream>

namespace JSON
{
    struct JsonValue 
    {
        using j_object = std::map<std::string, JsonValue>;
        using j_array = std::vector<JsonValue>;
        using j_null = std::monostate;

        //variant behaves is a better union (from what i know xD)
        std::variant<
            j_null,
            j_object,
            j_array,
            std::string,
            double,
            bool,
        > data;

        JsonValue(j_object obj) : data(obj) {}
        JsonValue(j_array arr) : data(arr) {}
        JsonValue(std::string value) : data(value) {}
        JsonValue(double value) : data(value) {}
        JsonValue(bool value) : data(value) {}
        JsonValue() : data(std::monostate{}) {}
    };

    struct Token
    {
        enum Type
        {
            LeftBrace, RightBrace,
            LeftBracket, RightBracket,
            Colon, Comma,
            String, Number,
            True, False, Null,
            End
        } type;
        std::string value;
    };

    class JSON_Parser
    {
    public:
        JsonValue parse_file();
        JSON_Parser(const char* file_path);

    private:
        const char* file_path;
        std::ifstream json_file;
        JsonValue root;
        Token next_token();
        JsonValue parse_object();
        JsonValue parse_array();
        JsonValue parse_value();


    };
}

