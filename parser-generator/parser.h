#ifndef PARSER_H
#define PARSER_H

#include <vector>
#include <memory>
#include <iostream>

#include "lexer.h"

class ParserTree;

typedef std::shared_ptr<ParserTree> TreeRef;
class Unit
{
public:
    Unit(Token token);
    Unit(ParserTree *tree);

    bool isToken();
    bool isTree();

    Token getToken();
    ParserTree* getTree();

private:
    Token token;
    TreeRef tree;

    bool _isToken;
};

class ParserTree
{
public:
    enum Type
        {
            zero_or_one=3, zero_or_more=4,
            group=5, expression=6,
            rule=7, lang=8
        };
    
    ParserTree(Type type,
               std::vector<Unit> units);

    std::vector<Token> getAllTokens();

    Type type;
    std::vector<Unit> units;

    ParserTree* copy();
};


class Parser
{
public:
    Parser();

    ParserTree* parse(std::vector<Token> tokens);

private:
    std::vector<Token> tokens;
    int currentIndex;

    ParserTree* zero_or_one();
    ParserTree* zero_or_more();
    ParserTree* group();
    ParserTree* expression();
    ParserTree* rule();
    ParserTree* lang();

    bool addToken(ParserTree* tree, Token::Type type);
    bool addTree(ParserTree* tree, ParserTree* subtree);
    bool acceptToken(Token::Type type);
};

#endif  // PARSER_H
