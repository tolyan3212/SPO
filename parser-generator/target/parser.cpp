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

ParserTree* Parser::zero_or_one() {
    ParserTree* tree = new ParserTree(ParserTree::zero_or_one, {});

// Template: AND 
bool b0;
int index0 = currentIndex;
ParserTree* copy0 = tree->copy();



b0 = addToken(tree, Token::SQ_OP);
if (b0) {
    // Template: AND 
bool b1;
int index1 = currentIndex;
ParserTree* copy1 = tree->copy();



b1 = addTree(tree, expression());
if (b1) {
    
    b1 = b1 && addToken(tree, Token::SQ_CL);
    if (b1) {}
    else {
        currentIndex = index1;
        delete tree;
        tree = copy1->copy();
    }
}
else {
    currentIndex = index1;
    delete tree;
    tree = copy1->copy();
}
    b0 = b0 && b1;
    if (b0) {}
    else {
        currentIndex = index0;
        delete tree;
        tree = copy0->copy();
    }
}
else {
    currentIndex = index0;
    delete tree;
    tree = copy0->copy();
}
    if (tree->units.size()) {
        return tree;
    }
    return nullptr;
}

ParserTree* Parser::zero_or_more() {
    ParserTree* tree = new ParserTree(ParserTree::zero_or_more, {});

// Template: AND 
bool b0;
int index0 = currentIndex;
ParserTree* copy0 = tree->copy();



b0 = addToken(tree, Token::CR_OP);
if (b0) {
    // Template: AND 
bool b1;
int index1 = currentIndex;
ParserTree* copy1 = tree->copy();



b1 = addTree(tree, expression());
if (b1) {
    
    b1 = b1 && addToken(tree, Token::CR_CL);
    if (b1) {}
    else {
        currentIndex = index1;
        delete tree;
        tree = copy1->copy();
    }
}
else {
    currentIndex = index1;
    delete tree;
    tree = copy1->copy();
}
    b0 = b0 && b1;
    if (b0) {}
    else {
        currentIndex = index0;
        delete tree;
        tree = copy0->copy();
    }
}
else {
    currentIndex = index0;
    delete tree;
    tree = copy0->copy();
}
    if (tree->units.size()) {
        return tree;
    }
    return nullptr;
}

ParserTree* Parser::group() {
    ParserTree* tree = new ParserTree(ParserTree::group, {});

// Template: AND 
bool b0;
int index0 = currentIndex;
ParserTree* copy0 = tree->copy();



b0 = addToken(tree, Token::BR_OP);
if (b0) {
    // Template: AND 
bool b1;
int index1 = currentIndex;
ParserTree* copy1 = tree->copy();



b1 = addTree(tree, expression());
if (b1) {
    
    b1 = b1 && addToken(tree, Token::BR_CL);
    if (b1) {}
    else {
        currentIndex = index1;
        delete tree;
        tree = copy1->copy();
    }
}
else {
    currentIndex = index1;
    delete tree;
    tree = copy1->copy();
}
    b0 = b0 && b1;
    if (b0) {}
    else {
        currentIndex = index0;
        delete tree;
        tree = copy0->copy();
    }
}
else {
    currentIndex = index0;
    delete tree;
    tree = copy0->copy();
}
    if (tree->units.size()) {
        return tree;
    }
    return nullptr;
}

ParserTree* Parser::expression() {
    ParserTree* tree = new ParserTree(ParserTree::expression, {});

// Template: AND 
bool b0;
int index0 = currentIndex;
ParserTree* copy0 = tree->copy();

// Template: GROUP 
bool b1;
int index1 = currentIndex;
ParserTree* copy1 = tree->copy();

// Template: OR 
bool b2;
int index2 = currentIndex;
ParserTree* copy2 = tree->copy();



b2 = addToken(tree, Token::VAR);
if (b2) {}
else {
    // Template: OR 
bool b3;
int index3 = currentIndex;
ParserTree* copy3 = tree->copy();



b3 = addToken(tree, Token::STRING);
if (b3) {}
else {
    // Template: OR 
bool b4;
int index4 = currentIndex;
ParserTree* copy4 = tree->copy();



b4 = addTree(tree, zero_or_one());
if (b4) {}
else {
    // Template: OR 
bool b5;
int index5 = currentIndex;
ParserTree* copy5 = tree->copy();



b5 = addTree(tree, zero_or_more());
if (b5) {}
else {
    
    b5 = b5 || addTree(tree, group());
    if (b5) {}
    else {
        currentIndex = index5;
        delete tree;
        tree = copy5->copy();
    }
}
    b4 = b4 || b5;
    if (b4) {}
    else {
        currentIndex = index4;
        delete tree;
        tree = copy4->copy();
    }
}
    b3 = b3 || b4;
    if (b3) {}
    else {
        currentIndex = index3;
        delete tree;
        tree = copy3->copy();
    }
}
    b2 = b2 || b3;
    if (b2) {}
    else {
        currentIndex = index2;
        delete tree;
        tree = copy2->copy();
    }
}

b1 = b2;
if (b1) {}
else {
    currentIndex = index1;
    delete tree;
    tree = copy1->copy();
}

b0 = b1;
if (b0) {
    // Template: ZERO_OR_ONE 
bool b6;
int index6 = currentIndex;
ParserTree* copy6 = tree->copy();

// Template: OR 
bool b7;
int index7 = currentIndex;
ParserTree* copy7 = tree->copy();

// Template: GROUP 
bool b8;
int index8 = currentIndex;
ParserTree* copy8 = tree->copy();

// Template: AND 
bool b9;
int index9 = currentIndex;
ParserTree* copy9 = tree->copy();



b9 = addToken(tree, Token::OR);
if (b9) {
    
    b9 = b9 && addTree(tree, expression());
    if (b9) {}
    else {
        currentIndex = index9;
        delete tree;
        tree = copy9->copy();
    }
}
else {
    currentIndex = index9;
    delete tree;
    tree = copy9->copy();
}

b8 = b9;
if (b8) {}
else {
    currentIndex = index8;
    delete tree;
    tree = copy8->copy();
}

b7 = b8;
if (b7) {}
else {
    // Template: GROUP 
bool b10;
int index10 = currentIndex;
ParserTree* copy10 = tree->copy();

// Template: AND 
bool b11;
int index11 = currentIndex;
ParserTree* copy11 = tree->copy();



b11 = addToken(tree, Token::COMMA);
if (b11) {
    
    b11 = b11 && addTree(tree, expression());
    if (b11) {}
    else {
        currentIndex = index11;
        delete tree;
        tree = copy11->copy();
    }
}
else {
    currentIndex = index11;
    delete tree;
    tree = copy11->copy();
}

b10 = b11;
if (b10) {}
else {
    currentIndex = index10;
    delete tree;
    tree = copy10->copy();
}
    b7 = b7 || b10;
    if (b7) {}
    else {
        currentIndex = index7;
        delete tree;
        tree = copy7->copy();
    }
}

b6 = b7;
if (b6) {
    copy6 = tree->copy();
    index6 = currentIndex;
}
currentIndex = index6;
delete tree;
tree = copy6->copy();

    b0 = b0 && true;
    if (b0) {}
    else {
        currentIndex = index0;
        delete tree;
        tree = copy0->copy();
    }
}
else {
    currentIndex = index0;
    delete tree;
    tree = copy0->copy();
}
    if (tree->units.size()) {
        return tree;
    }
    return nullptr;
}

ParserTree* Parser::rule() {
    ParserTree* tree = new ParserTree(ParserTree::rule, {});

// Template: AND 
bool b0;
int index0 = currentIndex;
ParserTree* copy0 = tree->copy();



b0 = addToken(tree, Token::VAR);
if (b0) {
    // Template: AND 
bool b1;
int index1 = currentIndex;
ParserTree* copy1 = tree->copy();



b1 = addToken(tree, Token::ASSIGN);
if (b1) {
    // Template: AND 
bool b2;
int index2 = currentIndex;
ParserTree* copy2 = tree->copy();



b2 = addTree(tree, expression());
if (b2) {
    
    b2 = b2 && addToken(tree, Token::DOT);
    if (b2) {}
    else {
        currentIndex = index2;
        delete tree;
        tree = copy2->copy();
    }
}
else {
    currentIndex = index2;
    delete tree;
    tree = copy2->copy();
}
    b1 = b1 && b2;
    if (b1) {}
    else {
        currentIndex = index1;
        delete tree;
        tree = copy1->copy();
    }
}
else {
    currentIndex = index1;
    delete tree;
    tree = copy1->copy();
}
    b0 = b0 && b1;
    if (b0) {}
    else {
        currentIndex = index0;
        delete tree;
        tree = copy0->copy();
    }
}
else {
    currentIndex = index0;
    delete tree;
    tree = copy0->copy();
}
    if (tree->units.size()) {
        return tree;
    }
    return nullptr;
}

ParserTree* Parser::lang() {
    ParserTree* tree = new ParserTree(ParserTree::lang, {});

// Template: ZERO_OR_MORE 
bool b0;
int index0 = currentIndex;
ParserTree* copy0 = tree->copy();

do {
    
    
    b0 = addTree(tree, rule());
    if (b0) {
        copy0 = tree->copy();
        index0 = currentIndex;
    }
} while (b0);
currentIndex = index0;
delete tree;
tree = copy0->copy();

    if (tree->units.size()) {
        return tree;
    }
    return nullptr;
}


