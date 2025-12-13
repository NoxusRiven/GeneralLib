#include "JSON_Parser.h"

#include <fstream>
#include <string>

namespace JSON
{
    Token JSON_Parser::next_token(std::istream& in)
    {
        char c;
        while (in.get(c))
        {
            switch (c)
            {
            case '{': return Token{ Token::LeftBrace };
            case '}': return Token{ Token::RightBrace };
            case '[': return Token{ Token::LeftBracket };
            case ']': return Token{ Token::RightBracket };
            case ':': return Token{ Token::Colon };
            case ',': return Token{ Token::LeftBrace };
            }

            if (c == '"') {
                std::string result;
                while (in.get(c) && c != '"') {
                    result += c;
                }
                return {Token::String, result};
            }

            if (isdigit(c) || c == '-') {
                std::string result(1, c);
                while (isdigit(in.peek())) {
                    in.get(c);
                    result += c;
                }
                return {Token::Number, result};
            }

            if (c == 't' || c == 'T')
            {
                if (in.get() == 'r' && in.get() == 'u' && in.get() == 'e')
                    return {Token::True};
                else
                    throw std::runtime_error("Invalid token: expected 'true'");
                
            }

            if (c == 'f' || c == 'F')
            {
                if (in.get() == 'a' && in.get() == 'l' && in.get() == 's' && in.get() == 'e')
                    return {Token::True};
                else
                    throw std::runtime_error("Invalid token: expected 'false'");
                
            }

        }
        return Token{ Token::End };
    }


    JsonValue JSON_Parser::read_file(const char* file_path)
    {
        std::ifstream json_file(file_path, std::ios::binary);
        JsonValue root;
        root.data = std::monostate{};


        if (!json_file.is_open())
        {
                std::cerr << "Could not open the file - '" << file_path << "'" << std::endl;
                return root;
        }

        if (json_file.peek() != '{')
        {
            std::cerr << "Invalid JSON format in file - '" << file_path << "'" << std::endl;
            return root;
        }

        root.data = std::map<std::string, JsonValue>{};
        Token token;
        while ((token = next_token(json_file)).type != Token::End)
        {
            switch (token.type)
            {
            case Token::LeftBrace:           
                break;
            case Token::RightBrace:
                break;
            case Token::LeftBracket:           
                break;
            case Token::RightBracket:
                break;
            case Token::Colon:           
                break;
            case Token::Comma:
                break;
            case Token::String:           
                break;
            case Token::False:
                break;
            case Token::True:
                break;
            case Token::Null:
                break;
            }
        }


        std::cout << "JSON file '" << file_path << "' read successfully." << std::endl;

        return root;

    }
}
