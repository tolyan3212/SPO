#ifndef PN_TOKEN_H
#define PN_TOKEN_H

#include <string>
#include <iostream>

#include "lexer.h"

class PnToken
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
    PnToken(std::string text, Type type);
    PnToken(Token token);
    Type type;
    std::string text;

    bool isOperator()  {
        // проверять, является ли ASSIGN_OPP?
        return type == LOGIC_OP || type == COMPARE_OP || type == MATH_OP;
    }

    friend std::ostream& operator<<(std::ostream& out, const PnToken token) {
        out << "{" << token.type << "," << token.text << "}";
        return out;
    }
};


#endif  // PN_TOKEN_H
