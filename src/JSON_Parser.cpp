#include "..\JSON_Parser.h"
#include <string>


namespace JSON
{
    // ------------------------------------------ JSON VALUE ------------------------------------------
    std::string JsonValue::to_string()
    {
        if (std::holds_alternative<std::string>(_data))
        {
            return std::get<std::string>(_data);
        }
        return "not string";

    }


    JsonValue& JsonValue::operator[](const std::string& key)
    {
        return std::get<j_object>(_data)[key];
    }

    JsonValue& JsonValue::operator[](size_t idx)
    {
        return std::get<j_array>(_data)[idx];
    }

    // ------------------------------------------ JSON PARSER ------------------------------------------
    JsonValue JSON_Parser::parse_value(Token t, std::ifstream& json_file)
    {
        switch (t.type) 
        {
            case Token::LeftBrace:
                return parse_object(json_file);
            case Token::LeftBracket:
                return parse_array(json_file);
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

    JsonValue JSON_Parser::parse_file(const char *file_path)
    {
        std::ifstream json_file;
        json_file.open(file_path);

        if (!json_file.is_open())
        {
                std::cerr << "Could not open the file - '" << file_path << "'" << std::endl;
                exit(-1);
        }

        Token firstFileToken = next_token(json_file);
        if (firstFileToken.type != Token::LeftBrace)
        {
            std::cerr << "First TOKEN of JSON file has to be '{'\n";
            exit(-1);
        }

        _root = parse_object(json_file);

        std::cout << "JSON file '" << file_path << "' read successfully." << std::endl;

        json_file.close();
        return _root;

    }

    Token JSON_Parser::next_token(std::ifstream& json_file)
    {
        _token_count++;
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

    JsonValue JSON_Parser::parse_object(std::ifstream& json_file)
    {
        JsonValue newJsonObject(j_object{});
        
        //making JsonValue an object and storing it in temp var
        j_object& obj = newJsonObject.get_as<j_object>();
        Token key, colon, t_value;
        JsonValue j_value;
        Token checkIfEnd = next_token(json_file);

        if(checkIfEnd.type == Token::RightBrace)
        {
            return newJsonObject;
        }

        //if not empty object, it should contain key so copy it to key token
        key = checkIfEnd;
        if (key.type != Token::String)
        {
            std::cerr << "Key in json object should be a string\n";
            std::cerr << "Found token type: " << key.type << ", in line: "<< _token_count << "\n";
            exit(-1);
        }

        colon = next_token(json_file);
        if (colon.type != Token::Colon)
        {
            std::cerr << "Key and value should be separated by ':'\n";
            std::cerr << "Found token type: " << colon.type << ", in line: "<< _token_count << "\n";
            exit(-1);

        }

        t_value = next_token(json_file);
        j_value = parse_value(t_value, json_file);
        obj.insert({key.value, j_value});

        
        checkIfEnd = next_token(json_file);
        while (checkIfEnd.type != Token::RightBrace)
        {
            if (checkIfEnd.type != Token::Comma)
            {
                std::cerr << "At the end of the key value pair expected ',' or '}'\n";
                std::cerr << "Found token type: " << checkIfEnd.type << ", in line: "<< _token_count << "\n";
                std::cerr << "key value: " << key.value << "\n";
                std::cerr << "json value: " << j_value.to_string() << "\n";
                exit(-1);
            }

            key = next_token(json_file);
            if (key.type != Token::String)
            {
                std::cerr << "Key in json object should be a string\n";
                std::cerr << "Found token type: " << key.type << ", in line: "<< _token_count << "\n";
                exit(-1);
            }

            colon = next_token(json_file);
            if (colon.type != Token::Colon)
            {
                std::cerr << "Key and value should be separated by ':'\n";
                std::cerr << "Found token type: " << colon.type << ", in line: "<< _token_count << "\n";
                exit(-1);

            }

            t_value = next_token(json_file);
            j_value = parse_value(t_value, json_file);
            obj.insert({key.value, j_value});

            checkIfEnd = next_token(json_file);
        }
        
        return newJsonObject;
    }

    JsonValue JSON_Parser::parse_array(std::ifstream& json_file)
    {
        JsonValue newJsonArray(j_array{});
        
        //making JsonValue an object and storing it in temp var
        j_array& arr = newJsonArray.get_as<j_array>();

        JsonValue value; 
        Token checkIfEnd = next_token(json_file);

        if(checkIfEnd.type == Token::RightBracket)
        {
            return newJsonArray;
        }

        //if code reaches here, checkIfEnd is first value
        //this will handle errors if next token is not a value
        value = parse_value(checkIfEnd, json_file);
        
        arr.push_back(value);

        checkIfEnd = next_token(json_file);
        while (checkIfEnd.type != Token::RightBracket)
        {
            if (checkIfEnd.type != Token::Comma)
            {
                std::cerr << "At the end of the element ',' or ']'\n";
                std::cerr << "Found token type: " << checkIfEnd.type << ", in line: "<< _token_count << "\n";
                exit(-1);
            }

            //adding next key value pair if ',' is present
            Token t = next_token(json_file);
            JsonValue nextValue = parse_value(t, json_file);

            arr.push_back(nextValue);

            checkIfEnd = next_token(json_file);
        }

        return newJsonArray;
    }
}
