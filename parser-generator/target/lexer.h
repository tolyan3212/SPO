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
        {VAR=0,COMMA=1,DOT=2,ASSIGN=3,OR=4,BR_OP=5,BR_CL=6,CR_OP=7,CR_CL=8,SQ_OP=9,SQ_CL=10,STRING=11,STRING_BEGINNING=12,};
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
