#include "parser.h"

#include <list>
#include <stdexcept>


Unit::Unit(Token token)
    : token(token),
      tree(nullptr),
      _isToken(true)
{}
Unit::Unit(ParserTree *tree)
    : token(Token("NOT A TOKEN", Token::VAR)),
      tree(tree),
      _isToken(false)
{}

bool Unit::isToken()
{
    return _isToken;
}
bool Unit::isTree()
{
    return !_isToken;
}

Token Unit::getToken()
{
    if (!isToken()) {
        throw std::runtime_error("Trying to get token of a tree unit");
    }
    return token;
}

ParserTree* Unit::getTree()
{
    if (isToken()) {
        throw std::runtime_error("Trying to get tree of a token unit");
    }
    return tree.get();
}


ParserTree::ParserTree(ParserTree::Type type,
                       std::vector<Unit> units)
    : type(type),
      units(units)
{}


std::vector<Token> ParserTree::getAllTokens()
{
    std::vector<Token> res;

    for (int i = 0; i < (int) units.size(); i++) {
        if (units[i].isToken()) {
            res.push_back(units[i].getToken());
        }
        else {
            auto subtreeTokens = units[i].getTree()->getAllTokens();
            res.insert(res.end(), subtreeTokens.begin(),
                       subtreeTokens.end());
        }
    }

    return res;
}

ParserTree* ParserTree::copy()
{
    ParserTree* res = new ParserTree(type, {});
    for (Unit &u : units) {
        if (u.isToken()) {
            res->units.push_back(Unit(Token(u.getToken())));
        }
        else {
            res->units.push_back(Unit(u.getTree()->copy()));
        }
    }
    return res;
}

Parser::Parser()
{}

bool Parser::acceptToken(Token::Type type)
{
    if (tokens[currentIndex].type == type) {
        currentIndex++;
        return true;
    }
    return false;
}
bool Parser::addToken(ParserTree* tree, Token::Type type)
{
    if (currentIndex < (int)tokens.size() &&
        tokens[currentIndex].type == type) {
        tree->units.push_back(Unit(tokens[currentIndex]));
        currentIndex++;
        return true;
    }
    return false;
}

bool Parser::addTree(ParserTree* tree, ParserTree* subtree)
{
    if (!subtree) {
        return false;
    }
    tree->units.push_back(Unit(subtree));
    return true;
}

ParserTree* Parser::parse(std::vector<Token> tokens)
{
    this->tokens = tokens;
    currentIndex = 0;

    ParserTree* tree = lang();
    if (currentIndex != (int) tokens.size()) {
        delete tree;
        throw std::runtime_error("Error while parsing at token number: "
                                 + std::to_string(currentIndex));
    }
    return tree;
}

ParserTree* Parser::zero_or_one()
{
    int indexBefore = currentIndex;
    ParserTree* tree = new ParserTree(ParserTree::zero_or_one, {});
    if (!addToken(tree, Token::SQ_OP)) {
        delete tree;
        return nullptr;
    }
    if (!addTree(tree, expression())) {
        currentIndex = indexBefore;
        delete tree;
        return nullptr;
    }
    if (!addToken(tree, Token::SQ_CL)) {
        currentIndex = indexBefore;
        delete tree;
        return nullptr;
    }
    return tree;
}

ParserTree* Parser::zero_or_more()
{
    int indexBefore = currentIndex;
    ParserTree* tree = new ParserTree(ParserTree::zero_or_more, {});
    if (!addToken(tree, Token::CR_OP)) {
        delete tree;
        return nullptr;
    }
    if (!addTree(tree, expression())) {
        currentIndex = indexBefore;
        delete tree;
        return nullptr;
    }
    if (!addToken(tree, Token::CR_CL)) {
        currentIndex = indexBefore;
        delete tree;
        return nullptr;
    }
    return tree;
}

ParserTree* Parser::group()
{
    int indexBefore = currentIndex;
    ParserTree* tree = new ParserTree(ParserTree::group, {});
    if (!addToken(tree, Token::BR_OP)) {
        delete tree;
        return nullptr;
    }
    if (!addTree(tree, expression())) {
        currentIndex = indexBefore;
        delete tree;
        return nullptr;
    }
    if (!addToken(tree, Token::BR_CL)) {
        currentIndex = indexBefore;
        delete tree;
        return nullptr;
    }
    return tree;
}

ParserTree* Parser::expression()
{
    int indexBefore = currentIndex;
    ParserTree* tree = new ParserTree(ParserTree::expression, {});
    // первое ИЛИ
    if (!addToken(tree, Token::VAR) &&
        !addToken(tree, Token::STRING) &&
        !addTree(tree, zero_or_one()) &&
        !addTree(tree, zero_or_more()) &&
        !addTree(tree, group())) {
        currentIndex = indexBefore;
        delete tree;
        return nullptr;
    }
    // ноль или один OR expression или COMMA expression
    int index2 = currentIndex;
    if (!(addToken(tree, Token::OR) &&
          addTree(tree, expression())) &&
        !(addToken(tree, Token::COMMA) &&
          addTree(tree, expression()))) {
        currentIndex = index2;
    }
    return tree;
}

ParserTree* Parser::rule()
{
    int indexBefore = currentIndex;
    ParserTree* tree = new ParserTree(ParserTree::rule, {});

    if (!(addToken(tree, Token::VAR) &&
          addToken(tree, Token::EQUAL) &&
          addTree(tree, expression()) &&
          addToken(tree, Token::DOT))) {
        currentIndex = indexBefore;
        delete tree;
        return nullptr;
    }
    return tree;
}

ParserTree* Parser::lang()
{
    ParserTree* tree = new ParserTree(ParserTree::lang, {});

    int index2 = currentIndex;
    while (addTree(tree, rule())) {
        index2 = currentIndex;
    }
    currentIndex = index2;

    return tree;
}
