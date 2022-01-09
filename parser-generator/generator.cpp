#include "generator.h"

#include "string_template.h"

#include <fstream>
#include <cassert>

// 1 - тип лексемы
// 2 - код для вычисления tree
const std::string methodTemplate {
    "\
ParserTree* Parser::#{1}() {\n\
    ParserTree* tree = new ParserTree(ParserTree::#{1}, {});\n\
\n\
#{2}\n\
    if (tree->units.size()) {\n\
        return tree;\n\
    }\n\
    return nullptr;\n\
}\n"
};

const std::string ifTemplate {
    "if (#{1}) {\n\
    #{2}\n\
}\n"
};

const std::string ifElseTemplate {
    "if (#{1}) {\n\
    #{2}\n\
}\n\
else {\n\
    #{3}\n\
}\n"
};

// 1 - порядковый номер проверки
// 2 - код для вычисления успешности первого выражения
// 3 - успешность первого выражения
// 4 - код для вычисления успешности второго выражения
// 5 - успешность второго выражения
const std::string andTemplate {
    "\
// Template: AND \n\
bool b#{1};\n\
int index#{1} = currentIndex;\n\
ParserTree* copy#{1} = tree->copy();\n\
\n\
#{2}\n\
\n\
b#{1} = #{3};\n\
if (b#{1}) {\n\
    #{4}\n\
    b#{1} = b#{1} && #{5};\n\
    if (b#{1}) {}\n\
    else {\n\
        currentIndex = index#{1};\n\
        delete tree;\n\
        tree = copy#{1}->copy();\n\
    }\n\
}\n\
else {\n\
    currentIndex = index#{1};\n\
    delete tree;\n\
    tree = copy#{1}->copy();\n\
}"
};

// 1 - порядковый номер проверки
// 2 - код для вычисления успешности первого выражения
// 3 - успешность первого выражения
// 4 - код для вычисления успешности второго выражения
// 5 - успешность второго выражения
const std::string orTemplate {
    "\
// Template: OR \n\
bool b#{1};\n\
int index#{1} = currentIndex;\n\
ParserTree* copy#{1} = tree->copy();\n\
\n\
#{2}\n\
\n\
b#{1} = #{3};\n\
if (b#{1}) {}\n\
else {\n\
    #{4}\n\
    b#{1} = b#{1} || #{5};\n\
    if (b#{1}) {}\n\
    else {\n\
        currentIndex = index#{1};\n\
        delete tree;\n\
        tree = copy#{1}->copy();\n\
    }\n\
}"
};

// 1 - порядковый номер проверки
// 2 - код для вычисления успешности выражения группы
// 3 - успешность выражения группы
const std::string groupTemplate {
    "\
// Template: GROUP \n\
bool b#{1};\n\
int index#{1} = currentIndex;\n\
ParserTree* copy#{1} = tree->copy();\n\
\n\
#{2}\n\
\n\
b#{1} = #{3};\n\
if (b#{1}) {}\n\
else {\n\
    currentIndex = index#{1};\n\
    delete tree;\n\
    tree = copy#{1}->copy();\n\
}"
};

// 1 - порядковый номер проверки
// 2 - код для вычисления успешности выражения группы
// 3 - успешность выражения группы
const std::string zeroOrOneTemplate {
    "\
// Template: ZERO_OR_ONE \n\
bool b#{1};\n\
int index#{1} = currentIndex;\n\
ParserTree* copy#{1} = tree->copy();\n\
\n\
#{2}\n\
\n\
b#{1} = #{3};\n\
if (b#{1}) {\n\
    copy#{1} = tree->copy();\n\
    index#{1} = currentIndex;\n\
}\n\
currentIndex = index#{1};\n\
delete tree;\n\
tree = copy#{1}->copy();\n\
"
};

// 1 - порядковый номер проверки
// 2 - код для вычисления успешности выражения группы
// 3 - успешность выражения группы
const std::string zeroOrMoreTemplate {
    "\
// Template: ZERO_OR_MORE \n\
bool b#{1};\n\
int index#{1} = currentIndex;\n\
ParserTree* copy#{1} = tree->copy();\n\
\n\
do {\n\
    #{2}\n\
    \n\
    b#{1} = #{3};\n\
    if (b#{1}) {\n\
        copy#{1} = tree->copy();\n\
        index#{1} = currentIndex;\n\
    }\n\
} while (b#{1});\n\
currentIndex = index#{1};\n\
delete tree;\n\
tree = copy#{1}->copy();\n\
"
};

// Генерировать блок кода на каждый and, or и так далее
// В начале блока запоминать текущий currentIndex
// Если and - генерировать условие для первого выражения,
// потом в теле этого условия - условие для второго выражения
// Аналогично для других конструкций
//
// Если выражение - токен, то генерировать addToken(tree, Token::тип)
// Если выражение - лексема, то генерировать addTree(tree, лексема())
// Если выражение - группа, то условие = успешность выражения группы


std::string readFile(std::string filename) {
    std::ifstream f(filename);
    std::stringstream buf;
    buf << f.rdbuf();
    std::string str = buf.str();
    f.close();
    return str;
}

ParserGenerator::ParserGenerator()
{
    lexerHeaderTemplate = readFile("templates/lexer_header.txt");
    lexerSourceTemplate = readFile("templates/lexer_source.txt");
    parserHeaderTemplate = readFile("templates/parser_header.txt");
    parserSourceTemplate = readFile("templates/parser_source.txt");

    currentExpressionNumber = 0;
}

int ParserGenerator::getCurrentExpressionNumber()
{
    currentExpressionNumber++;
    return currentExpressionNumber - 1;
}

void ParserGenerator::generateCode(ParserTree *tree)
{
    for (Unit &u : tree->units) {
        assert(u.isTree());
        assert(u.getTree()->type == ParserTree::rule);
        ParserTree* rule = u.getTree();
        assert(rule->units.size() == 4);
        assert(rule->units[0].isToken());
        assert(rule->units[2].isTree());
        ParserTree* e = rule->units[2].getTree();
        std::string tokenName = rule->units[0].getToken().text;
        // std::cout << "rule: " << tokenName << "\n";
        if (e->units.size() == 1 &&
            e->units[0].isToken() &&
            e->units[0].getToken().type == Token::STRING) {
            // Если правило задает токен
            std::string tokenRe = e->units[0].getToken().text;
            // std::cout << "is token\n";
            if (tokens.count(tokenName) ||
                lexems.count(tokenName)) {
                throw std::runtime_error("Multiple definition of token: " + tokenName);
            }
            // std::cout << tokenName << "is token\n";
            tokens.insert({tokenName, tokenRe});
            tokensArray.push_back(tokenName);
        }
        else {
            // Если правило задает синтаксическое правило
            // std::cout << "is lexem\n";
            if (tokens.count(tokenName) ||
                lexems.count(tokenName)) {
                throw std::runtime_error("Multiple definition of token: " + tokenName);
            }
            // std::cout << tokenName << "is lexem\n";
            lexems.insert({tokenName, &u});
            lexemsArray.push_back(tokenName);
        }
    }

    generateLexer();
    generateParser();
}

void ParserGenerator::generateLexer()
{
    lexerHeader = lexerHeaderTemplate;
    lexerSource = lexerSourceTemplate;

    std::string types = "";
    std::string definitions = "";
    for (int i = 0; i < (int)tokensArray.size(); i++) {
        std::string token = tokensArray[i];
        types += token + "=" + std::to_string(i) + ",";
        definitions += "{" + tokens[token] + ",Token::" + token  + " },";
    }
    lexerHeader = StringTemplate::getString(lexerHeader, 1, types);
    lexerSource = StringTemplate::getString(lexerSource, 1, definitions);
}

void ParserGenerator::generateParser()
{
    parserHeader = parserHeaderTemplate;
    parserSource = parserSourceTemplate;

    std::string types = "";
    std::string methodsDefinitions = "";
    std::string methods = "";
    
    if (!lexems.count("lang")) {
        throw std::runtime_error("Error: no lexem 'lang'");
    }

    for (int i = 0; i < (int)lexemsArray.size(); i++) {
        currentExpressionNumber = 0;
        std::string lexem = lexemsArray[i];
        std::cout << "lexem is " << lexem << "\n";
        types += lexem + "=" + std::to_string(i) + ",";
        methodsDefinitions += "ParserTree* " + lexem + "();\n";

        methods += getMethodCode(lexems[lexem]) + "\n";
    }

    parserHeader = StringTemplate::getString(parserHeader, 1, types);
    parserHeader = StringTemplate::getString(parserHeader, 2, methodsDefinitions);
    parserSource = StringTemplate::getString(parserSource, 1, methods);
}

std::string ParserGenerator::getMethodCode(Unit *rule)
{
    // Код - methodTemplate
    //   1 - rule->units[0].getToken().text
    //   2 - getExpressionCode(rule->units[2]).first
    // assert(rule->type == ParserTree::rule);
    assert(rule->isTree());
    assert(rule->getTree()->units.size() == 4);
    assert(rule->getTree()->units[0].isToken());
    assert(rule->getTree()->units[0].getToken().type == Token::VAR);

    std::string code = methodTemplate;
    std::string lexem = rule->getTree()->units[0].getToken().text;
    if (!lexems.count(lexem)) {
        throw std::runtime_error("Error: Using undefined lexem '" + lexem + "'");
    }
    std::string method = getExpressionCode(rule->getTree()->units[2]).first;

    code = StringTemplate::getString(code, 1, lexem);
    code = StringTemplate::getString(code, 2, method);

    return code;
}

std::pair<std::string, std::string>
ParserGenerator::getExpressionCode(ParserTree* expression)
{
    if (expression->type == ParserTree::group) {
        return getGroupCode(expression);
    }
    else if (expression->type == ParserTree::zero_or_one) {
        return getZeroOrOneCode(expression);
    }
    else if (expression->type == ParserTree::zero_or_more) {
        return getZeroOrMoreCode(expression);
    }
    assert(expression->type == ParserTree::expression);
    assert(expression->units.size());
    if (expression->units.size() == 1) {
        Unit &u = expression->units[0];
        if (u.isToken()) {
            Token t = u.getToken();
            if (tokens.count(t.text)) {
                return getTokenCode(expression);
            }
            else if (lexems.count(t.text)) {
                return getTreeCode(expression);
            }
            else {
                throw std::runtime_error("Using undefined token : '" + t.text + "'");
            }
        }
        else {
            if (u.getTree()->type == ParserTree::group) {
                return getGroupCode(u.getTree());
            }
            if (u.getTree()->type == ParserTree::zero_or_one) {
                return getZeroOrOneCode(u.getTree());
            }
            if (u.getTree()->type == ParserTree::zero_or_more) {
                return getZeroOrMoreCode(u.getTree());
            }
            assert(false);
        }
    }
    else {
        assert(expression->units.size() == 3);
        Unit &u = expression->units[1];
        assert(u.isToken());
        Token t = u.getToken();
        if (t.type == Token::OR) {
            return getOrCode(expression);
        }
        else {
            assert(t.type == Token::COMMA);
            return getAndCode(expression);
        }
    }
}

std::pair<std::string, std::string>
ParserGenerator::getExpressionCode(Unit &unit)
{
    if (unit.isToken()) {
        Token t = unit.getToken();
        if (tokens.count(t.text)) {
            return getTokenCode(unit);
        }
        else if (lexems.count(t.text)) {
            return getTreeCode(unit);
        }
        else {
            throw std::runtime_error("Using undefined token : '" + t.text + "'");
        }
    }
    else {
        return getExpressionCode(unit.getTree());
        // zero_or_one && zero_or_more
        // throw std::runtime_error("not implemented");
    }
}

std::pair<std::string, std::string>
ParserGenerator::getTokenCode(ParserTree *e)
{
    // код пустой; проверка - addToken(tree, Token:: тип)
    assert(e->type == ParserTree::expression);
    assert(e->units.size() == 1);
    assert(e->units[0].isToken());

    std::string t = e->units[0].getToken().text;

    assert(tokens.count(t));

    return {"", "addToken(tree, Token::" + t + ")"};
}
std::pair<std::string, std::string>
ParserGenerator::getTokenCode(Unit &u)
{
    // код пустой; проверка - addToken(tree, Token:: тип)
    assert(u.isToken());

    std::string t = u.getToken().text;

    assert(tokens.count(t));

    return {"", "addToken(tree, Token::" + t + ")"};
}

std::pair<std::string, std::string>
ParserGenerator::getTreeCode(ParserTree *e)
{
    // код пустой; проверка - addTree(tree, тип())
    assert(e->type == ParserTree::expression);
    assert(e->units.size() == 1);
    assert(e->units[0].isToken());

    std::string t = e->units[0].getToken().text;

    assert(lexems.count(t));

    return {"", "addTree(tree, " + t + "())"};
}

std::pair<std::string, std::string>
ParserGenerator::getTreeCode(Unit &u)
{
    // код пустой; проверка - addTree(tree, тип())
    assert(u.isToken());

    std::string t = u.getToken().text;

    assert(lexems.count(t));

    return {"", "addTree(tree, " + t + "())"};
}

std::pair<std::string, std::string>
ParserGenerator::getAndCode(ParserTree *e)
{
    // Код - andTemplate:
    //   1 - getCurrentExpressionNumber()
    //   2 - getExpressionCode(первое выражение).first
    //   3 - getExpressionCode(первое выражение).second
    //   4 - getExpressionCode(второе выражение).first
    //   5 - getExpressionCode(второе выражение).second
    // Успешность выражения - b*номер операции из 1*
    assert(e->units.size() == 3);
    assert(e->units[1].isToken());
    assert(e->units[1].getToken().type == Token::COMMA);
    int index = getCurrentExpressionNumber();
    std::string code = andTemplate;
    std::string res = "b" + std::to_string(index);

    code = StringTemplate::getString(code, 1, std::to_string(index));

    auto first_expr = getExpressionCode(e->units[0]);
    auto second_expr = getExpressionCode(e->units[2]);

    code = StringTemplate::getString(code, 2, first_expr.first);
    code = StringTemplate::getString(code, 3, first_expr.second);
    code = StringTemplate::getString(code, 4, second_expr.first);
    code = StringTemplate::getString(code, 5, second_expr.second);

    return {code, res};
}

std::pair<std::string, std::string>
ParserGenerator::getOrCode(ParserTree *e)
{
    // Код - orTemplate:
    //   1 - getCurrentExpressionNumber()
    //   2 - getExpressionCode(первое выражение).first
    //   3 - getExpressionCode(первое выражение).second
    //   4 - getExpressionCode(второе выражение).first
    //   5 - getExpressionCode(второе выражение).second
    // Успешность выражения - b*номер операции из 1*

    assert(e->units.size() == 3);
    assert(e->units[1].isToken());
    assert(e->units[1].getToken().type == Token::OR);
    int index = getCurrentExpressionNumber();
    std::string code = orTemplate;
    std::string res = "b" + std::to_string(index);

    code = StringTemplate::getString(code, 1, std::to_string(index));

    auto first_expr = getExpressionCode(e->units[0]);
    auto second_expr = getExpressionCode(e->units[2]);

    code = StringTemplate::getString(code, 2, first_expr.first);
    code = StringTemplate::getString(code, 3, first_expr.second);
    code = StringTemplate::getString(code, 4, second_expr.first);
    code = StringTemplate::getString(code, 5, second_expr.second);

    return {code, res};
}

std::pair<std::string, std::string>
ParserGenerator::getGroupCode(ParserTree* e)
{
    // Код - groupTemplate
    //   1 - getCurrentExpressionNumber()
    //   2 - getExpressionCode(выражение_группы).first
    //   3 - getExpressionCode(выражение_группы).second
    // Проверка - b*номер из 1*
    assert(e->type == ParserTree::group);
    assert(e->units.size() == 3);
    assert(e->units[0].isToken());
    assert(e->units[0].getToken().type == Token::BR_OP);
    assert(e->units[2].getToken().type == Token::BR_CL);

    int index = getCurrentExpressionNumber();
    std::string code = groupTemplate;
    std::string res = "b" + std::to_string(index);

    auto exp = getExpressionCode(e->units[1]);

    code = StringTemplate::getString(code, 1, std::to_string(index));
    code = StringTemplate::getString(code, 2, exp.first);
    code = StringTemplate::getString(code, 3, exp.second);

    return {code, res};
}

std::pair<std::string, std::string>
ParserGenerator::getZeroOrOneCode(ParserTree* e)
{
    // Код - zeroOrOneTemplate
    //   1 - getCurrentExpressionNumber()
    //   2 - getExpressionCode(выражение_группы).first
    //   3 - getExpressionCode(выражение_группы).second
    // Проверка - true
    assert(e->type == ParserTree::zero_or_one);
    assert(e->units.size() == 3);
    assert(e->units[0].isToken());
    assert(e->units[0].getToken().type == Token::SQ_OP);
    assert(e->units[2].getToken().type == Token::SQ_CL);

    int index = getCurrentExpressionNumber();
    std::string code = zeroOrOneTemplate;
    std::string res = "true";

    auto exp = getExpressionCode(e->units[1]);

    code = StringTemplate::getString(code, 1, std::to_string(index));
    code = StringTemplate::getString(code, 2, exp.first);
    code = StringTemplate::getString(code, 3, exp.second);

    return {code, res};
}

std::pair<std::string, std::string>
ParserGenerator::getZeroOrMoreCode(ParserTree* e)
{
    // Код - zeroOrMoreTemplate
    //   1 - getCurrentExpressionNumber()
    //   2 - getExpressionCode(выражение_группы).first
    //   3 - getExpressionCode(выражение_группы).second
    // Проверка - true
    assert(e->type == ParserTree::zero_or_more);
    assert(e->units.size() == 3);
    assert(e->units[0].isToken());
    assert(e->units[0].getToken().type == Token::CR_OP);
    assert(e->units[2].getToken().type == Token::CR_CL);

    int index = getCurrentExpressionNumber();
    std::string code = zeroOrMoreTemplate;
    std::string res = "true";

    auto exp = getExpressionCode(e->units[1]);

    code = StringTemplate::getString(code, 1, std::to_string(index));
    code = StringTemplate::getString(code, 2, exp.first);
    code = StringTemplate::getString(code, 3, exp.second);

    return {code, res};
}
