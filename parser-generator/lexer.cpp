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
    definitions = {{"[a-zA-Z_][a-zA-Z_0-9]*",Token::VAR },{",",Token::COMMA },{"\\.",Token::DOT },{"=",Token::ASSIGN },{"\\|",Token::OR },{"\\(",Token::BR_OP },{"\\)",Token::BR_CL },{"\\{",Token::CR_OP },{"\\}",Token::CR_CL },{"\\[",Token::SQ_OP },{"\\]",Token::SQ_CL },{"\"([^\"\n]|(\\\\\"))*\"",Token::STRING },{"\"([^\"\n]|(\\\\\"))*",Token::STRING_BEGINNING },};
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
