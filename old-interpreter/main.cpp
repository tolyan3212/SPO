#include <iostream>

#include "executor.h"
#include "pn.h"
#include "parser.h"
#include "lexer.h"

using namespace std;


int main()
{
    Executor e;
    PnGenerator generator;
    Parser p;
    Lexer l;

    string s;

    while (true) {
        cout << "input code:\n";
        string newLine = "";
        getline(cin, newLine);
        s = newLine;
        while (newLine != "") {
            getline(cin, newLine);
            s += newLine;
        }
        cout << "executing...\n";
        try {
            auto tokens = l.getTokens(s);    
            auto pn = generator.generate(tokens);

            e.execute(pn);
        }
        catch (exception& e) {
            cout << "Caught exception: " << e.what() << "\n";
        }
    }
}
