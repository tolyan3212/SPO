#ifndef GENERATOR_H
#define GENERATOR_H

#include <string>
#include <map>

#include "parser.h"

class ParserGenerator
{
public:
    ParserGenerator();
    void generateCode(ParserTree* tree);

    // std::string getHeaderText();
    // std::string getSourceText();

// private:
    std::string lexerHeader;
    std::string lexerSource;
    std::string parserHeader;
    std::string parserSource;

    int currentExpressionNumber;

    int getCurrentExpressionNumber();

    void generateLexer();
    void generateParser();

    std::string getMethodCode(Unit* rule);

    // методы возвращают пары {код, выражение_для_проверки_успешности}
    std::pair<std::string, std::string> getExpressionCode(Unit& unit);
    std::pair<std::string, std::string> getExpressionCode(ParserTree* expression);
    std::pair<std::string, std::string> getTokenCode(ParserTree* e);
    std::pair<std::string, std::string> getTokenCode(Unit &u);
    std::pair<std::string, std::string> getTreeCode(ParserTree* e);
    std::pair<std::string, std::string> getTreeCode(Unit &u);
    std::pair<std::string, std::string> getAndCode(ParserTree* e);
    std::pair<std::string, std::string> getOrCode(ParserTree* e);
    std::pair<std::string, std::string> getGroupCode(ParserTree* e);
    std::pair<std::string, std::string> getZeroOrOneCode(ParserTree* e);
    std::pair<std::string, std::string> getZeroOrMoreCode(ParserTree* e);
    
    

    // Имена типов токенов и их регулярные выражения
    std::map<std::string, std::string> tokens;
    // Имена типов токенов для последовательной их нумерации
    std::vector<std::string> tokensArray;
    // Имена типов лексем, определяющихся через токены и другие лексемы
    std::map<std::string, Unit*> lexems;
    std::vector<std::string> lexemsArray;

    std::string lexerHeaderTemplate;
    std::string lexerSourceTemplate;
    std::string parserHeaderTemplate;
    std::string parserSourceTemplate;
};


#endif  // GENERATOR_H
