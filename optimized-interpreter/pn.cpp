#include "pn.h"

#include <stdexcept>
#include <list>
#include <map>
#include <string>

#include <iostream>

#include <cassert>


PnGenerator::PnGenerator()
{
    treeOptimizers = std::vector<TreeOptimizer*>({
            new OptimizerExpression(),
            new OptimizerIf(),
            new OptimizerWhile(),
            new OptimizerDoWhile()
        });
}
PnGenerator::~PnGenerator()
{
    for (auto o : treeOptimizers) {
        delete o;
    }
}

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


std::vector<PnToken> PnGenerator::generate(std::vector<Token> tokens)
{
    pn = {};
    Parser p;
    std::shared_ptr<ParserTree> tree(p.parse(tokens));
    assert(tree.get()->type == ParserTree::lang);
    bool wasOptimized = false;
    do {
        wasOptimized = false;
        for (auto o : treeOptimizers) {
            if (o->optimize(tree.get())) {
                wasOptimized = true;
            }
        }
    } while (wasOptimized);
    generateLang(tree.get());

    return pn;
}


void PnGenerator::generateLang(ParserTree* t)
{
    assert(t->type == ParserTree::lang);
    for (Unit &u : t->units) {
        assert(u.isTree());
        ParserTree* tree = u.getTree();
        if (tree->type == ParserTree::expression) {
            generateExpression(tree);
        }
        else if (tree->type == ParserTree::function_definition) {
            generateFunctionDefinition(tree);
        }
        else {
            assert(false);
        }
    }
}

void PnGenerator::generateExpression(ParserTree* t)
{
    assert(t->type == ParserTree::expression);
    assert(t->units.size() == 1);
    assert(t->units[0].isTree());
    ParserTree* tree = t->units[0].getTree();
    if (tree->type == ParserTree::final_value_expr) {
        generateFinalValueExpr(tree);
    }
    else if (tree->type == ParserTree::while_block) {
        generateWhile(tree);
    }
    else if (tree->type == ParserTree::do_while) {
        generateDoWhile(tree);
    }
    else if (tree->type == ParserTree::if_else) {
        generateIfElse(tree);
    }
    else if (tree->type == ParserTree::return_expr) {
        generateReturnExpr(tree);
    }
    else {
        assert(false);
    }
}

void PnGenerator::generateFinalValueExpr(ParserTree* tree)
{
    assert(tree->type == ParserTree::final_value_expr);
    assert(tree->units.size() == 2);
    assert(tree->units[0].isTree());
    ParserTree* ve = tree->units[0].getTree();
    assert(ve->type == ParserTree::value_expr);
    assert(tree->units[1].isToken());
    Token eol = tree->units[1].getToken();
    generateValueExpr(ve);
    pn.push_back(PnToken(eol)); // добавление в pn ";"
}


bool tokenIsOperator(Token::Type type)
{
    return type == Token::LOGIC_OP || type == Token::COMPARE_OP
        || type == Token::MATH_OP || type == Token::ASSIGN_OP;
}
void PnGenerator::generateValueExpr(ParserTree* t)
{
    // value_expr состоит из
    //   assign_expr, function_call, value, ( value_expr )
    //   [ оператор value_expr ]
    // Алгоритм рекурсивный
    // ve инициализируется как t
    // 1. Если левый операнд - value, то добавить в pn токен из value
    //    Иначе вызвать генератор соответствующего типа
    // 2. Если есть оператор, добавить его на стек операторов
    //    (либо вытолкнуть со стека необходимое количество операторов и добавить туда новый)
    // 3. Если есть оператор, то присвоить ve значение правого операнда
    //    и повторить шаги 1-3.
    assert(t->type == ParserTree::value_expr);
    std::list<PnToken> operators;
    ParserTree *ve = t;

    while (ve) {
        unsigned int unitsCountToContinue;
        if (ve->units[0].isToken() &&
            ve->units[0].getToken().type == Token::BRACKET_OPEN) {
            // val_expr имеет структуру BR_OP val_expr BR_CL [ оператор val_expr ]
            unitsCountToContinue = 5;
            ParserTree *ve2;
            assert(ve->units[1].isTree());
            ve2 = ve->units[1].getTree();
            assert(ve2->type == ParserTree::value_expr);
            generateValueExpr(ve2);
        }
        else {
            // std::cout << "val expr: not brack open\n";
            unitsCountToContinue = 3;
            assert(ve->units[0].isTree());
            ParserTree *l = ve->units[0].getTree();
            if (l->type == ParserTree::value) {
                assert(l->units.size() == 1);
                assert(l->units[0].isToken());
                // std::cout << "adding value : '" << l->units[0].getToken().text << "'\n";
                pn.push_back(PnToken(l->units[0].getToken()));
            }
            else if (l->type == ParserTree::assign_expr) {
                generateAssignExpr(l);
            }
            else if (l->type == ParserTree::function_call) {
                generateFunctionCall(l);
            }
            else {
                assert(false);
            }
        }
        if (ve->units.size() == unitsCountToContinue) {
            assert(ve->units[unitsCountToContinue-2].isTree());
            ParserTree *op = ve->units[unitsCountToContinue-2].getTree();
            assert(ve->units[unitsCountToContinue-1].isTree());
            assert(ve->units[unitsCountToContinue-1].getTree()->type
                   == ParserTree::value_expr);
            auto tokens = op->getAllTokens();
            assert(tokens.size() == 1);
            assert(tokenIsOperator(tokens[0].type));
            PnToken o(tokens[0]);
            int p = priorities[o.text];
            while (operators.size() &&
                   priorities[operators.back().text] <= p) {
                pn.push_back(operators.back());
                operators.pop_back();
            }
            operators.push_back(o);
            ve = ve->units[unitsCountToContinue-1].getTree();
        }
        else {
            ve = nullptr;
        }
    }
    while (operators.size()) {
        pn.push_back(operators.back());
        operators.pop_back();
    }
}

void PnGenerator::generateAssignExpr(ParserTree* t)
{
    // var = expression -> var, expression_pn, =
    assert(t->type == ParserTree::assign_expr);
    assert(t->units.size() == 3);
    assert(t->units[0].isToken());
    assert(t->units[1].isToken());
    assert(t->units[2].isTree());
    Token var = t->units[0].getToken();
    assert(var.type == Token::VAR);
    Token op = t->units[1].getToken();
    assert(op.type == Token::ASSIGN_OP);
    pn.push_back(PnToken(var));
    ParserTree* ve = t->units[2].getTree();
    assert(ve->type == ParserTree::value_expr);
    generateValueExpr(ve);
    pn.push_back(PnToken(op));
}

void PnGenerator::generateBlock(ParserTree* tree)
{
    assert(tree->type == ParserTree::block);

    pn.push_back(PnToken("#ADD_SCOPE", PnToken::ADD_SCOPE));

    if (tree->units.size() == 1) {
        assert(tree->units[0].isTree());
        ParserTree* e = tree->units[0].getTree();
        assert(e->type == ParserTree::expression);
        generateExpression(e);
    }
    else {
        assert(tree->units.size() >= 2);
        assert(tree->units[0].isToken());
        assert(tree->units[0].getToken().type == Token::BEGIN);
        assert(tree->units[tree->units.size()-1].isToken());
        assert(tree->units[tree->units.size()-1].getToken().type
               == Token::END);
        for (int i = 1; i < (int)tree->units.size() - 1; i++) {
            assert(tree->units[i].isTree());
            ParserTree* e = tree->units[i].getTree();
            assert(e->type == ParserTree::expression);
            generateExpression(e);
        }
    }

    pn.push_back(PnToken("#REMOVE_SCOPE", PnToken::REMOVE_SCOPE));
}

void PnGenerator::generateIfElse(ParserTree* tree)
{
    // 1. добавляем значение из условия
    // 2. добавляем переход по лжи в конец if ..
    // 3. если есть else, то в конце if добавить переход в конец блока else

    assert(tree->type == ParserTree::if_else);
    assert(tree->units.size() >= 3);

    assert(tree->units[0].isToken());
    assert(tree->units[0].getToken().type == Token::IF);
    assert(tree->units[1].isTree());
    ParserTree* cond = tree->units[1].getTree();
    assert(cond->type == ParserTree::condition);
    pn.push_back(PnToken("#ADD_SCOPE", PnToken::ADD_SCOPE));

    generateCondition(cond);
    int labelIndex = pn.size();
    pn.push_back(PnToken("#LABEL", PnToken::LABEL));
    pn.push_back(PnToken("!F", PnToken::GOTO_IF_FALSE));

    assert(tree->units[2].isTree());
    ParserTree* b = tree->units[2].getTree();
    assert(b->type == ParserTree::block);
    generateBlock(b);
    
    if (tree->units.size() == 3) {
        // без else
        pn[labelIndex] = PnToken(std::to_string(int(pn.size()) - labelIndex - 1),
                                 PnToken::LITERAL);
    }
    else {
        // с else
        int secondLabel = pn.size();
        pn.push_back(PnToken("#LABEL", PnToken::LABEL));
        pn.push_back(PnToken("!!", PnToken::GOTO));
        pn[labelIndex] = PnToken(std::to_string(int(pn.size()) - labelIndex - 1),
                                 PnToken::LITERAL);
        assert(tree->units.size() == 5);
        assert(tree->units[4].isTree());
        ParserTree *b2 = tree->units[4].getTree();
        assert(b2->type == ParserTree::block);
        generateBlock(b2);
        pn[secondLabel] = PnToken(std::to_string(int(pn.size()) - labelIndex - 1),
                                  PnToken::LITERAL);
    }
    pn.push_back(PnToken("#REMOVE_SCOPE", PnToken::REMOVE_SCOPE));
}

void PnGenerator::generateWhile(ParserTree* tree)
{
    // Генерация условия, после него - переход по лжи в конец цикла
    // В конце тела цикла безусловный переход к условию
    assert(tree->type == ParserTree::while_block);
    assert(tree->units.size() == 3);
    assert(tree->units[0].isToken());
    assert(tree->units[0].getToken().type == Token::WHILE);
    assert(tree->units[1].isTree());
    ParserTree *cond = tree->units[1].getTree();
    assert(cond->type == ParserTree::condition);
    assert(tree->units[2].isTree());
    ParserTree *b = tree->units[2].getTree();
    assert(b->type == ParserTree::block);

    pn.push_back(PnToken("#ADD_SCOPE", PnToken::ADD_SCOPE));
    int condIndex = pn.size();
    generateCondition(cond);
    int labelIndex = pn.size();
    pn.push_back(PnToken("#LABEL", PnToken::LABEL));
    pn.push_back(PnToken("!F", PnToken::GOTO_IF_FALSE));
    generateBlock(b);
    pn.push_back(PnToken(std::to_string(condIndex - int(pn.size()) - 1),
                         PnToken::LITERAL));
    pn.push_back(PnToken("!!", PnToken::GOTO_IF_FALSE));
    pn[labelIndex] = PnToken(std::to_string(pn.size() - labelIndex),
                             PnToken::LITERAL);
    pn.push_back(PnToken("#REMOVE_SCOPE", PnToken::REMOVE_SCOPE));
}

void PnGenerator::generateDoWhile(ParserTree* tree)
{
    // после условия добавить переход по лжи в место после следующих двух токенов:
    //   места перехода в начало и безусловного перехода
    assert(tree->type == ParserTree::do_while);
    assert(tree->units.size() == 5);
    assert(tree->units[0].isToken());
    assert(tree->units[0].getToken().type == Token::DO);
    assert(tree->units[1].isTree());
    ParserTree *b = tree->units[1].getTree();
    assert(b->type == ParserTree::block);
    assert(tree->units[2].isToken());
    assert(tree->units[2].getToken().type == Token::WHILE);
    assert(tree->units[3].isTree());
    ParserTree *cond = tree->units[3].getTree();
    assert(cond->type == ParserTree::condition);
    assert(tree->units[4].isToken());
    assert(tree->units[4].getToken().type == Token::END_OF_LINE);
    pn.push_back(PnToken("#ADD_SCOPE", PnToken::ADD_SCOPE));

    int bodyBegin = pn.size();

    generateBlock(b);
    generateCondition(cond);

    int labelIndex = pn.size();
    pn.push_back(PnToken("#LABEL", PnToken::LABEL));
    pn.push_back(PnToken("!F", PnToken::GOTO_IF_FALSE));
    pn.push_back(PnToken(std::to_string(bodyBegin - int(pn.size()) - 1),
                         PnToken::LITERAL));
    pn.push_back(PnToken("!!", PnToken::GOTO));

    pn[labelIndex] = PnToken(std::to_string(pn.size() - labelIndex),
                             PnToken::LITERAL);

    pn.push_back(PnToken("#REMOVE_SCOPE", PnToken::REMOVE_SCOPE));
}

void PnGenerator::generateFunctionCall(ParserTree* tree)
{
    // структура FUNCALL:
    // arg1 arg2 argsCount returnTo FUNCALL
    // FUNCALL.text == functionName
    // Аргументы arg1, arg2, ..., argN и argsCount добавляются в генераторе argumentList
    // Если список аргументов отсутствует, то этот метод ставит argsCount = 0
    // 1. копирование аргументов
    // 2. добавление перехода в начало функции
    // 3. конкретное значение адреса перехода определяется во время выполнения
    assert(tree->type == ParserTree::function_call);

    assert(tree->units.size() >= 3);
    assert(tree->units[0].isToken());
    Token var = tree->units[0].getToken();
    assert(var.type == Token::VAR);
    assert(tree->units[1].isToken());
    assert(tree->units[1].getToken().type == Token::BRACKET_OPEN);
    assert(tree->units[tree->units.size()-1].isToken());
    assert(tree->units[tree->units.size()-1].getToken().type
           == Token::BRACKET_CLOSE);
    
    if (tree->units.size() != 4) {
        // нет аргументов
        assert(tree->units.size() == 3);
        pn.push_back(PnToken("0", PnToken::LITERAL));
    }
    else {
        assert(tree->units[2].isTree());
        ParserTree* args = tree->units[2].getTree();
        assert(args->type == ParserTree::argument_list);
        generateArgumentList(args);
    }
    // 3 + GET_TOKEN_POSITION дадут номер позиции после funcall
    pn.push_back(PnToken("3", PnToken::LITERAL));
    pn.push_back(PnToken("#GET_TOKEN_POSITION", PnToken::GET_TOKEN_POSITION));
    pn.push_back(PnToken("+", PnToken::MATH_OP));
    pn.push_back(PnToken(var.text, PnToken::FUNCALL));
}

void PnGenerator::generateFunctionDefinition(ParserTree* tree)
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
    
    assert(tree->type = ParserTree::function_definition);
    assert(tree->units.size() >= 7);
    // забил на ассерты
    assert(tree->units[3].isTree());
    ParserTree *args = tree->units[3].getTree();
    assert(args->type == ParserTree::function_def_args);
    pn.push_back(PnToken("#FUNCTION_DEFINITION", PnToken::FUNCTION_DEFINITION));
    generateFunctionDefArgs(args);

    assert(tree->units[1].isToken());
    Token var = tree->units[1].getToken();
    assert(var.type == Token::VAR);

    pn.push_back(PnToken(var.text, PnToken::FUNCTION_START));
    for (int i = 6; i < (int)tree->units.size() - 1; i++) {
        assert(tree->units[i].isTree());
        ParserTree *e = tree->units[i].getTree();
        assert(e->type == ParserTree::expression);
        generateExpression(e);
    }

    // Добавление оператора return в конец тела функции (даже если был внутри)
    pn.push_back(PnToken("#return_to", PnToken::VAR));
    // По умолчанию возвращать 0
    pn.push_back(PnToken("0", PnToken::LITERAL));
    pn.push_back(PnToken("return", PnToken::RETURN_OP));
    pn.push_back(PnToken("#FUNCTION_END", PnToken::FUNCTION_END));
}

void PnGenerator::generateFunctionDefArgs(ParserTree* tree)
{
    // Перед списком явных аргументов добавляет #return_to, 0, =, ;
    // после списка аргументов добавляет количество аргументов
    
    assert(tree->type == ParserTree::function_def_args);
    int argsCount = 1;
    pn.push_back(PnToken("#return_to", PnToken::VAR));
    pn.push_back(PnToken("0", PnToken::LABEL));
    pn.push_back(PnToken("=", PnToken::ASSIGN_OP));
    pn.push_back(PnToken(";", PnToken::END_OF_LINE));
    for (int i = 0; i < (int)tree->units.size(); i += 2) {
        // i += 2 - пропускается COMMA между аргументами
        argsCount++;
        assert(tree->units[i].isToken());
        Token var = tree->units[i].getToken();
        assert(var.type == Token::VAR);
        pn.push_back(PnToken(var));
        pn.push_back(PnToken(std::to_string(i/2+1),
                             PnToken::LABEL));
        pn.push_back(PnToken("=", PnToken::ASSIGN_OP));
        pn.push_back(PnToken(";", PnToken::END_OF_LINE));
    }
    pn.push_back(PnToken(std::to_string(argsCount),
                         PnToken::LITERAL));
}

void PnGenerator::generateReturnExpr(ParserTree* tree)
{
    // структура оператора RETURN_OP:
    // "#return_to" value RETURN_OP
    assert(tree->type == ParserTree::return_expr);
    assert(tree->units.size() == 2);
    assert(tree->units[0].isToken());
    assert(tree->units[0].getToken().type == Token::RETURN_OP);
    assert(tree->units[1].isTree());
    ParserTree *fve = tree->units[1].getTree();
    assert(fve->type == ParserTree::final_value_expr);

    pn.push_back(PnToken("#return_to", PnToken::VAR));
    generateFinalValueExpr(fve);
    // fve добавляет в конец ";". Ее надо убрать, чтобы не потерять значение
    pn.erase(pn.end()-1);
    pn.push_back(PnToken("return", PnToken::RETURN_OP));
}

void PnGenerator::generateCondition(ParserTree* tree)
{
    assert(tree->type == ParserTree::condition);
    assert(tree->units.size() == 3);
    assert(tree->units[1].isTree());
    ParserTree *ve = tree->units[1].getTree();
    assert(ve->type == ParserTree::value_expr);
    generateValueExpr(ve);
}

void PnGenerator::generateArgumentList(ParserTree *tree)
{
    assert(tree->type == ParserTree::argument_list);
    assert(tree->units.size());
    std::cout << "args tree size: " << tree->units.size() << "\n";
    int argsCount = 0;
    for (int i = 0; i < (int)tree->units.size(); i += 2) {
        // i += 2 чтобы пропускать COMMA между аргументами
        std::cout << "generate val expr\n";
        argsCount++;
        assert(tree->units[i].isTree());
        ParserTree *ve = tree->units[i].getTree();
        assert(ve->type == ParserTree::value_expr);
        generateValueExpr(ve);
    }
    pn.push_back(PnToken(std::to_string(argsCount),
                         PnToken::LITERAL));
}
