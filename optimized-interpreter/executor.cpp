#include "executor.h"

#include <cassert>
#include <iostream>

void Executor::execute(std::vector<PnToken> _pn)
{
    currentPosition = pn.size();
    for (int i = 0; i < _pn.size(); i++) {
        pn.push_back(_pn[i]);
    }
    // pn.insert(pn.end(), _pn.begin(), pn.end());

    while (currentPosition < pn.size()) {
        PnToken::Type type = pn[currentPosition].type;
        if (type == PnToken::FUNCTION_DEFINITION) {
            // определение / переопределение функции
            // в конце происходит переход к следующему токену после FUNCTION_END
            defineFunction();
        }
        else if (type == PnToken::LOGIC_OP || type == PnToken::MATH_OP
                 || type == PnToken::COMPARE_OP || type == PnToken::ASSIGN_OP) {
            // если это оператор
            operation();
        }
        else if (type == PnToken::FUNCALL) {
            function();
        }
        else if (type == PnToken::ADD_SCOPE) {
            addBlockScope();
            currentPosition++;
        }
        else if (type == PnToken::REMOVE_SCOPE) {
            removeBlockScope();
            currentPosition++;
        }
        else if (type == PnToken::GOTO || type == PnToken::GOTO_IF_FALSE
                 || type == PnToken::RETURN_OP) {
            changePosition();
        }
        else if (type == PnToken::END_OF_LINE) {
            if (operands.size())
                operands.pop_back();
            currentPosition++;
        }
        else if (type == PnToken::GET_TOKEN_POSITION) {
            operands.push_back(literalValue(std::to_string(currentPosition)));
            currentPosition++;
        }
        else if (type == PnToken::FUNCTION_START) {
            operands.pop_back(); // удаление количества аргументов
            currentPosition++;
        }
        else if (type == PnToken::VAR) {
            Var var = findVarByName(pn[currentPosition].text);
            if (var.type == NONE) {
                var.name = pn[currentPosition].text;
            }
            operands.push_back(var);
            currentPosition++;
        }
        else if (type == PnToken::LITERAL) {
            operands.push_back(literalValue(pn[currentPosition].text));
            currentPosition++;
        }
        else {
            std::cout << "type is: " << type << "\n";
            assert(false);
        }
    }
}


void Executor::defineFunction()
{
    // если функция с таким именем определена, удаляет ее
    // после этого переходит на позицию сразу после FUNCTION_END

    while (pn[currentPosition].type != PnToken::FUNCTION_START) {
        currentPosition++;
    }
    std::string functionName = pn[currentPosition].text;
    auto it = functionPositions.find(functionName);

    if (it != functionPositions.end()) {
        // если уже была определена
        int pos = it->second;
        while (pn[pos].type != PnToken::FUNCTION_DEFINITION) {
            pos--;
        }
        while (pn[pos].type != PnToken::FUNCTION_END) {
            pn.erase(pn.begin() + pos);
            if (pos < currentPosition) {
                currentPosition--;
            }
        }
        // Добавить pn.erase(pn.begin()+pos) для удаления PnToken::Function_End?
        // TODO: пересчитать позиции других функций
        functionPositions.erase(it);
    }
    functionPositions.insert({functionName, currentPosition});
    while (pn[currentPosition].type != PnToken::FUNCTION_END) {
        currentPosition++;
    }
    currentPosition++;
}


#define o(op) \
    value = leftValue op rightValue;
#define e(op) \
    else if (t.text == #op ) {                  \
        value = leftValue op rightValue;        \
    }

void Executor::operation() {
    PnToken t = pn[currentPosition];
    if (operands.size() < 2) {
        throw std::runtime_error("Error: too few operands for operation \'" +
                                 t.text + "\'");
    }
    Var right = operands.back();
    operands.pop_back();
    if (right.type == NONE) {
        // for (int i = 0; i < pn.size(); i++) {
        //     std::cout << i << ", " << pn[i] << "\n";
        // }
        // std::cout << "cur pos: " << currentPosition << "\n";
        // std::cout << "1\n";
        throw std::runtime_error("Error: trying to use undefined variable '"
                                 + right.name + "'");
    }
    Var left = operands.back();
    operands.pop_back();
    if (t.type == PnToken::ASSIGN_OP) {
        left.type = right.type;
        setVarValue(left, right.copy());
        operands.push_back(findVarByName(left.name));
    }
    else {
        if (left.type == NONE) {
            throw std::runtime_error("Error: trying to use undefined variable '"
                                     + left.name + "'");
        }
        int value = 0;
        if (right.type != INT) {
            throw std::runtime_error("Error: trying to use operator \'" +
                                     t.text + "\' with operand type: "
                                     + std::to_string(right.type));
        }
        if (left.type != INT) {
            throw std::runtime_error("Error: trying to use operator \'" +
                                     t.text + "\' with operand type: "
                                     + std::to_string(left.type));
        }
        int leftValue = *(int*) left.value;
        int rightValue = *(int*) right.value;
        if (t.text == "+") {
            o(+);
        }
        e(-)
            e(*)
            e(/)
            e(<)
            e(<=)
            e(>)
            e(>=)
            e(&&)
            e(||)
        else {
            assert(false);
        }
        operands.push_back(literalValue(value));
    }
    currentPosition++;
}

#undef o
#undef e


void Executor::function()
{
    assert(pn[currentPosition].type == PnToken::FUNCALL);

    assert(operands.size() >= 2);
    assert(operands.back().type == INT);

    std::string functionName = pn[currentPosition].text;

    if (functionName == "print") {
        print();
        currentPosition++;
    }
    else if (functionName == "list") {
        list();
        currentPosition++;
    }
    else if (functionName == "insert") {
        insertElementToList();
        currentPosition++;
    }
    else if (functionName == "remove") {
        removeElementFromList();
        currentPosition++;
    }
    else if (functionName == "get") {
        getElementFromList();
        currentPosition++;
    }
    else if (functionName == "sizeOf") {
        getListSize();
        currentPosition++;
    }
    else {
        // это функция определенная пользователем
        int returnTo = *(int*)operands.back().value;
        operands.pop_back();
        assert(operands.back().type == INT);
        int argsCount = *(int*) operands.back().value;
        operands.pop_back();

        assert(operands.size() >= argsCount);

        auto it = functionPositions.find(functionName);
        if (it == functionPositions.end()) {
            throw std::runtime_error("Function '" + functionName +
                                     "' is not defined!");
        }

        int functionPos = it->second;

        assert(pn[functionPos-1].type == PnToken::LITERAL);
        int realArgsCount = std::stoi(pn[functionPos-1].text);
        // в realArgsCount учитывается аргумента-позиция куда вернуться
        // а в argsCount - нет
        if (realArgsCount != argsCount+1) {
            throw std::runtime_error("Trying to call function '" + functionName
                                     + "' with wrong arguments count: passed "
                                     + std::to_string(argsCount) + " instead of "
                                     + std::to_string(realArgsCount-1));
        }

        // последний аргумент находится на functionPos - 4
        // первый на functionPos - 4*(realArgsCount)
        // начать выполнение функции необходимо с functionPos - 4*(realArgsCount) - 1

        addFunctionScope();

        pn[functionPos - 4*(realArgsCount)] =
            PnToken(std::to_string(returnTo), PnToken::LITERAL);
        std::vector<Var> args;
        for (int i = 0; i < argsCount; i++) {
            // i = 0 => последний аргумент
            Var arg = operands.back();
            operands.pop_back();
            if (arg.type == NONE) {
                throw std::runtime_error("Error: trying to use undefined variable '"
                                         + arg.name + "'");
            }
            int argPos = functionPos - 4 * (i+1);
            arg.name = "#arg" + std::to_string(argsCount - i);
            addVar(arg.copy());
            pn[argPos] = PnToken(arg.name, PnToken::VAR);
        }

        // переходим к месту присвоения аргументов функции
        currentPosition = functionPos - 4*(realArgsCount) - 1; 
    }
}

void Executor::changePosition()
{
    PnToken::Type type = pn[currentPosition].type;
    if (type == PnToken::GOTO) {
        assert(operands.size() >= 1);
        assert(operands.back().type == INT);
        int val = *(int*)operands.back().value;
        operands.pop_back();
        if (val >= 0) {
            for (; val >= 0; val--) {
                currentPosition++;
            }
        }
        else {
            for (; val <= 0; val++) {
                currentPosition--;
            }
        }
    }
    else if (type == PnToken::GOTO_IF_FALSE) {
        assert(operands.size() >= 2);
        assert(operands.back().type == INT);
        int val = *(int*)operands.back().value;
        operands.pop_back();
        assert(operands.back().type == INT);
        int cond = *(int*)operands.back().value;
        operands.pop_back();
        if (!cond) {
            if (val >= 0) {
                for (; val > 0; val--) {
                    currentPosition++;
                }
            }
            else {
                for (; val <= 0; val++) {
                    currentPosition--;
                }
            }
        }
        else {
            currentPosition++;
        }
    }
    else if (type == PnToken::RETURN_OP) {
        assert(operands.size() >= 2);
        assert(operands.back().type == INT);
        Var value = operands.back();
        operands.pop_back();
        assert(operands.back().type == INT);
        int returnTo = *(int*)operands.back().value;
        operands.pop_back();

        operands.push_back(value);
        currentPosition = returnTo;
        removeFunctionScope();
    }
    else {
        assert(false);
    }
}

void Executor::print()
{
    operands.pop_back(); // удаляем место возврата
    assert(operands.back().type == INT);
    int args = *(int*) operands.back().value;
    operands.pop_back();
    checkArgumentsCount(args, 1, "print");
    assert(operands.size());
    Var var = operands.back();
    operands.pop_back();

    printVar(var);
    operands.push_back(literalValue(0));
}

void Executor::printVar(Var var, bool addEndl)
{
    if (var.type == INT) {
        std::cout << *(int*) var.value;
    }
    else if (var.type == LIST) {
        std::cout << "[";
        ListElement* e = ((List*) var.value)->head;
        if (e) {
            printVar(*(e->value), false);
            e = e->next;
        }
        while (e) {
            std::cout << ",";
            printVar(*(e->value), false);
            e = e->next;
        }
        std::cout << "]";
    }
    if (addEndl) {
        std::cout << std::endl;
    }
}

void Executor::list()
{
    // insert(list, pos, value);
    operands.pop_back(); // удаляем место возврата
    int argsCount = *(int*) operands.back().value;
    operands.pop_back(); // удаляем количество аргументов
    checkArgumentsCount(argsCount, 0, "list");
    Var var("#temp", LIST, new List());
    operands.push_back(var);
}

void Executor::insertElementToList()
{
    // insert(list, pos, value);
    operands.pop_back(); // удаляем место возврата
    int argsCount = *(int*) operands.back().value;
    operands.pop_back();
    checkArgumentsCount(argsCount, 3, "insert");
    Var value = operands.back();
    operands.pop_back();
    Var posVar = operands.back();
    operands.pop_back();
    if (posVar.type != INT) {
        throw std::runtime_error("Error: argument 'pos' should be integer");
    }
    int pos = *(int*)posVar.value;
    Var list = operands.back();
    operands.pop_back();
    if (list.type != LIST) {
        throw std::runtime_error("Error: argument 'list' should be List");
    }

    ListElement *e = ((List*)list.value)->head;
    bool addToPrev = true;
    for (int i = 0; i < pos; i++) {
        if (e) {
            if (e->next) {
                e = e->next;
            }
            else {
                // добавить в конец списка
                addToPrev = false;
            }
        }
    }
    Var *v = new Var;
    *v = value;
    if (!e) {
        // если в списке 0 элементов
        ((List*)list.value)->head = new ListElement(v, nullptr);
    }
    else {
        ListElement* l;
        if (addToPrev) {
            ListElement* prev = e->prev;
            l = new ListElement(v, prev);
            if (prev) {
                prev->next = l;
            }
            l->next = e;
            e->prev = l;
        }
        else {
            ListElement* next = e->next;
            l = new ListElement(v, e);
            if (next) {
                next->prev = l;
            }
            l->next = next;
            e->next = l;
        }
        if (pos == 0) {
            ((List*)list.value)->head = l;
        }
    }
    setVarValue(list, list);
    operands.push_back(literalValue(0));
}

void Executor::removeElementFromList()
{
    // remove(list, pos);
    operands.pop_back(); // удаляем место возврата
    int argsCount = *(int*) operands.back().value;
    operands.pop_back();
    checkArgumentsCount(argsCount, 2, "remove");
    Var posVar = operands.back();
    operands.pop_back();
    if (posVar.type != INT) {
        throw std::runtime_error("Error: argument 'pos' should be integer");
    }
    int pos = *(int*)posVar.value;
    Var list = operands.back();
    operands.pop_back();
    if (list.type != LIST) {
        throw std::runtime_error("Error: argument 'list' should be List");
    }

    ListElement *e = ((List*)list.value)->head;
    bool shouldRemove = (e != nullptr);
    for (int i = 0; i < pos; i++) {
        if (e) {
            if (!e->next) {
                shouldRemove = false;
            }
            else {
                e = e->next;
            }
        }
    }
    if (shouldRemove && (pos >= 0)) {
        ListElement *next = e->next, *prev = e->prev;
        if (prev) {
            e->prev->next = next;
        }
        if (next) {
            e->next->prev = prev;
        }
        if (pos == 0) {
            ((List*)list.value)->head = next;
        }
    }

    operands.push_back(literalValue(0));
}

void Executor::getElementFromList()
{
    // get(list, pos);
    operands.pop_back(); // удаляем место возврата
    int argsCount = *(int*) operands.back().value;
    operands.pop_back();
    checkArgumentsCount(argsCount, 2, "get");
    Var posVar = operands.back();
    operands.pop_back();
    if (posVar.type != INT) {
        throw std::runtime_error("Error: argument 'pos' should be integer");
    }
    int pos = *(int*)posVar.value;
    Var list = operands.back();
    operands.pop_back();
    if (list.type != LIST) {
        throw std::runtime_error("Error: argument 'list' should be List");
    }

    ListElement *e = ((List*)list.value)->head;
    for (int i = 0; i < pos; i++) {
        if (!e) {
            throw std::runtime_error("Error: get(): size of list is less then pos");
        }
        e = e->next;
    }
    if (!e) {
        throw std::runtime_error("Error: get(): size of list is less then pos");
    }

    operands.push_back(*e->value);
}

void Executor::getListSize()
{
    // sizeOf(list);
    operands.pop_back(); // удаляем место возврата
    int argsCount = *(int*) operands.back().value;
    operands.pop_back();
    checkArgumentsCount(argsCount, 1, "get");
    Var list = operands.back();
    operands.pop_back();
    if (list.type != LIST) {
        throw std::runtime_error("Error: argument 'list' should be List");
    }

    ListElement *e = ((List*)list.value)->head;
    int size = 0;
    while (e) {
        size++;
        e = e->next;
    }
    operands.push_back(literalValue(size));
}

void Executor::checkArgumentsCount(int given, int required, std::string name)
{
    if (given != required) {
        throw std::runtime_error("Error: trying to call funcion "
                                 + name + "with wrong argument: passed "
                                 + std::to_string(given) + ", required "
                                 + std::to_string(required));
    }
}

Executor::Var& Executor::findVarByName(std::string name)
{
    for (int i = scopesVariables[currentScope].size()-1; i >= 0; i--) {
        // цикл по всем вложенным scope
        for (int j = 0; j <  scopesVariables[currentScope][i].size(); j++) {
            if (scopesVariables[currentScope][i][j].name == name) {
                return scopesVariables[currentScope][i][j];
            }
        }
    }
    if (currentScope != 0) {
        for (int j = 0; j <  scopesVariables[currentScope][0].size(); j++) {
            if (scopesVariables[currentScope][0][j].name == name) {
                return scopesVariables[currentScope][0][j];
            }
        }
    }
    return fictionVar;
}

void Executor::addVar(Var var)
{
    scopesVariables[currentScope]
        [scopesVariables[currentScope].size()-1].push_back(var);
}

void Executor::setVarValue(Var var, Var valueVar)
{
    Var &v = findVarByName(var.name);
    if (v.type != NONE) {
        v.value = valueVar.copy().value;
    }
    else {
        var.value = valueVar.copy().value;
        addVar(var);
    }
}

void Executor::addBlockScope()
{
    scopesVariables[currentScope].
        push_back(std::vector<Var>());
}
void Executor::removeBlockScope()
{
    // нет вложенных блоков => нечего удалять
    assert(!(currentScope == 0 && scopesVariables[0].size() == 1));

    scopesVariables[currentScope].pop_back();
}

void Executor::addFunctionScope()
{
    scopesVariables.
        push_back(std::vector< std::vector<Var> >({std::vector<Var>()}));
    currentScope++;
}

void Executor::removeFunctionScope()
{
    // есть только глобальный scope => нечего удалять
    assert(currentScope != 0);
    scopesVariables.pop_back();
    currentScope--;
}
