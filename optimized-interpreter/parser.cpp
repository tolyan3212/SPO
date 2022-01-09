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

ParserTree* Parser::value() {
    ParserTree* tree = new ParserTree(ParserTree::value, {});

// Template: OR 
bool b0;
int index0 = currentIndex;
ParserTree* copy0 = tree->copy();



b0 = addToken(tree, Token::LITERAL);
if (b0) {}
else {
    
    b0 = b0 || addToken(tree, Token::VAR);
    if (b0) {}
    else {
        currentIndex = index0;
        delete tree;
        tree = copy0->copy();
    }
}
    if (tree->units.size()) {
        return tree;
    }
    return nullptr;
}

ParserTree* Parser::op() {
    ParserTree* tree = new ParserTree(ParserTree::op, {});

// Template: OR 
bool b0;
int index0 = currentIndex;
ParserTree* copy0 = tree->copy();



b0 = addToken(tree, Token::MATH_OP);
if (b0) {}
else {
    // Template: OR 
bool b1;
int index1 = currentIndex;
ParserTree* copy1 = tree->copy();



b1 = addToken(tree, Token::LOGIC_OP);
if (b1) {}
else {
    
    b1 = b1 || addToken(tree, Token::COMPARE_OP);
    if (b1) {}
    else {
        currentIndex = index1;
        delete tree;
        tree = copy1->copy();
    }
}
    b0 = b0 || b1;
    if (b0) {}
    else {
        currentIndex = index0;
        delete tree;
        tree = copy0->copy();
    }
}
    if (tree->units.size()) {
        return tree;
    }
    return nullptr;
}

ParserTree* Parser::value_expr() {
    ParserTree* tree = new ParserTree(ParserTree::value_expr, {});

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



b2 = addTree(tree, assign_expr());
if (b2) {}
else {
    // Template: OR 
bool b3;
int index3 = currentIndex;
ParserTree* copy3 = tree->copy();



b3 = addTree(tree, function_call());
if (b3) {}
else {
    // Template: OR 
bool b4;
int index4 = currentIndex;
ParserTree* copy4 = tree->copy();



b4 = addTree(tree, value());
if (b4) {}
else {
    // Template: AND 
bool b5;
int index5 = currentIndex;
ParserTree* copy5 = tree->copy();



b5 = addToken(tree, Token::BRACKET_OPEN);
if (b5) {
    // Template: AND 
bool b6;
int index6 = currentIndex;
ParserTree* copy6 = tree->copy();



b6 = addTree(tree, value_expr());
if (b6) {
    
    b6 = b6 && addToken(tree, Token::BRACKET_CLOSE);
    if (b6) {}
    else {
        currentIndex = index6;
        delete tree;
        tree = copy6->copy();
    }
}
else {
    currentIndex = index6;
    delete tree;
    tree = copy6->copy();
}
    b5 = b5 && b6;
    if (b5) {}
    else {
        currentIndex = index5;
        delete tree;
        tree = copy5->copy();
    }
}
else {
    currentIndex = index5;
    delete tree;
    tree = copy5->copy();
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



b9 = addTree(tree, op());
if (b9) {
    
    b9 = b9 && addTree(tree, value_expr());
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
if (b7) {
    copy7 = tree->copy();
    index7 = currentIndex;
}
currentIndex = index7;
delete tree;
tree = copy7->copy();

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

ParserTree* Parser::assign_expr() {
    ParserTree* tree = new ParserTree(ParserTree::assign_expr, {});

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



b1 = addToken(tree, Token::ASSIGN_OP);
if (b1) {
    
    b1 = b1 && addTree(tree, value_expr());
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

ParserTree* Parser::final_value_expr() {
    ParserTree* tree = new ParserTree(ParserTree::final_value_expr, {});

// Template: AND 
bool b0;
int index0 = currentIndex;
ParserTree* copy0 = tree->copy();



b0 = addTree(tree, value_expr());
if (b0) {
    
    b0 = b0 && addToken(tree, Token::END_OF_LINE);
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

ParserTree* Parser::condition() {
    ParserTree* tree = new ParserTree(ParserTree::condition, {});

// Template: AND 
bool b0;
int index0 = currentIndex;
ParserTree* copy0 = tree->copy();



b0 = addToken(tree, Token::BRACKET_OPEN);
if (b0) {
    // Template: AND 
bool b1;
int index1 = currentIndex;
ParserTree* copy1 = tree->copy();



b1 = addTree(tree, value_expr());
if (b1) {
    
    b1 = b1 && addToken(tree, Token::BRACKET_CLOSE);
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

ParserTree* Parser::block() {
    ParserTree* tree = new ParserTree(ParserTree::block, {});

// Template: OR 
bool b0;
int index0 = currentIndex;
ParserTree* copy0 = tree->copy();

// Template: GROUP 
bool b1;
int index1 = currentIndex;
ParserTree* copy1 = tree->copy();

// Template: AND 
bool b2;
int index2 = currentIndex;
ParserTree* copy2 = tree->copy();



b2 = addToken(tree, Token::BEGIN);
if (b2) {
    // Template: AND 
bool b3;
int index3 = currentIndex;
ParserTree* copy3 = tree->copy();

// Template: ZERO_OR_MORE 
bool b4;
int index4 = currentIndex;
ParserTree* copy4 = tree->copy();

do {
    
    
    b4 = addTree(tree, expression());
    if (b4) {
        copy4 = tree->copy();
        index4 = currentIndex;
    }
} while (b4);
currentIndex = index4;
delete tree;
tree = copy4->copy();


b3 = true;
if (b3) {
    
    b3 = b3 && addToken(tree, Token::END);
    if (b3) {}
    else {
        currentIndex = index3;
        delete tree;
        tree = copy3->copy();
    }
}
else {
    currentIndex = index3;
    delete tree;
    tree = copy3->copy();
}
    b2 = b2 && b3;
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

b1 = b2;
if (b1) {}
else {
    currentIndex = index1;
    delete tree;
    tree = copy1->copy();
}

b0 = b1;
if (b0) {}
else {
    
    b0 = b0 || addTree(tree, expression());
    if (b0) {}
    else {
        currentIndex = index0;
        delete tree;
        tree = copy0->copy();
    }
}
    if (tree->units.size()) {
        return tree;
    }
    return nullptr;
}

ParserTree* Parser::if_else() {
    ParserTree* tree = new ParserTree(ParserTree::if_else, {});

// Template: OR 
bool b0;
int index0 = currentIndex;
ParserTree* copy0 = tree->copy();

// Template: GROUP 
bool b1;
int index1 = currentIndex;
ParserTree* copy1 = tree->copy();

// Template: AND 
bool b2;
int index2 = currentIndex;
ParserTree* copy2 = tree->copy();



b2 = addToken(tree, Token::IF);
if (b2) {
    // Template: AND 
bool b3;
int index3 = currentIndex;
ParserTree* copy3 = tree->copy();



b3 = addTree(tree, condition());
if (b3) {
    // Template: AND 
bool b4;
int index4 = currentIndex;
ParserTree* copy4 = tree->copy();



b4 = addTree(tree, block());
if (b4) {
    // Template: AND 
bool b5;
int index5 = currentIndex;
ParserTree* copy5 = tree->copy();



b5 = addToken(tree, Token::ELSE);
if (b5) {
    
    b5 = b5 && addTree(tree, block());
    if (b5) {}
    else {
        currentIndex = index5;
        delete tree;
        tree = copy5->copy();
    }
}
else {
    currentIndex = index5;
    delete tree;
    tree = copy5->copy();
}
    b4 = b4 && b5;
    if (b4) {}
    else {
        currentIndex = index4;
        delete tree;
        tree = copy4->copy();
    }
}
else {
    currentIndex = index4;
    delete tree;
    tree = copy4->copy();
}
    b3 = b3 && b4;
    if (b3) {}
    else {
        currentIndex = index3;
        delete tree;
        tree = copy3->copy();
    }
}
else {
    currentIndex = index3;
    delete tree;
    tree = copy3->copy();
}
    b2 = b2 && b3;
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

b1 = b2;
if (b1) {}
else {
    currentIndex = index1;
    delete tree;
    tree = copy1->copy();
}

b0 = b1;
if (b0) {}
else {
    // Template: GROUP 
bool b6;
int index6 = currentIndex;
ParserTree* copy6 = tree->copy();

// Template: AND 
bool b7;
int index7 = currentIndex;
ParserTree* copy7 = tree->copy();



b7 = addToken(tree, Token::IF);
if (b7) {
    // Template: AND 
bool b8;
int index8 = currentIndex;
ParserTree* copy8 = tree->copy();



b8 = addTree(tree, condition());
if (b8) {
    
    b8 = b8 && addTree(tree, block());
    if (b8) {}
    else {
        currentIndex = index8;
        delete tree;
        tree = copy8->copy();
    }
}
else {
    currentIndex = index8;
    delete tree;
    tree = copy8->copy();
}
    b7 = b7 && b8;
    if (b7) {}
    else {
        currentIndex = index7;
        delete tree;
        tree = copy7->copy();
    }
}
else {
    currentIndex = index7;
    delete tree;
    tree = copy7->copy();
}

b6 = b7;
if (b6) {}
else {
    currentIndex = index6;
    delete tree;
    tree = copy6->copy();
}
    b0 = b0 || b6;
    if (b0) {}
    else {
        currentIndex = index0;
        delete tree;
        tree = copy0->copy();
    }
}
    if (tree->units.size()) {
        return tree;
    }
    return nullptr;
}

ParserTree* Parser::while_block() {
    ParserTree* tree = new ParserTree(ParserTree::while_block, {});

// Template: AND 
bool b0;
int index0 = currentIndex;
ParserTree* copy0 = tree->copy();



b0 = addToken(tree, Token::WHILE);
if (b0) {
    // Template: AND 
bool b1;
int index1 = currentIndex;
ParserTree* copy1 = tree->copy();



b1 = addTree(tree, condition());
if (b1) {
    
    b1 = b1 && addTree(tree, block());
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

ParserTree* Parser::do_while() {
    ParserTree* tree = new ParserTree(ParserTree::do_while, {});

// Template: AND 
bool b0;
int index0 = currentIndex;
ParserTree* copy0 = tree->copy();



b0 = addToken(tree, Token::DO);
if (b0) {
    // Template: AND 
bool b1;
int index1 = currentIndex;
ParserTree* copy1 = tree->copy();



b1 = addTree(tree, block());
if (b1) {
    // Template: AND 
bool b2;
int index2 = currentIndex;
ParserTree* copy2 = tree->copy();



b2 = addToken(tree, Token::WHILE);
if (b2) {
    // Template: AND 
bool b3;
int index3 = currentIndex;
ParserTree* copy3 = tree->copy();



b3 = addTree(tree, condition());
if (b3) {
    
    b3 = b3 && addToken(tree, Token::END_OF_LINE);
    if (b3) {}
    else {
        currentIndex = index3;
        delete tree;
        tree = copy3->copy();
    }
}
else {
    currentIndex = index3;
    delete tree;
    tree = copy3->copy();
}
    b2 = b2 && b3;
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

ParserTree* Parser::function_call() {
    ParserTree* tree = new ParserTree(ParserTree::function_call, {});

// Template: AND 
bool b0;
int index0 = currentIndex;
ParserTree* copy0 = tree->copy();



b0 = addToken(tree, Token::VAR);
if (b0) {
    // Template: GROUP 
bool b1;
int index1 = currentIndex;
ParserTree* copy1 = tree->copy();

// Template: OR 
bool b2;
int index2 = currentIndex;
ParserTree* copy2 = tree->copy();

// Template: GROUP 
bool b3;
int index3 = currentIndex;
ParserTree* copy3 = tree->copy();

// Template: AND 
bool b4;
int index4 = currentIndex;
ParserTree* copy4 = tree->copy();



b4 = addToken(tree, Token::BRACKET_OPEN);
if (b4) {
    // Template: AND 
bool b5;
int index5 = currentIndex;
ParserTree* copy5 = tree->copy();



b5 = addTree(tree, argument_list());
if (b5) {
    
    b5 = b5 && addToken(tree, Token::BRACKET_CLOSE);
    if (b5) {}
    else {
        currentIndex = index5;
        delete tree;
        tree = copy5->copy();
    }
}
else {
    currentIndex = index5;
    delete tree;
    tree = copy5->copy();
}
    b4 = b4 && b5;
    if (b4) {}
    else {
        currentIndex = index4;
        delete tree;
        tree = copy4->copy();
    }
}
else {
    currentIndex = index4;
    delete tree;
    tree = copy4->copy();
}

b3 = b4;
if (b3) {}
else {
    currentIndex = index3;
    delete tree;
    tree = copy3->copy();
}

b2 = b3;
if (b2) {}
else {
    // Template: GROUP 
bool b6;
int index6 = currentIndex;
ParserTree* copy6 = tree->copy();

// Template: AND 
bool b7;
int index7 = currentIndex;
ParserTree* copy7 = tree->copy();



b7 = addToken(tree, Token::BRACKET_OPEN);
if (b7) {
    
    b7 = b7 && addToken(tree, Token::BRACKET_CLOSE);
    if (b7) {}
    else {
        currentIndex = index7;
        delete tree;
        tree = copy7->copy();
    }
}
else {
    currentIndex = index7;
    delete tree;
    tree = copy7->copy();
}

b6 = b7;
if (b6) {}
else {
    currentIndex = index6;
    delete tree;
    tree = copy6->copy();
}
    b2 = b2 || b6;
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

ParserTree* Parser::argument_list() {
    ParserTree* tree = new ParserTree(ParserTree::argument_list, {});

// Template: AND 
bool b0;
int index0 = currentIndex;
ParserTree* copy0 = tree->copy();



b0 = addTree(tree, value_expr());
if (b0) {
    // Template: ZERO_OR_MORE 
bool b1;
int index1 = currentIndex;
ParserTree* copy1 = tree->copy();

do {
    // Template: AND 
bool b2;
int index2 = currentIndex;
ParserTree* copy2 = tree->copy();



b2 = addToken(tree, Token::COMMA);
if (b2) {
    
    b2 = b2 && addTree(tree, value_expr());
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
    
    b1 = b2;
    if (b1) {
        copy1 = tree->copy();
        index1 = currentIndex;
    }
} while (b1);
currentIndex = index1;
delete tree;
tree = copy1->copy();

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

ParserTree* Parser::return_expr() {
    ParserTree* tree = new ParserTree(ParserTree::return_expr, {});

// Template: AND 
bool b0;
int index0 = currentIndex;
ParserTree* copy0 = tree->copy();



b0 = addToken(tree, Token::RETURN_OP);
if (b0) {
    
    b0 = b0 && addTree(tree, final_value_expr());
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

ParserTree* Parser::function_def_args() {
    ParserTree* tree = new ParserTree(ParserTree::function_def_args, {});

// Template: ZERO_OR_ONE 
bool b0;
int index0 = currentIndex;
ParserTree* copy0 = tree->copy();

// Template: AND 
bool b1;
int index1 = currentIndex;
ParserTree* copy1 = tree->copy();



b1 = addToken(tree, Token::VAR);
if (b1) {
    // Template: ZERO_OR_MORE 
bool b2;
int index2 = currentIndex;
ParserTree* copy2 = tree->copy();

do {
    // Template: AND 
bool b3;
int index3 = currentIndex;
ParserTree* copy3 = tree->copy();



b3 = addToken(tree, Token::COMMA);
if (b3) {
    
    b3 = b3 && addToken(tree, Token::VAR);
    if (b3) {}
    else {
        currentIndex = index3;
        delete tree;
        tree = copy3->copy();
    }
}
else {
    currentIndex = index3;
    delete tree;
    tree = copy3->copy();
}
    
    b2 = b3;
    if (b2) {
        copy2 = tree->copy();
        index2 = currentIndex;
    }
} while (b2);
currentIndex = index2;
delete tree;
tree = copy2->copy();

    b1 = b1 && true;
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

b0 = b1;
if (b0) {
    copy0 = tree->copy();
    index0 = currentIndex;
}
currentIndex = index0;
delete tree;
tree = copy0->copy();

    if (tree->units.size()) {
        return tree;
    }
    return nullptr;
}

ParserTree* Parser::function_definition() {
    ParserTree* tree = new ParserTree(ParserTree::function_definition, {});

// Template: AND 
bool b0;
int index0 = currentIndex;
ParserTree* copy0 = tree->copy();



b0 = addToken(tree, Token::DEF);
if (b0) {
    // Template: AND 
bool b1;
int index1 = currentIndex;
ParserTree* copy1 = tree->copy();



b1 = addToken(tree, Token::VAR);
if (b1) {
    // Template: AND 
bool b2;
int index2 = currentIndex;
ParserTree* copy2 = tree->copy();



b2 = addToken(tree, Token::BRACKET_OPEN);
if (b2) {
    // Template: AND 
bool b3;
int index3 = currentIndex;
ParserTree* copy3 = tree->copy();



b3 = addTree(tree, function_def_args());
if (b3) {
    // Template: AND 
bool b4;
int index4 = currentIndex;
ParserTree* copy4 = tree->copy();



b4 = addToken(tree, Token::BRACKET_CLOSE);
if (b4) {
    // Template: AND 
bool b5;
int index5 = currentIndex;
ParserTree* copy5 = tree->copy();



b5 = addToken(tree, Token::BEGIN);
if (b5) {
    // Template: AND 
bool b6;
int index6 = currentIndex;
ParserTree* copy6 = tree->copy();

// Template: ZERO_OR_MORE 
bool b7;
int index7 = currentIndex;
ParserTree* copy7 = tree->copy();

do {
    
    
    b7 = addTree(tree, expression());
    if (b7) {
        copy7 = tree->copy();
        index7 = currentIndex;
    }
} while (b7);
currentIndex = index7;
delete tree;
tree = copy7->copy();


b6 = true;
if (b6) {
    
    b6 = b6 && addToken(tree, Token::END);
    if (b6) {}
    else {
        currentIndex = index6;
        delete tree;
        tree = copy6->copy();
    }
}
else {
    currentIndex = index6;
    delete tree;
    tree = copy6->copy();
}
    b5 = b5 && b6;
    if (b5) {}
    else {
        currentIndex = index5;
        delete tree;
        tree = copy5->copy();
    }
}
else {
    currentIndex = index5;
    delete tree;
    tree = copy5->copy();
}
    b4 = b4 && b5;
    if (b4) {}
    else {
        currentIndex = index4;
        delete tree;
        tree = copy4->copy();
    }
}
else {
    currentIndex = index4;
    delete tree;
    tree = copy4->copy();
}
    b3 = b3 && b4;
    if (b3) {}
    else {
        currentIndex = index3;
        delete tree;
        tree = copy3->copy();
    }
}
else {
    currentIndex = index3;
    delete tree;
    tree = copy3->copy();
}
    b2 = b2 && b3;
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

ParserTree* Parser::expression() {
    ParserTree* tree = new ParserTree(ParserTree::expression, {});

// Template: OR 
bool b0;
int index0 = currentIndex;
ParserTree* copy0 = tree->copy();



b0 = addTree(tree, final_value_expr());
if (b0) {}
else {
    // Template: OR 
bool b1;
int index1 = currentIndex;
ParserTree* copy1 = tree->copy();



b1 = addTree(tree, while_block());
if (b1) {}
else {
    // Template: OR 
bool b2;
int index2 = currentIndex;
ParserTree* copy2 = tree->copy();



b2 = addTree(tree, do_while());
if (b2) {}
else {
    // Template: OR 
bool b3;
int index3 = currentIndex;
ParserTree* copy3 = tree->copy();



b3 = addTree(tree, if_else());
if (b3) {}
else {
    
    b3 = b3 || addTree(tree, return_expr());
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
    b1 = b1 || b2;
    if (b1) {}
    else {
        currentIndex = index1;
        delete tree;
        tree = copy1->copy();
    }
}
    b0 = b0 || b1;
    if (b0) {}
    else {
        currentIndex = index0;
        delete tree;
        tree = copy0->copy();
    }
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
    // Template: OR 
bool b1;
int index1 = currentIndex;
ParserTree* copy1 = tree->copy();



b1 = addTree(tree, function_definition());
if (b1) {}
else {
    
    b1 = b1 || addTree(tree, expression());
    if (b1) {}
    else {
        currentIndex = index1;
        delete tree;
        tree = copy1->copy();
    }
}
    
    b0 = b1;
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


