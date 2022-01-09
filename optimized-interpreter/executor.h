#ifndef EXECUTOR_H
#define EXECUTOR_H

#include <stdexcept>
#include <map>
#include <memory>
#include <cassert>

#include "pn.h"

class Executor
{
public:

    enum VarType {
        NONE, INT, LIST,
    };
    struct Var;
    struct List;

    Executor() {}

    std::vector<PnToken> pn; // полая PN, состоящая из всех PN, переданных в execute

    // первое измерение соответствует функции, в которой находится область видимости
    // второе измерение соответствует блоку внутри функции
    // третье измерение - поядок появления переменной в текущем блоке
    // scopesVariables[0][0] соответствует глобальному scope; в нем ищутся переменные
    // если не найдутся в текущей функции
    std::vector< std::vector< std::vector<Var> > > scopesVariables {{{}}};
    // std::vector<Var> globalVariables;
    int currentScope = 0; // значение первого измерения scopesVariables

    int currentPosition = 0; // значение текущей позиции в PN
    std::list<Var> operands;  // стек значений

    // позиции операторов FUNCTION_START для каждой функции
    std::map<std::string, int> functionPositions;

    // ищет переменную в доступных областях видимости
    // если не находит, возвращает Var()
    Var& findVarByName(std::string name);
    void addVar(Var var); // добавляет переменную в самый наружний scope
    void setVarValue(Var var, Var valueVar); // добавляет переменную или изменяет значение
    
    
    void addBlockScope(); // добавляет scope в текущей функции
    void removeBlockScope();
    void addFunctionScope();
    void removeFunctionScope();

    void execute(std::vector<PnToken> pn);

    void defineFunction();
    void operation();
    void function();
    void changePosition();

    void print();
    void list();
    void insertElementToList();
    void removeElementFromList();
    void getElementFromList();
    void getListSize();

    void printVar(Var var, bool addEndl = true);
    void checkArgumentsCount(int given, int required, std::string name);
    
    Var literalValue(std::string text) {
        int *val = new int;
        *val = std::stoi(text);
        return Var(text, INT, val);
    }
    Var literalValue(int value) {
        int *val = new int;
        *val = value;
        return Var(std::to_string(value), INT, val);
    }

    struct Var {
        Var()
            : name(""), type(NONE), value(nullptr) {}
        Var(const Var &other) {
            this->name = other.name;
            this->type = other.type;
            this->value = other.value;
        }
        Var(std::string name, VarType type, void* value = nullptr)
            : name(name), type(type), value(value)
        {}
        ~Var() {
            // if (value) {
            //     if (type == INT) {
            //         delete (int*) value;
            //     }
            //     else if (type == LIST) {
            //         delete (List*) value;
            //     }
            // }
        }
        std::string name;
        VarType type;
        void* value { nullptr };
        Var copy() {
            if (!value) {
                throw std::runtime_error("Trying to copy null var value (name is \'" +
                                         name + "\')");
            }
            if (type == INT) {
                int *val = new int;
                *val = *(int*)value;
                return Var(name, type, val);
            }
            else if (type == LIST) {
                // передача списков по ссылке
                return Var(name, type, value);
            }
            assert(false);
        }
    };
    struct ListElement {
        ListElement(Var *value, ListElement* prev = nullptr)
            : value(value), prev(prev), next(nullptr) {}
        Var* value {nullptr};
        ListElement* prev {nullptr};
        ListElement* next {nullptr};
    };
    struct List {
        List() : head(nullptr) {}
        ~List() {
            while (head) {
                ListElement* next = head->next;
                delete head;
                head = next;
            }
        }
        ListElement* head {nullptr};
    };

    Var fictionVar{};
    
};


#endif  // EXECUTOR_H
