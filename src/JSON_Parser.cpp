#include "JSON_Parser.h"

#include <fstream>
#include <string>


namespace JSON
{
    // ------------------------------------------ JSON VALUE ------------------------------------------
    std::string JsonValue::to_string()
    {
        if (std::holds_alternative<std::string>(data))
        {
            return std::get<std::string>(data);
        }
        return "not string";
    }


    JsonValue& JsonValue::operator[](const std::string& key)
    {
        return std::get<j_object>(data)[key];
    }

    JsonValue& JsonValue::operator[](size_t idx)
    {
        return std::get<j_array>(data)[idx];
    }

    // ------------------------------------------ JSON PARSER ------------------------------------------
    JSON_Parser::JSON_Parser(const char* file_path)
        : json_file(file_path)
    {
        this->file_path = file_path;
    }

    JsonValue JSON_Parser::parse_value(Token t)
    {
        switch (t.type) 
        {
            case Token::LeftBrace:
                return parse_object();
            case Token::LeftBracket:
                return parse_array();
            case Token::String:
                return JsonValue(t.value);
            case Token::Number:
                return JsonValue(std::stod(t.value));
            case Token::True:
                return JsonValue(true);
            case Token::False:
                return JsonValue(false);
            case Token::Null:
                return JsonValue();
            default:
                std::cerr << "Wrong token in json file: " << t.type;
                exit(-1);
        }
    }

    JsonValue JSON_Parser::parse_file()
    {
        if (!json_file.is_open())
        {
                std::cerr << "Could not open the file - '" << file_path << "'" << std::endl;
                return root;
        }

        Token firstFileToken = next_token();
        if (firstFileToken.type != Token::LeftBrace)
        {
            std::cerr << "First TOKEN of JSON file has to be '{'\n";
            return JsonValue();
        }

        root = parse_object();

        std::cout << "JSON file '" << file_path << "' read successfully." << std::endl;

        return root;

    }

    Token JSON_Parser::next_token()
    {
        token_count++;
        char c;
        while (json_file.get(c))
        {
            switch (c)
            {
            case '{': return Token{ Token::LeftBrace };
            case '}': return Token{ Token::RightBrace };
            case '[': return Token{ Token::LeftBracket };
            case ']': return Token{ Token::RightBracket };
            case ':': return Token{ Token::Colon };
            case ',': return Token{ Token::Comma };
            }

            if (c == '"') 
            {
                std::string result;
                while (json_file.get(c) && c != '"')
                {
                    result += c;
                }
                return {Token::String, result};
            }

            if (isdigit(c) || c == '-')
            {
                std::string result(1, c);

                bool dot_encountered = false;
                while (isdigit(json_file.peek()) ||
                       (json_file.peek() == '.' && !dot_encountered))
                {
                    json_file.get(c);
                    if (c == '.')
                        dot_encountered = true;
                    result += c;
                }
                return {Token::Number, result};
            }

            if (c == 't')
            {
                if (json_file.get() == 'r' && json_file.get() == 'u' && json_file.get() == 'e')
                    return {Token::True};
                else
                    throw std::runtime_error("Invalid token: expected 'true'");
                
            }

            if (c == 'f')
            {
                if (json_file.get() == 'a' && json_file.get() == 'l' && json_file.get() == 's' && json_file.get() == 'e')
                    return {Token::False};
                else
                    throw std::runtime_error("Invalid token: expected 'false'");
                
            }

            if (c == 'n')
            {
                if (json_file.get() == 'u' && json_file.get() == 'l' && json_file.get() == 'l')
                    return {Token::Null};
                else
                    throw std::runtime_error("Invalid token: expected 'null'");
            }

        }
        return Token{ Token::End };
    }

    JsonValue JSON_Parser::parse_object()
    {
        JsonValue newJsonObject(j_object{});
        
        //making JsonValue an object and storing it in temp var
        j_object& obj = newJsonObject.get_as<j_object>();
        Token key, colon, t_value;
        JsonValue j_value;
        Token checkIfEnd = next_token();

        if(checkIfEnd.type == Token::RightBrace)
        {
            return newJsonObject;
        }

        //if not empty object, it should contain key so copy it to key token
        key = checkIfEnd;
        if (key.type != Token::String)
        {
            std::cerr << "Key in json object should be a string\n";
            std::cerr << "Found token type: " << key.type << ", in line: "<< token_count << "\n";
            exit(-1);
        }

        colon = next_token();
        if (colon.type != Token::Colon)
        {
            std::cerr << "Key and value should be separated by ':'\n";
            std::cerr << "Found token type: " << colon.type << ", in line: "<< token_count << "\n";
            exit(-1);

        }

        t_value = next_token();
        j_value = parse_value(t_value);
        obj.insert({key.value, j_value});

        
        checkIfEnd = next_token();
        while (checkIfEnd.type != Token::RightBrace)
        {
            if (checkIfEnd.type != Token::Comma)
            {
                std::cerr << "At the end of the key value pair expected ',' or '}'\n";
                std::cerr << "Found token type: " << checkIfEnd.type << ", in line: "<< token_count << "\n";
                std::cerr << "key value: " << key.value << "\n";
                std::cerr << "json value: " << j_value.to_string() << "\n";
                exit(-1);
            }

            key = next_token();
            if (key.type != Token::String)
            {
                std::cerr << "Key in json object should be a string\n";
                std::cerr << "Found token type: " << key.type << ", in line: "<< token_count << "\n";
                exit(-1);
            }

            colon = next_token();
            if (colon.type != Token::Colon)
            {
                std::cerr << "Key and value should be separated by ':'\n";
                std::cerr << "Found token type: " << colon.type << ", in line: "<< token_count << "\n";
                exit(-1);

            }

            t_value = next_token();
            j_value = parse_value(t_value);
            obj.insert({key.value, j_value});

            checkIfEnd = next_token();
        }
        
        return newJsonObject;
    }

    JsonValue JSON_Parser::parse_array()
    {
        JsonValue newJsonArray(j_array{});
        
        //making JsonValue an object and storing it in temp var
        j_array& arr = newJsonArray.get_as<j_array>();

        JsonValue value; 
        Token checkIfEnd = next_token();

        if(checkIfEnd.type == Token::RightBracket)
        {
            return newJsonArray;
        }

        //if code reaches here, checkIfEnd is first value
        //this will handle errors if next token is not a value
        value = parse_value(checkIfEnd);
        
        arr.push_back(value);

        checkIfEnd = next_token();
        while (checkIfEnd.type != Token::RightBracket)
        {
            if (checkIfEnd.type != Token::Comma)
            {
                std::cerr << "At the end of the element ',' or ']'\n";
                std::cerr << "Found token type: " << checkIfEnd.type << ", in line: "<< token_count << "\n";
                exit(-1);
            }

            //adding next key value pair if ',' is present
            Token t = next_token();
            JsonValue nextValue = parse_value(t);

            arr.push_back(nextValue);

            checkIfEnd = next_token();
        }

        return newJsonArray;
    }
}
