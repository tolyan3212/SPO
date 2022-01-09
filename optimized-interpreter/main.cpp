#include <iostream>

#include "executor.h"
#include "pn.h"
#include "parser.h"
#include "lexer.h"

#include "triad_optimizer.h"

using namespace std;


int main()
{
    Executor e;
    PnGenerator generator;
    Parser p;
    Lexer l;
    TriadOptimizer to;

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
            int i = 0;
            for (auto t : tokens) {
                cout << i << ": " << t << "\n";
                i++;
            }
            auto pn = generator.generate(tokens);

            cout << "pn is: \n";
            i = 0;
            for (auto p : pn) {
                cout << i << ": " << p << "\n";
                i++;
            }

            auto triads = to.getTriads(pn);
            for (int i = 0; i < triads.size(); i++) {
                cout << i << ": " << triads[i] << "\n";
            }
            cout << "\noptimized constants:\n";
            triads = to.optimizeConstants(triads);
            for (int i = 0; i < triads.size(); i++) {
                cout << i << ": " << triads[i] << "\n";
            }
            cout << "\n";
            triads = to.optimizeAssigns(triads,
                                        to.getBlocks(triads));
            cout << "optimized assigns:\n";
            for (int i = 0; i < triads.size(); i++) {
                cout << i << ": " << triads[i] << "\n";
            }            cout << "\n";
            triads = to.optimizeSameOperations(triads,
                                               to.getBlocks(triads));
            cout << "optimized same operations:\n";
            for (int i = 0; i < triads.size(); i++) {
                cout << i << ": " << triads[i] << "\n";
            }
            cout << "\n";
            
            pn = to.generatePn(triads);
            cout << "optimized:\n";
            for (auto p : pn) {
                cout << p << "\n";
            }

            
            e.execute(pn);
        }
        catch (exception& e) {
            cout << "Caught exception: " << e.what() << "\n";
        }
    }
}
