#include "pn.h"

#include <stdexcept>
#include <list>
#include <map>
#include <string>

#include <iostream>

#include <cassert>

std::map<std::string, int> priorities {
    {"+", 4},
    {"-", 4},
    {"*", 3},
    {"/", 3},
    {"<", 6},
    {"<=", 6},
    {">", 6},
    {">=", 6},
    {"==", 7},
    {"!=", 7},
    {"&&", 11},
    {"||", 10},
    {"=", 14},
    {",", 15},
    {"(", 100}, // никогда не выталкивается
    {"!funcall", 1}
};


std::vector<Token> PnGenerator::generate(std::vector<Token> tokens)
{
    pn = {};
    operators = {};
    currentIndex = 0;
    this->tokens = tokens;
    labelsCount = 0;
    Parser p;
    auto v = p.getVertex(this->tokens);
    
    generateVertex(p.getVertex(this->tokens));
    return pn;
}


#define m(t, e) \
    case Parser:: e:\
    generate##t (vertex); \
    break;
void PnGenerator::generateVertex(Parser::Vertex vertex)
{
    switch (vertex.type) {
        m(Lang, LANG);
        m(Expression, EXPRESSION);
        m(FinalValueExpr, FINAL_VALUE_EXPR);
        m(ValueExpr, VALUE_EXPR);
        m(AssignExpr, ASSIGN_EXPR);
        m(Value, VALUE);
        m(Block, BLOCK);
        m(IfElse, IF_ELSE);
        m(While, WHILE);
        m(DoWhile, DO_WHILE);
        m(Condition, CONDITION);
        m(FunctionCall, FUNCTION_CALL);
        m(ArgumentList, ARGUMENT_LIST);
        m(Operator, OPERATOR);
        m(FunctionDefinition, FUNCTION_DEFINITION);
        m(FunctionDefArgs, FUNCTION_DEF_ARGS);
        m(ReturnExpr, RETURN_EXPR);
    }
}
#undef m

void PnGenerator::generateLang(Parser::Vertex vertex)
{
    assert(vertex.type == Parser::LANG);
    for (auto v : vertex.vertices) {
        generateVertex(v);
    }
}

void PnGenerator::generateExpression(Parser::Vertex vertex)
{
    assert(vertex.type == Parser::EXPRESSION);
    generateVertex(vertex.vertices[0]);
}

void PnGenerator::generateFinalValueExpr(Parser::Vertex vertex)
{
    assert(vertex.type == Parser::FINAL_VALUE_EXPR);
    generateValueExpr(vertex.vertices[0]);
    assert(tokens[currentIndex].type == Token::END_OF_LINE);
    pn.push_back(tokens[currentIndex]); // добавление в pn ";"
    currentIndex++;
}

bool tokenIsOperator(Token::Type type)
{
    return type == Token::LOGIC_OP || type == Token::COMPARE_OP
        || type == Token::MATH_OP || type == Token::ASSIGN_OP
        || type == Token::FUNCALL;
}
bool tokenIsValueExpr(Token::Type type)
{
    return type == Token::VAR || type == Token::LITERAL
        || type == Token::BRACKET_OPEN || type == Token::BRACKET_CLOSE
        // || type == Token::COMMA
        || tokenIsOperator(type);
}

void PnGenerator::generateValueExpr(Parser::Vertex vertex)
{
    assert(vertex.type == Parser::VALUE_EXPR);
    std::list<Token> operators;
    std::list<int> functionsArgsCount;
    int vertexIndex = 0; 

    std::vector<Parser::Vertex> childVertices = vertex.vertices;
    
    
    for (; currentIndex < tokens.size()
             && tokenIsValueExpr(tokens[currentIndex].type)
             && !(tokens[currentIndex].type == Token::BRACKET_CLOSE
                  && !operatorsHasOpenBracket())
             && !(currentIndex > 0
                  && tokens[currentIndex-1].type == Token::BRACKET_CLOSE
                  && !tokenIsOperator(tokens[currentIndex].type));
         /* currentIndex++ */) {
        if (tokens[currentIndex].type == Token::BRACKET_OPEN){
            operators.push_back(tokens[currentIndex]);
        }
        else if (tokens[currentIndex].type == Token::BRACKET_CLOSE) {
            while (operators.back().type != Token::BRACKET_OPEN
                   && operators.size()) {
                pn.push_back(operators.back());
                operators.pop_back();
            }
            if (operators.size()) {
                operators.pop_back();
            }
        }
        else if (tokens[currentIndex].type == Token::VAR
                 && currentIndex < tokens.size()-1
                 && tokens[currentIndex+1].type == Token::BRACKET_OPEN) {
            // если это вызов функции
            // while (vertex.vertices[vertexIndex].type != Parser::FUNCTION_CALL) {
                // vertexIndex++;
            // }
            
            while (childVertices[0].type != Parser::FUNCTION_CALL) {
                Parser::Vertex v = childVertices[0];
                childVertices.erase(childVertices.begin());
                childVertices.insert(childVertices.begin(),
                                     v.vertices.begin(),
                                     v.vertices.end());
            }
            assert(childVertices.size() > 0); // то есть в потомках нет вызова функции
            generateFunctionCall(childVertices[0]);
            // for (; vertexIndex < vertex.vertices.size(); vertexIndex++) {
                // if (vertex.vertices[vertexIndex].type == Parser::FUNCTION_CALL) {
                    // generateFunctionCall(vertex.vertices[vertexIndex]);
                    // generateArgumentList(vertex.vertices[vertexIndex]); // формальная вершина
                    // break;
                // }
            // }
            // vertexIndex++;
            // pn.push_back(tokens[currentIndex]);
            // operators.push_back(Token("!funcall", Token::FUNCALL));
        }
        // else if (tokens[currentIndex].type == Token::ASSIGN_OP) {
        // }
        else if (tokenIsOperator(tokens[currentIndex].type)) {
            int p = priorities[tokens[currentIndex].text];
            while (operators.size()
                   && priorities[operators.back().text] <= p) {
                pn.push_back(operators.back());
                operators.pop_back();
            }
            operators.push_back(tokens[currentIndex]);
        }
        else {
            // операнд
            pn.push_back(tokens[currentIndex]);
        }
        if (tokenIsValueExpr(tokens[currentIndex].type)
            && !(tokens[currentIndex].type == Token::BRACKET_CLOSE
                 && !operatorsHasOpenBracket())) {
            // второе условие == если это конце списка аргументов
            currentIndex++;
        }
    }
    while (operators.size()) {
        if (operators.back().type != Token::BRACKET_OPEN)
            pn.push_back(operators.back());
        operators.pop_back();
    }
}

void PnGenerator::generateAssignExpr(Parser::Vertex)
{}
void PnGenerator::generateValue(Parser::Vertex)
{}

void PnGenerator::generateBlock(Parser::Vertex vertex)
{
    assert(vertex.type == Parser::BLOCK);
    assert(tokens[currentIndex].type == Token::BEGIN);
    currentIndex++;

    pn.push_back(Token("", Token::ADD_SCOPE));
    for (auto v : vertex.vertices) {
        generateVertex(v);
    }
    assert(tokens[currentIndex].type == Token::END);
    currentIndex++;
    pn.push_back(Token("", Token::REMOVE_SCOPE));
}

void  PnGenerator::generateIfElse(Parser::Vertex vertex)
{
    // 1. добавляем значение из условия
    // 2. добавляем переход по лжи в конец if ..
    // 3. если есть else, то в конце if добавить переход в конец блока else

    bool withElse = false;
    if (vertex.vertices.size() > 2) {
        withElse = true;
    }
    assert(vertex.type == Parser::IF_ELSE);
    assert(tokens[currentIndex].type == Token::IF);
    currentIndex++;
    pn.push_back(Token("", Token::ADD_SCOPE));
    generateCondition(vertex.vertices[0]);

    int labelIndex = pn.size();
    pn.push_back(Token(std::to_string(labelsCount),
                       Token::LABEL));
    pn.push_back(Token("!F", Token::GOTO_IF_FALSE));
    generateBlock(vertex.vertices[1]);
    if (!withElse) {
        pn[labelIndex] = Token(std::to_string(pn.size() - labelIndex),
                               Token::LITERAL);
    }
    else {
        assert(tokens[currentIndex].type == Token::ELSE);
        currentIndex++;
        int newLabelIndex = pn.size();
        pn.push_back(Token(std::to_string(labelsCount), Token::LABEL));
        pn.push_back(Token("!!", Token::GOTO));
        pn[labelIndex] = Token(std::to_string(pn.size() - labelIndex),
                               Token::LITERAL);
        generateBlock(vertex.vertices[2]);
        pn[newLabelIndex] = Token(std::to_string(pn.size() - newLabelIndex),
                                  Token::LITERAL);
    }
    pn.push_back(Token("", Token::REMOVE_SCOPE));
}

void PnGenerator::generateWhile(Parser::Vertex vertex)
{
    // в условии переход по лжи в конец блока

    assert(vertex.type == Parser::WHILE);
    assert(tokens[currentIndex].type == Token::WHILE);
    currentIndex++;
    pn.push_back(Token("", Token::ADD_SCOPE));
    int condIndex = pn.size();
    generateCondition(vertex.vertices[0]);
    int labelIndex = pn.size();
    pn.push_back(Token("", Token::LABEL));
    pn.push_back(Token("!F", Token::GOTO_IF_FALSE));
    // std::cout << "current token: " << tokens[currentIndex] << "\n";
    generateBlock(vertex.vertices[1]);
    // std::cout << condIndex << "," << pn.size() << condIndex - (int)pn.size() << "\n";
    pn.push_back(Token(std::to_string(condIndex - (int)pn.size() - 1), Token::LITERAL));
    pn.push_back(Token("!!", Token::GOTO));
    pn[labelIndex] = Token(std::to_string(pn.size() - labelIndex),
                           Token::LITERAL);
    pn.push_back(Token("", Token::REMOVE_SCOPE));
}

void PnGenerator::generateDoWhile(Parser::Vertex vertex)
{
    // после условия добавить переход по лжи в место после следующего перехода
    // после первого перехода добавить безусловный переход в начало блока

    assert(vertex.type == Parser::DO_WHILE);
    assert(tokens[currentIndex].type == Token::DO);
    currentIndex++;

    pn.push_back(Token("", Token::ADD_SCOPE));

    int blockBeginIndex = pn.size();
    generateBlock(vertex.vertices[0]);
    assert(tokens[currentIndex].type == Token::WHILE);
    currentIndex++;
    generateCondition(vertex.vertices[1]);
    assert(tokens[currentIndex].type == Token::END_OF_LINE);
    currentIndex++;
    int labelIndex = pn.size();
    pn.push_back(Token("", Token::LABEL));
    pn.push_back(Token("!F", Token::GOTO_IF_FALSE));
    pn.push_back(Token(std::to_string(blockBeginIndex - pn.size()), Token::LITERAL));
    pn.push_back(Token("!!", Token::GOTO));
    pn[labelIndex] = Token(std::to_string(pn.size() - labelIndex), Token::LITERAL);

    pn.push_back(Token("", Token::REMOVE_SCOPE));
}

void PnGenerator::generateCondition(Parser::Vertex vertex)
{
    assert(vertex.type == Parser::CONDITION);
    generateValueExpr(vertex.vertices[0]);
    if (tokens[currentIndex].type == Token::BRACKET_CLOSE) {
        currentIndex++;
    }
}

void PnGenerator::generateFunctionCall(Parser::Vertex vertex)
{
    // структура FUNCALL:
    // arg1 arg2 argsCount returnTo FUNCALL
    // FUNCALL.text == functionName
    // 1. копирование аргументов
    // 2. добавление перехода в начало функции
    // 3. конкретное значение адреса перехода определяется при выполнении проги (?)

    assert(vertex.type == Parser::FUNCTION_CALL);
    assert(tokens[currentIndex].type == Token::VAR);
    std::string functionName = tokens[currentIndex].text;
    currentIndex++;
    assert(tokens[currentIndex].type == Token::BRACKET_OPEN);
    currentIndex++;

    // pn.push_back(Token("", Token::ADD_SCOPE)); // должен делаться в обработке funcall

    // bool hasArgs = tokens[currentIndex].type == Token::BRACKET_CLOSE;
    if (vertex.vertices.size() > 0) { // если есть аргументы
        generateArgumentList(vertex.vertices[0]);
    }
    assert(tokens[currentIndex].type == Token::BRACKET_CLOSE);
    currentIndex++;

    if (vertex.vertices.size() == 0) {
        // если аргументов не было, добавить указание о 0 аргументах
        pn.push_back(Token("0", Token::LITERAL));
    }

    // 3 + позиция токена GET_TOKEN_POSITION дадут номер позиции после funcall
    pn.push_back(Token("3", Token::LITERAL));
    pn.push_back(Token("GET_TOKEN_POSITION", Token::GET_TOKEN_POSITION));
    pn.push_back(Token("+", Token::MATH_OP));
    pn.push_back(Token(functionName, Token::FUNCALL));


    // pn.push_back(Token("", Token::REMOVE_SCOPE)); // должен удаляться в функции
}

void PnGenerator::generateArgumentList(Parser::Vertex vertex,
                                       bool calledFromOutside)
{
    assert(vertex.type == Parser::ARGUMENT_LIST);

    if (calledFromOutside) {
        argumentsCount.push_back(0);
    }
    generateValueExpr(vertex.vertices[0]);
    argumentsCount.back()++;
    if (vertex.vertices.size() > 1) {
        assert(tokens[currentIndex].type == Token::COMMA);
        currentIndex++;
        generateArgumentList(vertex.vertices[1], false);
    }
    if (calledFromOutside) {
        pn.push_back(Token(std::to_string(argumentsCount.back()),
                           Token::LITERAL));
        argumentsCount.pop_back();
    }
}

void PnGenerator::generateOperator(Parser::Vertex)
{}

void PnGenerator::generateFunctionDefinition(Parser::Vertex vertex)
{
    // структура оператора FUNCTION_START:
    // FUNCTION_DEFINITION arg1 label1 = ; arg2 label2 = ; args_count FUNCTION_START /* body */ FUNCTION_END
    // (FUNCTION_START.text = fn_name)
    // FUNCTION_DEFINITION и FUNCTION_END нужны для удаления функции при ее переопределении
    // arg1 - номер токена, в который нужно вернуться после выполнения функции;
    // arg1 имеет имя `#return_to'
    // таким образом, при переходе к функции, происходит переход к FUNCTION_START,
    // по количеству аргументов определяется позиция arg1,
    // label1, label2, ... заменяются на значения аргументов,
    // потом происходит выполнение функции начиная с токена arg1
    // !!! При заходе в FUNCTION_START создается параллельный scope !!!
    assert(vertex.type == Parser::FUNCTION_DEFINITION);
    assert(tokens[currentIndex].type == Token::DEF);
    currentIndex++;
    pn.push_back(Token("FUNCTION_DEFINITION", Token::FUNCTION_DEFINITION));
    assert(tokens[currentIndex].type == Token::VAR);
    Token functionName = tokens[currentIndex];
    currentIndex++;
    assert(tokens[currentIndex].type == Token::BRACKET_OPEN);
    currentIndex++;

    // pn.push_back(Token("ADD_SCOPE", Token::ADD_SCOPE)); // ADD_SCOPE добавляет scope блока, а не функции

    generateFunctionDefArgs(vertex.vertices[0]);

    pn.push_back(Token(functionName.text, Token::FUNCTION_START));

    assert(tokens[currentIndex].type == Token::BRACKET_CLOSE);
    currentIndex++;
    assert(tokens[currentIndex].type == Token::BEGIN);
    currentIndex++;

    for (int i = 1; i < vertex.vertices.size(); i++) {
        generateExpression(vertex.vertices[i]);
    }

    // добавление оператора return в конец даже если он был внутри функции
    pn.push_back(Token("#return_to", Token::VAR));
    pn.push_back(Token("0", Token::LITERAL));
    pn.push_back(Token("return", Token::RETURN_OP));
    assert(tokens[currentIndex].type == Token::END);
    currentIndex++;
    pn.push_back(Token("FUNCTION_END", Token::FUNCTION_END));
}

void PnGenerator::generateFunctionDefArgs(Parser::Vertex vertex)
{
    // в конце добавляет количество аргументов (включая номер куда вернуться после функции)
    assert(vertex.type == Parser::FUNCTION_DEF_ARGS);
    // добавление аргумента, указывающего, куда вернуться после выполнения функции:
    int argsCount = 1;
    pn.push_back(Token("#return_to", Token::VAR));
    pn.push_back(Token("0", Token::LABEL));
    pn.push_back(Token("=", Token::ASSIGN_OP));
    pn.push_back(Token(";", Token::END_OF_LINE));
    if (vertex.vertices.size() > 0) {
        // есть хотя бы один аргумент
        argsCount++;
        assert(tokens[currentIndex].type == Token::VAR);
        pn.push_back(tokens[currentIndex]);
        currentIndex++;
        pn.push_back(Token("1", Token::LABEL));
        pn.push_back(Token("=", Token::ASSIGN_OP));
        pn.push_back(Token(";", Token::END_OF_LINE));
    }
    for (int i = 1; i < vertex.vertices.size(); i++) {
        assert(tokens[currentIndex].type == Token::COMMA);
        argsCount++;
        currentIndex++;
        assert(tokens[currentIndex].type == Token::VAR);
        pn.push_back(tokens[currentIndex]);
        currentIndex++;
        pn.push_back(Token(std::to_string(i+1), Token::LABEL));
        pn.push_back(Token("=", Token::ASSIGN_OP));
        pn.push_back(Token(";", Token::END_OF_LINE));
    }
    pn.push_back(Token(std::to_string(argsCount), Token::LITERAL));
}

void PnGenerator::generateReturnExpr(Parser::Vertex vertex)
{
    // структура оператора RETURN_OP:
    // #return_to value RETURN_OP
    assert(vertex.type == Parser::RETURN_EXPR);
    assert(tokens[currentIndex].type == Token::RETURN_OP);
    currentIndex++;

    pn.push_back(Token("#return_to", Token::VAR));
    generateFinalValueExpr(vertex.vertices[0]);
    assert(pn[pn.size()-1].type == Token::END_OF_LINE);
    pn.pop_back(); // удаление ";" перед "return"
    pn.push_back(Token("return", Token::RETURN_OP));
}
