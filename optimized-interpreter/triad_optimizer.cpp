#include "triad_optimizer.h"

#include <functional>
#include <set>
#include <cassert>
#include <iostream>

void print(std::vector<PnToken> tokens) {
    for (auto t : tokens) {
        std::cout << t.text << ", ";
    }
}

bool isOperator(PnToken token) {
    PnToken::Type t = token.type;
    return t == PnToken::LOGIC_OP || t == PnToken::COMPARE_OP
        || t == PnToken::MATH_OP || t == PnToken::ASSIGN_OP
        || t == PnToken::GOTO || t == PnToken::GOTO_IF_FALSE
        || t == PnToken::RETURN_OP || t == PnToken::FUNCALL;
}

bool isFunctionalPnToken(PnToken token) {
    PnToken::Type t = token.type;
    return isOperator(token) || t == PnToken::END_OF_LINE
        || t == PnToken::ADD_SCOPE || t == PnToken::REMOVE_SCOPE
        || t == PnToken::FUNCTION_START || t == PnToken::FUNCTION_END
        || t == PnToken::FUNCTION_DEFINITION
        || t == PnToken::GET_TOKEN_POSITION;
        // || t == PnToken::LABEL;
}

#define e(type) \
    || op == Triad:: type

bool isTriadHasArgs(Triad triad) {
    Triad::Operators op = triad.op;
    return op == Triad::ADD
        e(SUB)
        e(MUL)
        e(DIV)
        e(ASSIGN)
        e(LESS)
        e(LESS_EQ)
        e(GREATER)
        e(GREATER_EQ)
        e(EQ)
        e(UNEQ)
        e(AND)
        e(OR)
        e(GOTO_IF_FALSE)
        e(FUNCALL)
        e(RETURN_OP)
        e(FUNCTION_START); // FUNCTION_START имеет один аргумент - количество аргументов функции
}
#undef e

#define e(type) || op == Triad:: type
int getOperatorTriadArgsCount(Triad triad) {
    Triad::Operators op = triad.op;
    if (op == Triad::ADD
        e(SUB)
        e(MUL)
        e(DIV)
        e(ASSIGN)
        e(LESS)
        e(LESS_EQ)
        e(GREATER)
        e(GREATER_EQ)
        e(EQ)
        e(UNEQ)
        e(AND)
        e(OR)
        e(FUNCTION_START)
        e(RETURN_OP)
        e(GOTO_IF_FALSE)) {
        return 2;
    }
    else if (op == Triad::GET_TOKEN_POSITION
             e(END_OF_LINE)
             // e(FUNCTION_START)
             e(FUNCTION_END)
             e(FUNCTION_DEFINITION)
             e(ADD_SCOPE)
             // e(LABEL)
             e(REMOVE_SCOPE)) {
        return 0;
    }
    else if (op == Triad::GOTO) {
        return 1;
    }
    assert(false);
}
#undef e

#define e(type) \
    else if (t == PnToken:: type) { \
        op = Triad:: type; \
    }
#define a(o, type) \
    else if (text == #o ) { \
        op = Triad:: type; \
    }
Triad getTriadByPnToken(PnToken token) {
    // Создает триаду с пустым массивом аргументов и правильным оператором
    PnToken::Type t = token.type;
    std::string text = token.text;
    Triad::Operators op;
    if (t == PnToken::END_OF_LINE) {
        op = Triad::END_OF_LINE;
    }
    e(GOTO)
        e(GOTO_IF_FALSE)
        e(FUNCALL)
        e(ADD_SCOPE)
        e(REMOVE_SCOPE)
        e(RETURN_OP)
        e(FUNCTION_START)
        e(FUNCTION_END)
        e(FUNCTION_DEFINITION)
        e(GET_TOKEN_POSITION)
        // e(LABEL)
    if (text == "+") {
        op = Triad::ADD;
    }
    a(-, SUB)
        a(*, MUL)
        a(/, DIV)
        a(=, ASSIGN)
        a(<, LESS)
        a(<=, LESS_EQ)
        a(>, GREATER)
        a(>=, GREATER_EQ)
        a(==, EQ)
        a(!=, UNEQ)
        a(&&, AND)
        a(||, OR)

    return Triad(op, {});
}
#undef e
#undef a

#define o(operator, triadType, tokenType)  \
    else if (op == Triad:: triadType) { \
        text = #operator;\
        type = PnToken:: tokenType;\
    }
#define m(op, type) o(op, type, MATH_OP)
#define c(op, type) o(op, type, COMPARE_OP)
#define l(op, type) o(op, type, LOGIC_OP)
#define a(op, type) o(op, type, ASSIGN_OP)

#define t(_type, te)\
    else if (op == Triad:: _type) {\
        text = te;                \
        type = PnToken:: _type;      \
    }

PnToken getPnTokenByTriad(Triad triad) {
    // Создает токен с соответствующим оператором
    std::string text;
    PnToken::Type type;

    Triad::Operators op = triad.op;

    if (op == Triad::ADD) {
        text = "+";
        type = PnToken::MATH_OP;
    }
    m(-, SUB)
        m(*, MUL)
        m(/, DIV)
        c(>, GREATER)
        c(>=, GREATER_EQ)
        c(<, LESS)
        c(<=, LESS_EQ)
        c(==, EQ)
        c(!=, UNEQ)
        l(&&, AND)
        l(||, OR)
        a(=, ASSIGN)
        t(END_OF_LINE, ";")
        t(GOTO, "!!")
        t(GOTO_IF_FALSE, "!F")
        t(FUNCALL, "#FUNCALL")
        t(ADD_SCOPE, "#ADD_SCOPE")
        t(REMOVE_SCOPE, "#REMOVE_SCOPE")
        t(GET_TOKEN_POSITION, "#GET_TOKEN_POSITION")
        t(RETURN_OP, "#RETURN_OP")
        t(FUNCTION_START, "#FUNCTION_START")
        t(FUNCTION_END, "#FUNCTION_END")
        t(FUNCTION_DEFINITION, "#FUNCTION_DEFINITION")
    else {
        assert(false);
    }
    return PnToken(text, type);
}
#undef o
#undef m
#undef c
#undef l
#undef a
#undef t

std::vector<PnToken> TriadOptimizer::optimize(std::vector<PnToken> pn)
{
    std::vector<Triad> triads = getTriads(pn);
    triads = optimizeConstants(triads);
    std::vector<PnToken> result = generatePn(triads);

    return result;
}

std::vector<Triad> TriadOptimizer::getTriads(std::vector<PnToken> tokens)
{
    std::vector<Triad> res;
    // В pn места переходов goto всегда заданы константами
    // В триадах на место перехода всегда добавляется триада
    //   (GOTO_DEST,PnToken(LITERAL, "последовательный_номер_перехода"))
    // На место, откуда идет переход, добавляется триада
    //  (GOTO/GOTO_IF_NOT, {PnToken(LITERAL, "последовательный_номер_перехода"), условие})
    // В pn !F задан следующим образом: условие, место_перехода, !F
    
    // При преобразовании для начала создается массив триад,
    // который фактически является полизом, записанным другими структурами
    // То есть для каждого оператора создается триада (OPERATOR, {}),
    // а для каждой переменной или литерала - (TEMP_TRIAD, {переменная/литерал})
    // Далее для всех переходов goto вычисляются их абсолютные значения позиций
    // Все операторы goto получают порядковый номер
    // Далее на всех позициях, куда ведут goto, добавляется триада
    //   (GOTO_DEST, PnToken(LITERAL, порядковый_номер_goto))
    // После каждого добавления такой триады при необходимости
    // обновляются абсолютные значения позиций переходов

    // После того, как все переходы будут заданы, все TEMP_TRIAD распределяются
    // по триадам.
    // В первую очередь удаляются все TEMP_TRIAD, стоящие непосредственно перед goto
    // Все не TEMP_TRIAD триады последовательно нумеруются
    // У каждой не TEMP_TRIAD определяется количество аргументов
    // Далее последовательно просматриваются все триады
    // Если текущая триада - операция с N аргументами, то начинается просмотр
    // в обратную сторону.
    // Если встречается непомеченная триада TEMP_TRIAD, то она добавляется в аргументы текущей триады
    // и помечается
    // Если встречается другая непомеченная триада, то добавляем в аргументы
    // последовательный номер этой триады и помечаем ее
    // Если добавлено необходимое количество аргументов, то продолжаем просмотр вперед

    // Создание полиз-подобного массива триад
    for (auto it = tokens.begin(); it != tokens.end(); it++) {
        Triad triad;
        if (isFunctionalPnToken(*it)) {
            triad = getTriadByPnToken(*it);
            if (triad.op == Triad::FUNCALL) {
                // Добавление имени функции в триаду
                triad.args.push_back(TriadArgument(*it));
            }
            else if (triad.op == Triad::FUNCTION_START) {
                // задание имени у функции
                triad.args.push_back(TriadArgument(*it));
            }
        }
        else if (it->type == PnToken::VAR || it->type == PnToken::LITERAL
                 || it->type == PnToken::LABEL) {
            triad = Triad(Triad::TEMP_TRIAD, {TriadArgument(*it)});
        }
        else {
            assert(false);
        }
        res.push_back(triad);
    }

    // auto printRes = [&] () {
    //     for (int i = 0; i < res.size(); i++) {
    //         std::cout << i << ": " << res[i] << "\n";
    //     }
    // };
    // std::cout << "pn-res:\n";
    // printRes();
    std::vector<int> gotoDest;

    // получения массива абсолютных значений переходов goto
    for (auto it = res.begin(); it != res.end(); it++) {
        if (it->op == Triad::TEMP_TRIAD) {
            if (it + 1 != res.end() &&
                ((it+1)->op == Triad::GOTO
                 || (it+1)->op == Triad::GOTO_IF_FALSE)) {
                (it+1)->args = {TriadArgument(PnToken(std::to_string(gotoDest.size()),
                                                    PnToken::LITERAL))};
                int currentPos = std::distance(res.begin(), it+1);
                int destPos = currentPos + std::stoi(it->args[0].token.text);
                gotoDest.push_back(destPos);
            }
        }
    }

    // Добавление триад - меток переходов
    for (int i = 0; i < gotoDest.size(); i++) {
        res.insert(res.begin() + gotoDest[i],
                   Triad(Triad::GOTO_DEST,
                         {PnToken(std::to_string(i), PnToken::LITERAL)}));
        for (int j = i+1; j < gotoDest.size(); j++) {
            if (gotoDest[j] >= gotoDest[i]) {
                gotoDest[j]++;
            }
        }
    }

    // Удаление TEMP_TRIAD, содержащих значение для перехода goto
    for (int i = 0; i < res.size() - 1; i++) {
        if (res[i].op == Triad::TEMP_TRIAD &&
            (res[i+1].op == Triad::GOTO
             || res[i+1].op == Triad::GOTO_IF_FALSE)) {
            res.erase(res.begin() + i);
            i--;
        }
    }

    // printRes();
    // Нумерация триад-операторов и определение количества аргументов

    // Если триада находится на res[pos],
    // и это триада-оператор под номером n,
    // то triadIndexes[pos] == n
    std::map<int,int> triadIndexes; 
    std::map<int,int> triadArgsCount;
    for (int i = 0; i < res.size(); i++) {
        if (res[i].op != Triad::TEMP_TRIAD) {
            int index = triadIndexes.size();
            triadIndexes.insert({i, index});
            int count;
            if (res[i].op == Triad::GOTO_DEST) {
                count = 0;
            }
            else if (res[i].op == Triad::FUNCALL) {
                // Структура вызова функции:
                // args... argsCount 3 GET_TOKEN_POSITION + FUNCALL
                // res[i-3] - константа TEMP_TRIAD
                // Фактическое количество аргументов = argsCount + 3
                //   (имя функции, returnTo, argsCount)
                assert(i-4 >= 0);
                assert(res[i-1].op == Triad::ADD);
                assert(res[i-2].op == Triad::GET_TOKEN_POSITION);
                assert(res[i-3].args.size() == 1);
                assert(res[i-3].args[0].token.type == PnToken::LITERAL);
                assert(res[i-4].args.size() == 1);
                assert(res[i-4].args[0].token.type == PnToken::LITERAL);
                count = std::stoi(res[i-4].args[0].token.text) + 3;
            }
            else {
                count = getOperatorTriadArgsCount(res[i]);
            }
            triadArgsCount.insert({index, count});
        }
    }

    // Распределение аргументов
    std::set<int> marked;
    for (int i = 0; i < res.size(); i++) {
        if (isTriadHasArgs(res[i])) {
            for (int j = i-1; res[i].args.size() < triadArgsCount[triadIndexes[i]]; j--) {
                assert(j >= 0);
                if (!marked.count(j)) {
                    if (res[j].op == Triad::TEMP_TRIAD) {
                        assert(res[j].args[0].isPnToken);
                        res[i].args.push_back(TriadArgument(res[j].args[0].token));
                    }
                    else {
                        res[i].args.push_back(TriadArgument(triadIndexes[j]));
                    }
                    marked.insert(j);
                }
            }
        }
    }

    // std::cout << "triads after setting args:\n";
    // printRes();
    // Удаление TEMP_TRIAD из массива
    for (int i = 0; i < res.size(); i++) {
        if (res[i].op == Triad::TEMP_TRIAD) {
            res.erase(res.begin()+i);
            i--;
        }
    }

    return res;
}


bool isTriadConstant(Triad triad) {
    // Проверяет, являются ли литералами аргументы триады
    for (auto to : triad.args) {
        if (!to.isPnToken) {
            return false;
        }
        else {
            if (to.token.type != PnToken::LITERAL) {
                return false;
            }
        }
    }
    return true;
}

#define e(type) || op == Triad:: type
bool isTriadComputable(Triad triad) {
    Triad::Operators op = triad.op;
    return op == Triad::ADD
        e(SUB)
        e(MUL)
        e(DIV)
        e(LESS)
        e(LESS_EQ)
        e(GREATER)
        e(GREATER_EQ)
        e(EQ)
        e(UNEQ)
        e(AND)
        e(OR);
}
#undef e

#define e(o, type) \
    else if (op == Triad:: type) { \
        return std::stoi(triad.args[1].token.text) o \
            std::stoi(triad.args[0].token.text);\
    }

int computeConstantTriad(Triad triad) {
    assert(isTriadConstant(triad));
    Triad::Operators op = triad.op;
    assert(triad.args.size() == 2);
    if (op == Triad::ADD) {
        return std::stoi(triad.args[1].token.text) +
            std::stoi(triad.args[0].token.text);
    }
    e(-, SUB)
        e(*, MUL)
        e(/, DIV)
        e(<, LESS)
        e(<=, LESS_EQ)
        e(>, GREATER)
        e(>=, GREATER_EQ)
        e(==, EQ)
        e(!=, UNEQ)
        e(&&, AND)
        e(||, OR);
    assert(false);
}
#undef e

std::vector<Triad> TriadOptimizer::optimizeConstants(std::vector<Triad> triads) {
    auto replaceTriadReferenceWithValue = [&] (int triadIndex, int value) {
        bool changed = false;
        std::string val = std::to_string(value);
        for (int i = 0; i < triads.size(); i++) {
            for (int j = 0; j < triads[i].args.size(); j++) {
                if (!triads[i].args[j].isPnToken) {
                    if (triads[i].args[j].index == triadIndex) {
                        triads[i].args[j] = TriadArgument(PnToken(val, PnToken::LITERAL));
                        changed = true;
                    }
                }
            }
        }
        return changed;
    };

    // Индексы триад, которые нужно удалить,
    // потому что они были предвычислены и заменились на константы
    std::vector<int> indexesToRemove;
    bool wasOptimized = false;
    do {
        wasOptimized = false;
        for (int i = 0; i < triads.size(); i++) {
            if (isTriadComputable(triads[i]) && isTriadConstant(triads[i])) {
                if (std::find(indexesToRemove.begin(),
                              indexesToRemove.end(),
                              i) == indexesToRemove.end()) {
                    indexesToRemove.push_back(i);
                    int value = computeConstantTriad(triads[i]);
                    if (replaceTriadReferenceWithValue(i, value)) {
                        wasOptimized = true;
                    }
                }
            }
        }
    } while (wasOptimized);
    std::sort(indexesToRemove.begin(), indexesToRemove.end(),
              std::greater<int>());
    for (int index : indexesToRemove) {
        triads.erase(triads.begin()+index);
        for (int i = 0; i < triads.size(); i++) {
            for (int j = 0; j < triads[i].args.size(); j++) {
                if (!triads[i].args[j].isPnToken &&
                    triads[i].args[j].index > index) {
                    triads[i].args[j].index--;
                }
            }
        }
    }

    return triads;
}

std::vector<Triad>
TriadOptimizer::optimizeSameOperations(std::vector<Triad> triads,
                                       std::vector<std::pair<int, int>> blocks)
{
    // Если искать одинаковые триады, начиная с начала,
    // то может оказаться, что данная пара одинаковых триад
    // используется в других одинаковых триадах, из-за чего
    // для этих триад не нужно вводить дополнительную переменную

    // Решить эту проблему можно, если начинать искать с конца массива

    // Рассматриваются только триады математических/логических операций
    
    // Две триады считаются одинаковыми, если:
    // 1. Их константные аргументы одинаковые
    // 2. Их аргументы-переменные одинаковые, и между
    //    этими триадами нет присвоений этим переменным
    // 3. Их аргументы-триады одинаковые

    // При выполнении кода после ASSIGN_OP в стек добавляется
    // значение переменной, которой присваивали новое значение

    // Для оптимизации одинаковых триад:
    // 1. После первой из них добавляется триада
    //    (ASSIGN_OP, {^first_triad, temp_name})
    //    После пересчитываются номера всех триад (?)
    //    Определяется триада, которая использует эту триаду
    //    Ссылка на эту триаду заменяется на ссылку на добавленную триаду ASSIGN_OP
    // 2. Для каждой следующей определяется триада, в которой она используется
    //    Она может использоваться в 0 или 1 триаде
    //    Если она не используется, она помечается на удаление
    //    Если она используется, то ссылка на эту триаду в использующей заменяется
    //    на temp_name, и сама триада помечается на удаление
    //    Все триады, на которые рекурсивно указывает данная, помечаются на удаление

    // После удаления триад пересчитываются номера оставшихся

    int newVariablesCount = 0;

    // auto printTriads = [&] () {
    //     for (int i = 0; i < triads.size(); i++) {
    //         std::cout << i << ": " << triads[i] << "\n";
    //     }
    // };

    auto isOperationTriad = [=] (Triad triad) {
        Triad::Operators op = triad.op;
        switch (op) {
        case Triad::ADD:
        case Triad::SUB:
        case Triad::MUL:
        case Triad::DIV:
        case Triad::LESS:
        case Triad::LESS_EQ:
        case Triad::GREATER:
        case Triad::GREATER_EQ:
        case Triad::EQ:
        case Triad::UNEQ:
        case Triad::AND:
        case Triad::OR:
            return true;
        default:
            return false;
        }
    };

    std::function<bool(std::string, int,int)> hasVarAssignBetween =
        [&] (std::string var, int i, int j) {
            assert(i <= j);
            for (int k = i; k <= j; k++) {
                assert(k < triads.size());
                if (triads[k].op == Triad::ASSIGN) {
                    assert(triads[k].args.size() == 2);
                    assert(triads[k].args[1].isPnToken);
                    assert(triads[k].args[1].token.type == PnToken::VAR);
                    if (triads[k].args[1].token.text == var) {
                        return true;
                    }
                }
            }
            return false;
        };
    std::function<bool(int,int)> isEqual = [&] (int i, int j) {
        if (j < i) {
            std::swap(i, j);
        }
        Triad first = triads[i], second = triads[j];
        if (first.op != second.op) {
            return false;
        }
        if (first.args.size() != second.args.size()) {
            return false;
        }
        for (int k = 0; k < first.args.size(); k++) {
            if (first.args[k].isPnToken != second.args[k].isPnToken) {
                return false;
            }
            if (first.args[k].isPnToken) {
                PnToken firstPnToken = first.args[k].token,
                    secondPnToken = second.args[k].token;
                if (firstPnToken.type != secondPnToken.type) {
                    return false;
                }
                if (firstPnToken.type == PnToken::LITERAL) {
                    if (firstPnToken.text != secondPnToken.text) {
                        return false;
                    }
                }
                else if (firstPnToken.type == PnToken::VAR) {
                    if (firstPnToken.text != secondPnToken.text) {
                        return false;
                    }
                    if (hasVarAssignBetween(firstPnToken.text, i, j)) {
                        return false;
                    }
                }
                else {
                    assert(false);
                }
            }
            else {
                if (!isEqual(first.args[k].index,
                             second.args[k].index)) {
                    return false;
                }
            }
        }
        return true;
    };

    auto changeTriadIndexes = [&] (int indexFrom, int changeFrom, int changeBy) {
        for (int i = indexFrom; i < triads.size(); i++) {
            for (auto &to : triads[i].args) {
                if (!to.isPnToken) {
                    if (to.index >= changeFrom) {
                        to.index += changeBy;
                    }
                }
            }
        }
    };

    std::function<void(int)> markToRemove = [&] (int triadIndex) {
        assert(triads[triadIndex].op != Triad::TEMP_TRIAD);
        triads[triadIndex].op = Triad::TEMP_TRIAD;
        for (auto ta : triads[triadIndex].args) {
            if (!ta.isPnToken) {
                markToRemove(ta.index);
            }
        }
    };
    for (std::pair<int,int> block : blocks) {
        for (int i = block.second; i >= block.first; i--) {
            if (!isOperationTriad(triads[i])) {
                continue;
            }
            std::vector<int> sameTriads = {i};
            for (int j = i-1; j >= block.first; j--) {
                if (isEqual(j, i)) {
                    sameTriads.insert(sameTriads.begin(), j);
                }
            }
            if (sameTriads.size() > 1) {
                // Запись значения первой триады в новую переменную
                PnToken var = PnToken("#TEMP_VAR_" + std::to_string(newVariablesCount),
                                  PnToken::VAR);
                newVariablesCount++;
                int firstTriad = sameTriads[0];
                Triad assignTriad = Triad(Triad::ASSIGN,
                                          {TriadArgument(firstTriad),
                                          TriadArgument(var)});
                // std::cout << "triads indexed: ";
                // for (auto i : sameTriads) {
                //     std::cout << i << ", ";
                // }
                // Помечаем как TEMP_TRIAD эквивалентные триады и
                // триады, на которые они рекунсивно ссылаются
                for (int k = 1; k < sameTriads.size(); k++) {
                    markToRemove(sameTriads[k]);
                }
                // std::cout << "\n";
                triads.insert(triads.begin() + firstTriad + 1, assignTriad);
                // std::cout << "trida after insertion at " << firstTriad+1 << "\n";
                // printTriads();


                // Заменяем ссылки на эквивалентные триады значением по var
                for (int k = 1; k < sameTriads.size(); k++) {
                    for (int i = sameTriads[k] + 1; i < triads.size(); i++) {
                        for (int j = 0; j < triads[i].args.size(); j++) {
                            if (!triads[i].args[j].isPnToken &&
                                triads[i].args[j].index == sameTriads[k]) {
                                triads[i].args[j] = TriadArgument(var);
                            }
                        }
                    }
                }
                // std::cout << "triads after relinking to var:\n";
                // printTriads();

                // Добавили одну триаду, так что у всех следующих повышаем номер на 1
                // Начиная с триады после ASSIGN увеличиваем все ссылки, большие или равные первой триаде, на 1
                // Таким образом ссылка на первую триаду превратится в ссылку на ASSIGN
                // std::cout << "change from first triad: " << firstTriad << "\n";
                changeTriadIndexes(firstTriad + 2, firstTriad, 1);

                // std::cout << "triads after changing indexed\n";
                // printTriads();
            }
        }
    }

    // Удаление помеченных триад и корректировка индексов триад
    for (int i = 0; i < triads.size(); i++) {
        if (triads[i].op == Triad::TEMP_TRIAD) {
            triads.erase(triads.begin() + i);
            // Начиная с триады, идущей после удаленной,
            // уменьшить на 1 ссылки на все триады, начиная с удаленной
            changeTriadIndexes(i, i, -1);
            i--;
        }
    }

    return triads;
}

std::vector<Triad>
TriadOptimizer::optimizeAssigns(std::vector<Triad> triads,
                                std::vector<std::pair<int,int>> blocks)
{

    // Находятся пары соседних ближайших присвоений одной переменной
    // в одном блоке.
    // В такой паре первое присвоение считается бессмысленным, если:
    // 1. Эта переменная не используется в триадах между присвоениями
    // 2. В присваемом первый раз выражении нет вызовов функций

    // Лямбды из optimizeSameOperations
    auto changeTriadIndexes = [&] (int indexFrom, int changeFrom, int changeBy) {
        for (int i = indexFrom; i < triads.size(); i++) {
            for (auto &to : triads[i].args) {
                if (!to.isPnToken) {
                    if (to.index >= changeFrom) {
                        to.index += changeBy;
                    }
                }
            }
        }
    };
    std::function<void(int)> markToRemove = [&] (int triadIndex) {
        assert(triads[triadIndex].op != Triad::TEMP_TRIAD);
        triads[triadIndex].op = Triad::TEMP_TRIAD;
        for (auto ta : triads[triadIndex].args) {
            if (!ta.isPnToken) {
                markToRemove(ta.index);
            }
        }
    };

    std::function<bool(int)> triadHasFuncalls = [&] (int triadIndex) {
        if (triads[triadIndex].op == Triad::FUNCALL) {
            return true;
        }
        for (auto ta : triads[triadIndex].args) {
            if (!ta.isPnToken) {
                if (triadHasFuncalls(ta.index)) {
                    return true;
                }
            }
        }
        return false;
    };

    auto isVarUsedBetween = [&] (std::string var, int begin, int end) {
        assert(end >= begin);
        for (int i = begin; i <= end; i++) {
            for (int j = 0; j < triads[i].args.size(); j++) {
                auto ta = triads[i].args[j];
                if (triads[i].op == Triad::ASSIGN
                    && j == triads[i].args.size() - 1) {
                    // Если аргумент -- переменная, которой присваивают
                    continue;
                }
                if (ta.isPnToken) {
                    if (ta.token.text == var) {
                        return true;
                    }
                }
            }
        }
        return false;
    };

    for (auto block : blocks) {
        for (int i = block.first; i <= block.second; i++) {
            if (triads[i].op != Triad::ASSIGN) {
                continue;
            }
            if (triadHasFuncalls(i)) {
                continue;
            }
            // std::cout << "i is " << i << "\n";
            assert(triads[i].args.size() == 2);
            assert(triads[i].args[1].isPnToken);
            assert(triads[i].args[1].token.type == PnToken::VAR);
            for (int j = i+1; j <= block.second; j++) {
                if (triads[j].op != Triad::ASSIGN) {
                    continue;
                }
                // std::cout << "j is " << j << "\n";
                assert(triads[j].args.size() == 2);
                assert(triads[j].args[1].isPnToken);
                assert(triads[j].args[1].token.type == PnToken::VAR);
                if (triads[i].args[1].token.text ==
                    triads[j].args[1].token.text) {
                    // std::cout << "same variables\n";
                    // Если присвоение одной переменной
                    
                    // Проверяем, используется ли переменная начиная со следующей
                    // триады после первого присвоения, заканчивая вторым присвоением:
                    // переменная может использоваться в присвоении
                    if (!isVarUsedBetween(triads[i].args[1].token.text,
                                          i+1, j)) {
                        // std::cout << "mark to remove\n";
                        markToRemove(i);
                        if (triads[i+1].op == Triad::END_OF_LINE) {
                            // Удаление ; после присваивания
                            markToRemove(i+1);
                        }
                    }
                }
            }
        }
    }

    // Удаление помеченных триад и корректировка индексов триад
    for (int i = 0; i < triads.size(); i++) {
        if (triads[i].op == Triad::TEMP_TRIAD) {
            triads.erase(triads.begin() + i);
            // Начиная с триады, идущей после удаленной,
            // уменьшить на 1 ссылки на все триады, начиная с удаленной
            changeTriadIndexes(i, i, -1);
            i--;
        }
    }    

    return triads;
}

std::vector<std::pair<int,int>>
TriadOptimizer::getBlocks(std::vector<Triad> triads)
{
    // Возвращает пары индексов массива триад,
    // которые являются границами блоков, в которых
    // производятся определенные оптимизации

    // 1. Из функции берутся триады от FUNCTION_START до FUNCTION_END
    // 2. Вне функций берутся все триады, находящиеся вне
    //    [FUNCTION_DEFINITION, ..., FUNCTION_END]
    // 3. Полученные области разбиваются по FUNCALL
    //    (потенциально функции могут иметь побочные эффекты)
    // 4. Полученные области разбиваются по GOTO_DEST, GOTO, GOTO_IF_FALSE

    std::vector<std::pair<int, int>> functionBodies;

    for (int i = 0; i < triads.size(); i++) {
        if (triads[i].op == Triad::FUNCTION_START) {
            int j = i+1;
            assert(j < triads.size());
            while (triads[j].op != Triad::FUNCTION_END) {
                j++;
                assert(j < triads.size());
            }
            functionBodies.push_back({i+1, j-1});
            i = j;
        }
    }

    // Определение индексов блоков кода вне функций
    std::vector<std::pair<int, int>> plainCode;
    for (int i = 0; i < triads.size(); i++) {
        if (triads[i].op == Triad::FUNCTION_DEFINITION) {
            while (triads[i].op != Triad::FUNCTION_END) {
                assert(i < triads.size());
                i++;
            }
        }
        else {
            int j = i+1;
            while (j < triads.size() &&
                   triads[j].op != Triad::FUNCTION_DEFINITION) {
                j++;
            }
            // Если цикл закончился потому что j==triads.size(),
            //   то нужно уменьшить j на 1, чтобы он указывал на последний элемент
            // Если цикл закончился по второму условию, нужно уменьшить j,
            //    чтобы он указывал на последний элемент до начала функции
            j--;
            plainCode.push_back({i, j});
            i = j;
        }
    }

    std::vector<std::pair<int, int>> plainAndFunctions;
    for (auto p : functionBodies) {
        plainAndFunctions.push_back(p);
    }
    for (auto p : plainCode) {
        plainAndFunctions.push_back(p);
    }

    auto triadIsBorder = [] (Triad triad) {
        Triad::Operators op = triad.op;
        return op == Triad::GOTO_DEST
            || op == Triad::GOTO
            || op == Triad::GOTO_IF_FALSE
            || op == Triad::FUNCALL;
    };

    std::vector<std::pair<int, int>> res;
    for (auto p : plainAndFunctions) {
        assert(p.first >= 0 && p.first < triads.size());
        assert(p.second >= p.first && p.second < triads.size());
        for (int i = p.first; i <= p.second; i++) {
            if (!triadIsBorder(triads[i])) {
                int j = i;
                while (j <= p.second
                       && !triadIsBorder(triads[j])) {
                    j++;
                }
                if (j == triads.size() ||
                    triadIsBorder(triads[j])) {
                    j--;
                }
                assert(i <= j);
                res.push_back({i, j});
                i = j;
            }
        }
    }
    return res;
}

std::vector<PnToken> TriadOptimizer::generatePn(std::vector<Triad> triads)
{
    // Для каждой триады добавляем в полиз
    // аргументы этой триады, а после - оператор триады
    //
    // На каждую триаду не ссылаются больше одного раза.
    // (Если нужно использовать значение триады несколько раз,
    // то ее значение помещается в дополнительную переменную и используется уже она)

    // Проход идет с конца в начало:
    // Для каждой триады в начало полиза добавляется оператор,
    // после добавляются аргументы
    // Если аргументом является другая триада, то эта триада помечается
    // Если очередной триадой является помеченная триада, то она игнорируется

    // При этом в триадах для указания имени функции в FUNCALL
    // добавляется дополнительный аргумент, которого нет в полизе
    // Соответственно, после предыдущего  шага его нужно вернуть обратно в триаду

    // На первом этапе происходит генерация полиз-подобного массива триад
    // Потом для каждого номера GOTO_DEST определяется его абсолютная позиция
    // в полизе. Проход осуществляется с начала, и после запоминания позиции
    // GOTO_DEST эта триада удаляется. Это должно гарантировать, что позиции
    // всех GOTO_DEST определятся корректно
    // После этого значение литерала в каждом переходе заменяется с его номера
    // на разницу между положением оператора перехода и его назначением

    std::vector<Triad> pnTriads;

    std::set<int> markedTriads;

    std::function<void(int)> addTriadToPn =
        [&] (int index) {
            assert(index >= 0);
            assert(index < triads.size());
            assert(markedTriads.count(index) == 0);
            markedTriads.insert(index);
            Triad triad = triads[index];
            Triad opTriad(triad.op, {});
            pnTriads.insert(pnTriads.begin(), opTriad);
            for (auto ta : triad.args) {
                if (ta.isPnToken) {
                    Triad t(Triad::TEMP_TRIAD, {ta.token});
                    pnTriads.insert(pnTriads.begin(), t);
                }
                else {
                    addTriadToPn(ta.index);
                }
            }
        };
    for (int i = triads.size() - 1; i >= 0; i--) {
        if (!markedTriads.count(i)) {
            addTriadToPn(i);
        }
    }

    // Возвращение имени функции обратно в триаду FUNCALL
    // и в FUNCTION_START
    for (int i = 0; i < pnTriads.size()-1; i++) {
        if (pnTriads[i+1].op == Triad::FUNCALL) {
            assert(pnTriads[i].op == Triad::TEMP_TRIAD);
            assert(pnTriads[i].args.size() == 1);
            assert(pnTriads[i].args[0].token.type == PnToken::FUNCALL);
            assert(pnTriads[i+1].args.size() == 0);
            pnTriads[i+1].args.push_back(TriadArgument(pnTriads[i].args[0].token));
            pnTriads.erase(pnTriads.begin() + i);
            i--;
        }
        else if (pnTriads[i+1].op == Triad::FUNCTION_START) {
            assert(pnTriads[i].op == Triad::TEMP_TRIAD);
            assert(pnTriads[i].args.size() == 1);
            assert(pnTriads[i].args[0].token.type == PnToken::FUNCTION_START);
            assert(pnTriads[i+1].args.size() == 0);
            pnTriads[i+1].args.push_back(TriadArgument(pnTriads[i].args[0].token));
            pnTriads.erase(pnTriads.begin() + i);
            i--;
        }
    }
    
    // Вычисление абсолютных позиций переходов goto
    std::map<int,int> gotoPositions;
    for (int i = 0; i < pnTriads.size()-1; i++) {
        if (pnTriads[i+1].op == Triad::GOTO_DEST) {
            // GOTO_DEST разворачивается в 2 триады,
            // где первая - TEMP_TRIAD с номером перехода
            assert(pnTriads[i].op == Triad::TEMP_TRIAD);
            assert(pnTriads[i].args.size() == 1);
            assert(pnTriads[i].args[0].isPnToken);
            int gotoIndex = std::stoi(pnTriads[i].args[0].token.text);
            gotoPositions.insert({gotoIndex, i});
            pnTriads.erase(pnTriads.begin() + i); // удаление TEMP_TRIAD
            pnTriads.erase(pnTriads.begin() + i); // удаление GOTO_DEST
            i--;
        }
    }

    // Замена номеров переходов на относительный сдвиг
    for (int i = 0; i < pnTriads.size() - 1; i++) {
        if (pnTriads[i+1].op == Triad::GOTO ||
            pnTriads[i+1].op == Triad::GOTO_IF_FALSE) {
            assert(pnTriads[i].op == Triad::TEMP_TRIAD);
            assert(pnTriads[i].args.size() == 1);
            assert(pnTriads[i].args[0].isPnToken);
            assert(pnTriads[i].args[0].token.type == PnToken::LITERAL);
            int gotoIndex = std::stoi(pnTriads[i].args[0].token.text);
            int pos = i+1;
            int shift = gotoPositions[gotoIndex] - pos;
            pnTriads[i].args[0].token.text = std::to_string(shift);
        }
    }

    // Преобразование массива полиз-подобных триад в полиз
    std::vector<PnToken> res;
    for (auto triad : pnTriads) {
        if (triad.op == Triad::TEMP_TRIAD) {
            assert(triad.args.size() == 1);
            assert(triad.args[0].isPnToken);
            res.push_back(triad.args[0].token);
        }
        else {
            PnToken token = getPnTokenByTriad(triad);
            if (token.type == PnToken::FUNCALL) {
                assert(triad.args.size() == 1);
                assert(triad.args[0].isPnToken);
                assert(triad.args[0].token.type == PnToken::FUNCALL);
                token = triad.args[0].token;
            }
            else if (token.type == PnToken::FUNCTION_START) {
                assert(triad.args.size() == 1);
                assert(triad.args[0].isPnToken);
                assert(triad.args[0].token.type == PnToken::FUNCTION_START);
                token = triad.args[0].token;
            }
            res.push_back(token);
        }
    }

    return res;
}
