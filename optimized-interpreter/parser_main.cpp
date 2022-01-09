#include <iostream>
#include <fstream>
#include <list>

#include "lexer.h"
#include "parser.h"

#include "tree_optimizer.h"

using namespace std;

void printTree(ParserTree* tree);

std::string readFile1(std::string filename) {
    std::ifstream f(filename);
    std::stringstream buf;
    buf << f.rdbuf();
    std::string str = buf.str();
    f.close();
    return str;
}

int main()
{
    Lexer l;
    Parser p;

    string text = "\
a = 2;\n\
b = 3;\n\
if (2 < 3) { c = b + c; }\n\
if (1 < 2) {\n\
    while (2 < 3) { \n\
        if (3 < 4) {}\n\
    }\n\
}\n\
while (1 < 2) {}\n\
do {} while (1 < 10);\n\
c = a + b;";
    // string text = "a = (b+2) + f((b+2));";
//     std::string text = "\
// def function(a, b) { \n\
//     while (a < b) {\n\
//     }\n\
// }";
// "def function(a, b) { \n\
//     while (a < b) {\n\
//         a = a + b;\n\
//     }\n\
// }\n\
// function(1,3);\n\
// c=5*2 + 8/3 - 5/2;\
// ";
    cout << "text is:\n" << text << "\n";
    auto tokens = l.getTokens(text);

    for (int i = 0; i < tokens.size(); i++) {
        cout << i << ": " << tokens[i] << "\n";
    }
    cout << "\n";

    ParserTree* tree(p.parse(tokens));

    std::cout << "generated\n";
    printTree(tree);
    std::cout << "\n";

    std::vector<TreeOptimizer*> optimizers({
            new OptimizerExpression(),
            new OptimizerIf(),
            new OptimizerWhile(),
            new OptimizerDoWhile()
        });
    bool wasOptimized = false;
    do {
        wasOptimized = false;
        for (auto o : optimizers) {
            if (o->optimize(tree)) {
                wasOptimized = true;
            }
        }
    } while (wasOptimized);
    std::cout << "optimized\n";
    printTree(tree);
    std::cout << "\n";
    // tree.get();
    delete tree;
    for (auto o : optimizers) {
        delete o;
    }
    // tokens = tree.get()->getAllTokens();
    // for (int i = 0; i < tokens.size(); i++) {
    //     cout << i << ": " << tokens[i] << "\n";
    // }
    cout << "\n";
    cout << "end\n";
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

