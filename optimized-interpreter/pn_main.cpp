#include <iostream>

#include "pn.h"
#include "parser.h"

using namespace std;

void printTree(ParserTree *tree)
{
    list<ParserTree*> queue;
    queue.push_back(tree);
    int number = 0;
    int count = 1;
    std::cout << "aa\n";
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
    PnGenerator pn;

    string s = "\
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
//     string s = "\
// a = 5;\n\
// b = 6;\n\
// while (a < b + 2) {\n\
//     while (c < 2) {\n\
//     }\n\
//     a = b;\n\
// }\n\
// l = list();";
    Lexer l;

    cout << "text is:\n";
    cout << s << "\n";

    auto tokens = l.getTokens(s);
    Parser p;
    shared_ptr<ParserTree> tree(p.parse(tokens));
    printTree(tree.get());

    auto v = pn.generate(tokens);

    cout << "PN is:\n";
    for (int i = 0; i < v.size(); i++) {
        cout << i << ":" << v[i] << "\n";
    }
    cout << "\n";
}
