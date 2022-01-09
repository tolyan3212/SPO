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
    definitions = {{"while",Token::WHILE },{"do",Token::DO },{"return",Token::RETURN_OP },{"def",Token::DEF },{"if",Token::IF },{"else",Token::ELSE },{"[a-zA-Z_][a-zA-Z0-9_]*",Token::VAR },{"0|[1-9][0-9]*",Token::LITERAL },{"=",Token::ASSIGN_OP },{"[\\+\\-\\*\\/]",Token::MATH_OP },{">|(>=)|<|(<=)|(==)",Token::COMPARE_OP },{"&&|\\|\\|",Token::LOGIC_OP },{"\\(",Token::BRACKET_OPEN },{"\\)",Token::BRACKET_CLOSE },{"\\{",Token::BEGIN },{"\\}",Token::END },{",",Token::COMMA },{";",Token::END_OF_LINE },};
}

std::vector<Token> Lexer::getTokens(const std::string& text)
{
    std::vector<Token> result;
    std::string temp = "";
    for (char c : text) {
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
            temp = std::string();
            if (c != ' ' && c != '\t' && c != '\n')
                temp = std::string() + c;
        }
        else {
            temp = temp2;
        }
    }
    std::smatch match;
    bool ok = true;
    if (temp.size() > 0) {
        ok = false;
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
