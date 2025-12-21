#pragma once
#include <iostream>
#include <map>
#include <vector>
#include <variant>
#include <fstream>
#include <memory>

namespace JSON
{
    struct JsonValue;
    using j_object = std::map<std::string, JsonValue>;
    using j_array = std::vector<JsonValue>;
    using j_null = std::monostate;


    class JsonValue 
    {
        //variant behaves is a better union (from what i know xD)
    private:
        std::variant<
            j_null,
            j_object,
            j_array,
            std::string,
            double,
            bool
        > data;

    public:
        JsonValue(const j_object& obj) : data(obj) {}
        JsonValue(j_object&& obj) : data(std::move(obj)) {}
        JsonValue(const j_array& arr) : data(arr) {}
        JsonValue(j_array&& arr) : data(std::move(arr)) {}
        JsonValue(const std::string& value) : data(value) {}
        JsonValue(std::string&& value) : data(std::move(value)) {}
        JsonValue(double value) : data(value) {}
        JsonValue(bool value) : data(value) {}
        JsonValue() : data(j_null{}) {}

        template<typename T>
        T& get_as()
        {
            if (std::holds_alternative<T>(data))
            {
                return std::get<T>(data);
            }
            else
            {
                std::cerr << "JsonValue does not hold the requested type.\n";
                exit(-1);
            }
        }

        //helper for debugging
        std::string to_string();

        //overloaded operators for convinience
        JsonValue& operator[](const std::string& key);
        JsonValue& operator[](size_t idx);
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
            End = -1
        } type;
        std::string value;
    };

    class JSON_Parser
    {
    public:
        JsonValue parse_file();
        JSON_Parser(const char* file_path);

    private:
        int token_count = 0;
        const char* file_path;
        std::ifstream json_file;
        JsonValue root;
        Token next_token();
        JsonValue parse_object();
        JsonValue parse_array();
        JsonValue parse_value(Token t);
    };
}

