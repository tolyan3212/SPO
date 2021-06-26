#ifndef PARSER_H
#define PARSER_H

#include "lexer.h"


class Parser
{
public:
    enum Type {
        LANG, EXPRESSION, FINAL_VALUE_EXPR,
        VALUE_EXPR, ASSIGN_EXPR, VALUE, BLOCK,
        IF_ELSE, WHILE, DO_WHILE, CONDITION, FUNCTION_CALL,
        ARGUMENT_LIST, OPERATOR, FUNCTION_DEFINITION,
        FUNCTION_DEF_ARGS, RETURN_EXPR
    };
    struct Vertex {
        Vertex(Type type, std::vector<Vertex> vertices) {
            this->type = type;
            this->vertices = std::vector<Vertex>();
            for (auto v : vertices) {
                this->vertices.push_back(v);
            }
        }
        Vertex(const Vertex& other) {
            this->type = other.type;
            this->vertices = std::vector<Vertex>();
            for (auto v : other.vertices) {
                this->vertices.push_back(v);
            }
        }
        Type type;
        std::vector<Vertex> vertices {};
    };
    Parser();
    int getErrorPlace(std::vector<Token> tokens);
    Vertex getVertex(std::vector<Token> tokens);

private:
    std::vector<Token> tokens;
    int currentTokenIndex;
    
    std::vector<Vertex> vertices;

    void lang();
    void expression();
    void final_value_expr();
    void value_expr();
    void assign_expr();
    void value();
    void block();
    void if_else();
    void while_block();
    void do_while();
    void condition();
    void function_call();
    void argument_list();
    void operator_symbol();
    bool current_token_is_operator();
    void function_def_args();
    void function_definition();
    void return_expr();

    void begining_of_structure(Type type);
    void end_of_structure();

    bool current_token_is(Token::Type type);
    bool accept(Token::Type type);
    void expect(Token::Type type);
};


#endif  // PARSER_H
