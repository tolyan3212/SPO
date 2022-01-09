#ifndef LEXER_H
#define LEXER_H

#include <string>
#include <vector>
#include <regex>
#include <iostream>


class Token
{
public:
    enum Type
        {
            // LETTER=1, DIGIT=2,
            VAR=0,
            COMMA=5, DOT=6, EQUAL=7,
            OR=8, BR_OP = 9, BR_CL=10,
            CR_OP=11, CR_CL=12, SQ_OP=13, SQ_CL=14,
            STRING=15, STRING_BEGINNING=16,
        };
    Token(std::string text, Type type);
    Type type;
    std::string text;

    friend std::ostream& operator<<(std::ostream& out, const Token token) {
        out << "{" << token.type << "," << token.text << "}";
        return out;
    }
};


class TokenDefinition
{
public:
    TokenDefinition(std::string re, Token::Type type);
    Token::Type type;
    std::regex re;
};


class Lexer
{
public:
    Lexer();
    std::vector<Token> getTokens(const std::string& text);


private:
    std::vector<TokenDefinition> definitions;
};


#endif  // LEXER_H
