#include "JSON_Parser.h"

#include <fstream>
#include <string>

namespace JSON
{
    JSON_Parser::JSON_Parser(const char* file_path)
        : json_file(file_path)
    {
        this->file_path = file_path;
        root.data = j_null{};
    }

    JsonValue JSON_Parser::parse_value()
    {
        Token t = next_token();

        //fix tab with vim xd
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
                std::cerr << "Wrong token in json file";
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

        parse_object();

        /*Token token;
        while ((token = next_token()).type != Token::End)
        {
            
        }*/


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
                while (isdigit(json_file.peek()))
                {
                    json_file.get(c);
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
        j_object& obj = std::get<j_object>(newJsonObject.data);

        Token key = next_token();
        if (key.type != Token::String)
        {
            std::cerr << "Key in json object should be a string\n";
            std::cerr << "Found token type: " << key.type << ", in line: "<< token_count << "\n";
            exit(-1);
        }

        Token colon = next_token();
        if (colon.type != Token::Colon)
        {
            std::cerr << "Key and value should be separated by ':'\n";
            std::cerr << "Found token type: " << colon.type << ", in line: "<< token_count << "\n";
            exit(-1);

        }

        JsonValue value = parse_value();
        obj.insert({key.value, value});

        Token checkIfEnd = next_token();
        while (checkIfEnd.type != Token::RightBrace)
        {
            if(checkIfEnd.type == Token::End)
            {
                std::cerr << "Unexpected end of file while parsing object\n";
                exit(-1);
            }

            if (checkIfEnd.type != Token::Comma)
            {
                std::cerr << "At the end of the key value pair expected ',' or '}'\n";
                std::cerr << "Found token type: " << checkIfEnd.type << ", in line: "<< token_count << "\n";
                std::cerr << "key value: " << key.value << "\n";
                std::cerr << "json value: " << value.to_string() << "\n";
            }

            //adding next key value pair if ',' is present
            JsonValue nextObject = parse_object();

            obj.insert({key.value, nextObject});
        }

        Token nextToken = next_token();
        if (nextToken.type != Token::Colon)
        {
            return newJsonObject;
        }

        //error chunk in json:
        /*
        {
        "frames": [

            {
                "filename": "ghost.png",
                "frame": {
                    "x": 0,
                    "y": 0,
                    "w": 128,
                    "h": 32
                },
           reads comma but expects string (key) for somereason
           */
        JsonValue nextValue = parse_value();

        
        return newJsonObject;

    }

    JsonValue JSON_Parser::parse_array()
    {
        JsonValue newJsonArray(j_array{});
        
        //making JsonValue an object and storing it in temp var
        j_array& arr = std::get<j_array>(newJsonArray.data);

        JsonValue value = parse_value();
        arr.push_back(value);

        Token checkIfEnd = next_token();
        while (checkIfEnd.type != Token::RightBracket)
        {
            if(checkIfEnd.type == Token::End)
            {
                return newJsonArray;
            }

            if (checkIfEnd.type != Token::Comma)
            {
                std::cerr << "At the end of the element ',' or ']'\n";
                std::cerr << "Found token type: " << checkIfEnd.type << "\n";
                exit(-1);
            }

            //adding next key value pair if ',' is present
            JsonValue nextObject = parse_object();

            arr.push_back(nextObject);
        }

        return newJsonArray;
    }
}
