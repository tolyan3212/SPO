#include "parser.h"

#include <stdexcept>
#include <iostream>


Parser::Parser()
{}


int Parser::getErrorPlace(std::vector<Token> tokens)
{
    vertices = {};
    this->tokens = tokens;
    
    currentTokenIndex = 0;
    // try {
        lang();
    // }
    // catch (std::exception& e) {
        // std::cout << "Exception while parsing: " << e.what() << "\n";
        // return currentTokenIndex;
    // }
    return -1;
}

Parser::Vertex Parser::getVertex(std::vector<Token> tokens)
{
    getErrorPlace(tokens);
    return vertices[0];
}


bool Parser::current_token_is(Token::Type type) {
    return tokens[currentTokenIndex].type == type;
}

bool Parser::accept(Token::Type type)
{
    if (tokens[currentTokenIndex].type == type) {
        currentTokenIndex++;
        return true;
    }
    return false;
}
void Parser::expect(Token::Type type) {
    if (!accept(type)) {
        throw std::runtime_error("Error while excepting type: "
                                 + std::to_string(type));
    }
}

void Parser::lang()
{
    begining_of_structure(LANG);
    while (currentTokenIndex < tokens.size()) {
        if (current_token_is(Token::DEF)) {
            function_definition();
        }
        else {
            expression();
        }
    }
}

void Parser::expression()
{
    begining_of_structure(EXPRESSION);
    // vertices.push_back({EXPRESSION, {}});
    if (current_token_is(Token::WHILE)) {
        while_block();
    }
    else if (current_token_is(Token::DO)) {
        do_while();
    }
    else if (current_token_is(Token::IF)) {
        if_else();
    }
    else if (current_token_is(Token::RETURN_OP)) {
        return_expr();
    }
    else {
        final_value_expr();
    }
    end_of_structure();
}

void Parser::final_value_expr()
{
    begining_of_structure(FINAL_VALUE_EXPR);
    value_expr();
    expect(Token::END_OF_LINE);
    end_of_structure();
}

void Parser::value_expr()
{
    begining_of_structure(VALUE_EXPR);
    if (accept(Token::BRACKET_OPEN)) {
        value_expr();
        expect(Token::BRACKET_CLOSE);
    }
    else {
        // если это value, assign_expr или function_call
        int index = currentTokenIndex;
        try {
            function_call();
        }
        catch (...) {
            vertices.pop_back();
            try {
                currentTokenIndex = index;
                assign_expr();
            }
            catch (...) {
                vertices.pop_back();
                currentTokenIndex = index;
                value();
            }
        }
    }
    while (current_token_is_operator()) {
        operator_symbol();
        value_expr();
    }
    end_of_structure();
}

void Parser::assign_expr()
{
    begining_of_structure(ASSIGN_EXPR);
    expect(Token::VAR);
    expect(Token::ASSIGN_OP);
    value_expr();
    end_of_structure();
}

void Parser::value()
{
    begining_of_structure(VALUE);
    if (!accept(Token::LITERAL)) {
        expect(Token::VAR);
    }
    end_of_structure();
}

void Parser::block()
{
    begining_of_structure(BLOCK);
    if (accept(Token::BEGIN)) {
        int index;
        try {
            while (1) {
                index = currentTokenIndex;
                expression();
            }
        }
        catch (...) {
            // удаление вершин до последнего expression включительно:
            while (vertices[vertices.size()-1].type != EXPRESSION) {
                vertices.pop_back();
            }
            vertices.pop_back();
            currentTokenIndex = index;
            expect(Token::END);
        }
    }
    else {
        expression();
    }
    end_of_structure();
}

void Parser::if_else()
{
    begining_of_structure(IF_ELSE);
    expect(Token::IF);
    condition();
    block();
    if (accept(Token::ELSE)) {
        block();
    }
    end_of_structure();
}

void Parser::while_block()
{
    begining_of_structure(WHILE);
    expect(Token::WHILE);
    condition();
    block();
    end_of_structure();
}

void Parser::do_while()
{
    begining_of_structure(DO_WHILE);
    expect(Token::DO);
    block();
    expect(Token::WHILE);
    condition();
    expect(Token::END_OF_LINE);
    end_of_structure();
}

void Parser::condition()
{
    begining_of_structure(CONDITION);
    expect(Token::BRACKET_OPEN);
    value_expr();
    expect(Token::BRACKET_CLOSE);
    end_of_structure();
}

void Parser::function_call()
{
    begining_of_structure(FUNCTION_CALL);
    expect(Token::VAR);
    expect(Token::BRACKET_OPEN);
    if (!accept(Token::BRACKET_CLOSE)) {
        argument_list();
        expect(Token::BRACKET_CLOSE);
    }
    end_of_structure();
}

void Parser::argument_list()
{
    begining_of_structure(ARGUMENT_LIST);
    value_expr();
    if (accept(Token::COMMA)) {
        argument_list();
    }
    end_of_structure();
}

void Parser::operator_symbol() {
    begining_of_structure(OPERATOR);
    if (!accept(Token::MATH_OP)
        && !accept(Token::LOGIC_OP)) {
        expect(Token::COMPARE_OP);
    }
    end_of_structure();
}

bool Parser::current_token_is_operator()
{
    Token::Type type = tokens[currentTokenIndex].type;
    if (type == Token::MATH_OP
        || type == Token::COMPARE_OP
        || type == Token::LOGIC_OP) {
        return true;
    }
    return false;
}

void Parser::function_def_args()
{
    begining_of_structure(FUNCTION_DEF_ARGS);
    if (tokens[currentTokenIndex].type != Token::BRACKET_CLOSE) {
        value();
    }
    while (accept(Token::COMMA)) {
        value();
    }
    // expect(Token::VAR);
    // value_expr();
    // if (accept(Token::COMMA)) {
        // argument_list();
    // }    

    end_of_structure();
}

void Parser::function_definition()
{
    begining_of_structure(FUNCTION_DEFINITION);
    expect(Token::DEF);
    expect(Token::VAR);
    expect(Token::BRACKET_OPEN);
    function_def_args();
    expect(Token::BRACKET_CLOSE);
    expect(Token::BEGIN);
    int index;
    try {
        while(1) {
            index = currentTokenIndex;
            expression();
        }
    }
    catch (...) {
        // удаление вершин до последнего expression включительно:
        while (vertices[vertices.size()-1].type != EXPRESSION) {
            vertices.pop_back();
        }
        vertices.pop_back();
        currentTokenIndex = index;
    }
    expect(Token::END);
    end_of_structure();
}

void Parser::return_expr()
{
    begining_of_structure(RETURN_EXPR);

    expect(Token::RETURN_OP);
    final_value_expr();

    end_of_structure();
}

void Parser::begining_of_structure(Type type)
{
    vertices.push_back({type, std::vector<Vertex>()});
}
void Parser::end_of_structure()
{
    vertices.at(vertices.size()-1);
    Vertex v = vertices.at(vertices.size()-1);
    vertices.pop_back();
    vertices[vertices.size()-1].vertices.push_back(v);
}
