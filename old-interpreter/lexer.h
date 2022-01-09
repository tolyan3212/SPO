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
            VAR, LITERAL, BEGIN, END, BRACKET_OPEN, BRACKET_CLOSE,
            COMMA, LOGIC_OP, COMPARE_OP, MATH_OP, ASSIGN_OP,
            WHILE, DO, IF, ELSE, DEF, END_OF_LINE,
            GOTO, GOTO_IF_FALSE, FUNCALL, LABEL,
            ADD_SCOPE, REMOVE_SCOPE, GET_TOKEN_POSITION,
            RETURN_OP, FUNCTION_START, FUNCTION_END, FUNCTION_DEFINITION,
        };
    Token(std::string text, Type type);
    Type type;
    std::string text;

    bool isOperator()  {
        // проверять, является ли ASSIGN_OPP?
        return type == LOGIC_OP || type == COMPARE_OP || type == MATH_OP;
    }

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
