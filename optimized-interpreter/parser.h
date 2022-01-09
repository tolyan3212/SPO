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
        {value=0,op=1,value_expr=2,assign_expr=3,final_value_expr=4,condition=5,block=6,if_else=7,while_block=8,do_while=9,function_call=10,argument_list=11,return_expr=12,function_def_args=13,function_definition=14,expression=15,lang=16,};
    
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

ParserTree* value();
ParserTree* op();
ParserTree* value_expr();
ParserTree* assign_expr();
ParserTree* final_value_expr();
ParserTree* condition();
ParserTree* block();
ParserTree* if_else();
ParserTree* while_block();
ParserTree* do_while();
ParserTree* function_call();
ParserTree* argument_list();
ParserTree* return_expr();
ParserTree* function_def_args();
ParserTree* function_definition();
ParserTree* expression();
ParserTree* lang();


    bool addToken(ParserTree* tree, Token::Type type);
    bool addTree(ParserTree* tree, ParserTree* subtree);
};

#endif  // PARSER_H
