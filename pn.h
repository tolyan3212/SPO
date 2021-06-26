#ifndef PN_H
#define PN_H

#include <list>

#include "lexer.h"
#include "parser.h"

class PnGenerator
{
public:
    std::vector<Token> generate(std::vector<Token> tokens);

    enum Operators {
        ADD, SUB, MUL, DIV, ASSIGN, LESS, LESS_EQ,
        GREATER, GREATER_EQ, EQ, UNEQ,
        AND, OR, GOTO, GOTO_IF_NOT, FUNCALL,
    };

private:
    // enum BlockType {
        
    // };
    
    void generateVertex(Parser::Vertex vertex);
    void generateLang(Parser::Vertex vertex);
    void generateExpression(Parser::Vertex vertex);
    void generateFinalValueExpr(Parser::Vertex vertex);
    void generateValueExpr(Parser::Vertex vertex);
    void generateAssignExpr(Parser::Vertex vertex);
    void generateValue(Parser::Vertex vertex);
    void generateBlock(Parser::Vertex vertex);
    void generateIfElse(Parser::Vertex vertex);
    void generateWhile(Parser::Vertex vertex);
    void generateDoWhile(Parser::Vertex vertex);
    void generateCondition(Parser::Vertex vertex);
    void generateFunctionCall(Parser::Vertex vertex);

    // добавляет в pn аргументы в скобках и после - количество аргументов
    void generateArgumentList(Parser::Vertex vertex, bool calledFromOutsize = true);
    void generateOperator(Parser::Vertex vertex);
    void generateFunctionDefinition(Parser::Vertex vertex);
    void generateFunctionDefArgs(Parser::Vertex vertex);
    void generateReturnExpr(Parser::Vertex vertex);

    bool operatorsHasOpenBracket() {
        for (Token t : operators) {
            if (t.type == Token::BRACKET_OPEN) {
                return true;
            }
        }
        return false;
    }

    std::vector<Token> tokens;
    std::vector<Token> pn;
    std::list<Token> operators;

    // нужен для того, чтобы в конце списка аргументов добавлять
    // число - количество этих аргументов
    std::list<int> argumentsCount;
    int currentIndex;
    int labelsCount;
};


#endif  // PN_H
