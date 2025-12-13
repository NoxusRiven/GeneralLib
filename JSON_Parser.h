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
        std::variant<
            std::monostate, //null
            std::map<std::string, JsonValue>, //object
            std::vector<JsonValue>, //array
            std::string,
            double,
            bool,
        > data;
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
        JsonValue read_file(const char* file_path);
    private:
        Token next_token(std::istream& in);

    };
}

