#ifndef PN_H
#define PN_H

#include <list>

#include "lexer.h"
#include "parser.h"
#include "pn_token.h"

#include "tree_optimizer.h"

class PnGenerator
{
public:
    PnGenerator();
    ~PnGenerator();

    std::vector<PnToken> generate(std::vector<Token> tokens);

    enum Operators {
        ADD, SUB, MUL, DIV, ASSIGN, LESS, LESS_EQ,
        GREATER, GREATER_EQ, EQ, UNEQ,
        AND, OR, GOTO, GOTO_IF_NOT, FUNCALL,
    };

private:
    std::vector<TreeOptimizer*> treeOptimizers;
    // enum BlockType {
        
    // };
    
    // std::vector<Token> tokens;
    std::vector<PnToken> pn;
    // std::list<PnToken> operators;

    // void generateUnit(Unit &u);
    // void generateTree(ParserTree* t);

    void generateValueExpr(ParserTree* t);
    void generateAssignExpr(ParserTree* t);
    void generateFinalValueExpr(ParserTree* t);
    void generateCondition(ParserTree* t);
    void generateBlock(ParserTree* t);
    void generateIfElse(ParserTree* t);
    void generateWhile(ParserTree* t);
    void generateDoWhile(ParserTree* t);
    void generateFunctionCall(ParserTree* t);
    void generateArgumentList(ParserTree* t);
    void generateReturnExpr(ParserTree* t);
    void generateFunctionDefArgs(ParserTree* t);
    void generateFunctionDefinition(ParserTree* t);
    void generateExpression(ParserTree* t);
    void generateLang(ParserTree* t);
    

    // нужен для того, чтобы в конце списка аргументов добавлять
    // число - количество этих аргументов
    // std::list<int> argumentsCount;
    // int currentIndex;
    // int labelsCount;
};


#endif  // PN_H
