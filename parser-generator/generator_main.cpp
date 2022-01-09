#include <iostream>
#include <fstream>
#include <list>

#include "parser.h"
#include "generator.h"

using namespace std;

std::string readFile1(std::string filename) {
    std::ifstream f(filename);
    std::stringstream buf;
    buf << f.rdbuf();
    std::string str = buf.str();
    f.close();
    return str;
}

void writeFile(std::string filename,
               std::string text)
{
    ofstream f(filename);
    f << text;
    f.close();
}

void printTree(ParserTree *tree)
{
    list<ParserTree*> queue;
    queue.push_back(tree);
    int number = 0;
    int count = 1;
    while (queue.size()) {
        auto t = queue.front();
        queue.pop_front();
        cout << number << ": type=" << t->type
             << ", {";
        for (Unit &u : t->units) {
            if (u.isToken()) {
                cout << u.getToken();
            }
            else {
                cout << "^" << count;
                count++;
                queue.push_back(u.getTree());
            }
            cout << ",";
        }
        cout << "}\n";
        number++;
    }
}

int main()
{
    string text = readFile1("grammar_test.txt");

    Lexer l;
    Parser p;
    ParserGenerator pg;

    auto tokens = l.getTokens(text);
    cout << "tokens:\n";
    int c = 0;
    for (auto t : tokens) {
        cout << c << ", " << t << "\n";
        c++;
    }
    cout << "\n";
    shared_ptr<ParserTree> tree(p.parse(tokens));

    printTree(tree.get());
    std::cout << "\n";
    pg.generateCode(tree.get());

    // cout << "lexer header:\n\n";
    // cout << pg.lexerHeader;
    // cout << "\n\n\nlexer source:\n\n";
    // cout << pg.lexerSource;
    // cout << "\n";

    // cout << "parser header:\n\n";
    // cout << pg.parserHeader;
    // cout << "\n\n\nparser source:\n\n";
    // cout << pg.parserSource;
    // cout << "\n";

    writeFile("target/lexer.h", pg.lexerHeader);
    writeFile("target/lexer.cpp", pg.lexerSource);
    writeFile("target/parser.h", pg.parserHeader);
    writeFile("target/parser.cpp", pg.parserSource);
    
}
