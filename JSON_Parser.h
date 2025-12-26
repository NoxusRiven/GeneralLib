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
        > _data;

    public:
        JsonValue(const j_object& obj) : _data(obj) {}
        JsonValue(j_object&& obj) : _data(std::move(obj)) {}
        JsonValue(const j_array& arr) : _data(arr) {}
        JsonValue(j_array&& arr) : _data(std::move(arr)) {}
        JsonValue(const std::string& value) : _data(value) {}
        JsonValue(std::string&& value) : _data(std::move(value)) {}
        JsonValue(double value) : _data(value) {}
        JsonValue(bool value) : _data(value) {}
        JsonValue() : _data(j_null{}) {}

        template<typename T>
        T& get_as()
        {
            if (!std::holds_alternative<T>(_data))
            {
                std::cerr << "JsonValue: bad get_as<" << typeid(T).name() << ">\n";
                exit(-1);
            }
            return std::get<T>(_data);
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
    private:
        int _token_count = 0;
        JsonValue _root;
        Token next_token(std::ifstream& json_file);
        JsonValue parse_object(std::ifstream& json_file);
        JsonValue parse_array(std::ifstream& json_file);
        JsonValue parse_value(Token t, std::ifstream& json_file);

    public:
        JsonValue parse_file(const char* json_file);
    };
}

