#ifndef TRIAD_H
#define TRIAD_H


#include <vector>
#include <string>


#include "pn.h"

struct TriadArgument
{
    TriadArgument(int index)
        : index(index),
          token(PnToken("#NOT_A_TOKEN", PnToken::LITERAL)),
          isPnToken(false) {}
    TriadArgument(PnToken token)
        : index(-1), token(token), isPnToken(true) {}
    int index;
    PnToken token;
    bool isPnToken;

    friend std::ostream& operator<<(std::ostream& out,
                                    const TriadArgument arg) {
        if (arg.isPnToken) {
            out << arg.token;
        }
        else {
            out << "^" << arg.index;
        }
        return out;
    }
};

class Triad
{
public:
    enum Operators {
        ADD, SUB, MUL, DIV, ASSIGN, LESS, LESS_EQ,
        GREATER, GREATER_EQ, EQ, UNEQ,
        AND, OR, GOTO, GOTO_IF_FALSE, FUNCALL,
        END_OF_LINE, FUNCTION_START, FUNCTION_END,
        FUNCTION_DEFINITION, RETURN_OP, ADD_SCOPE,
        REMOVE_SCOPE, GET_TOKEN_POSITION,
        GOTO_DEST, TEMP_TRIAD, // TEMP_TRIAD - временная триада, содержащая литерал/переменную; нужны во время преобразования полиза в триады для 
    };

    Triad();
    Triad(Operators op, std::vector<TriadArgument> args);

    bool operator==(const Triad& other);

    // оператор триады
    Operators op;
    // некоторое количество аргументов триады
    std::vector<TriadArgument> args;

    friend std::ostream& operator<<(std::ostream& out,
                                    const Triad triad) {
        out << "(";
        out << triad.op;
        // if (triad.args.size()) {
        //     out << triad.args[0];
        // }
        for (int i = 0; i < triad.args.size(); i++) {
            out << "," << triad.args[i];
        }
        out << ")";
        return out;
    }
};


#endif  // TRIAD_H
