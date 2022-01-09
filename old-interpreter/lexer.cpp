#include "lexer.h"
#include <stdexcept>

Token::Token(std::string text,
             Type type)
    : type(type)
    , text(text)
{}
TokenDefinition::TokenDefinition(std::string re,
                                 Token::Type type)
    : type(type)
    , re(re)
{}


Lexer::Lexer()
{
    // std::cout << "1, \n";
    definitions.push_back(TokenDefinition("\\{",
                                          Token::BEGIN));
    // std::cout << "2, ";
    definitions.push_back(TokenDefinition("\\}",
                                          Token::END));
    // std::cout << "3, \n";
    definitions.push_back(TokenDefinition("\\(",
                                          Token::BRACKET_OPEN));
    // std::cout << "4, \n";
    definitions.push_back(TokenDefinition("\\)",
                                          Token::BRACKET_CLOSE));
    // std::cout << "5, \n";
    definitions.push_back(TokenDefinition(",",
                                          Token::COMMA));
    // std::cout << "6, \n";
    definitions.push_back(TokenDefinition("&&|\\|\\|",
                                          Token::LOGIC_OP));
    // std::cout << "7, \n";
    definitions.push_back(TokenDefinition(">|>=|<|<=|==|!=",
                                          Token::COMPARE_OP));
    // std::cout << "8, \n";
    definitions.push_back(TokenDefinition("[\\+\\-\\*\\/]",
                                          Token::MATH_OP));
    // std::cout << "9, \n";
    definitions.push_back(TokenDefinition("=",
                                          Token::ASSIGN_OP));
    // std::cout << "10, \n";
    definitions.push_back(TokenDefinition("while",
                                          Token::WHILE));
    // std::cout << "11, \n";
    definitions.push_back(TokenDefinition("do",
                                          Token::DO));
    // std::cout << "12, \n";
    definitions.push_back(TokenDefinition("if",
                                          Token::IF));
    // std::cout << "13, \n";
    definitions.push_back(TokenDefinition("else",
                                          Token::ELSE));
    // std::cout << "14, \n";
    definitions.push_back(TokenDefinition("def",
                                          Token::DEF));
    definitions.push_back(TokenDefinition("return",
                                          Token::RETURN_OP));
    // std::cout << "15, \n";
    definitions.push_back(TokenDefinition(";",
                                          Token::END_OF_LINE));
    // std::cout << "16, \n";
    definitions.push_back(TokenDefinition("[a-zA-Z_][a-zA-Z0-9_]*",
                                          Token::VAR));
    // std::cout << "17, \n";
    definitions.push_back(TokenDefinition("0|[1-9][0-9]*",
                                          Token::LITERAL));
    // std::cout << "18, \n";
    
}

std::vector<Token> Lexer::getTokens(const std::string& text)
{
    std::vector<Token> result;
    std::string temp = "";
    for (char c : text) {
        // std::cout << "current vector: ";
        // for (Token& t : result) {
            // std::cout << t << ",";
        // }
        // std::cout << "\n";
        if (temp == " " || temp == "\n" || temp == "\t")
            temp = "";
        // std::cout << "current temp: " << temp << "\n";
        // std::cout << "next symbol: " << c << "\n";
        
        std::string temp2 = temp + c;
        if (temp2 == " " || temp2 == "\n" || temp2 == "\t")
            continue;
        std::smatch match;
        bool newIsOk = false;
        for (TokenDefinition def : definitions) {
            if (std::regex_match(temp2, match, def.re)) {
                newIsOk = true;
                break;
            }
        }
        if (!newIsOk) {
            bool ok = false;
            for (TokenDefinition def : definitions) {
                if (std::regex_match(temp, match, def.re)) {
                    result.push_back(Token(temp, def.type));
                    ok = true;
                    break;
                }
            }
            if (!ok) {
                throw std::runtime_error("Unable to make a token");
            }
            temp = std::string() + c;
        }
        else {
            temp = temp2;
        }
    }
    std::smatch match;
    bool ok = false;
    if (temp.size() > 0) {
        for (TokenDefinition def : definitions) {
            if (std::regex_match(temp, match, def.re)) {
                result.push_back(Token(temp, def.type));
                ok = true;
                break;
            }
        }
    }
    if (!ok) {
        throw std::runtime_error("Unable to make a token");
    }
    return result;
}
