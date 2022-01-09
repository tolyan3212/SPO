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
        {WHILE=0,DO=1,RETURN_OP=2,DEF=3,IF=4,ELSE=5,VAR=6,LITERAL=7,ASSIGN_OP=8,MATH_OP=9,COMPARE_OP=10,LOGIC_OP=11,BRACKET_OPEN=12,BRACKET_CLOSE=13,BEGIN=14,END=15,COMMA=16,END_OF_LINE=17,};
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
