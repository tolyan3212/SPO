#include "pn_token.h"

#include <cassert>

PnToken::PnToken(std::string text,
                 Type type)
    : type(type)
    , text(text)
{}

#define e(t) \
    else if (type == Token:: t) {\
        return PnToken:: t;\
    }
PnToken::Type getTypeFromToken(Token token)
{
    Token::Type type = token.type;
    if (type == Token::VAR) {
        return PnToken::VAR;
    }
    e(LITERAL)
        e(BEGIN)
        e(END)
        e(BRACKET_OPEN)
        e(BRACKET_CLOSE)
        e(COMMA)
        e(LOGIC_OP)
        e(COMPARE_OP)
        e(MATH_OP)
        e(ASSIGN_OP)
        e(WHILE)
        e(DO)
        e(IF)
        e(ELSE)
        e(DEF)
        e(END_OF_LINE);
    assert(false);
}
#undef e

PnToken::PnToken(Token token)
    : type(getTypeFromToken(token))
    , text(token.text)
{}
