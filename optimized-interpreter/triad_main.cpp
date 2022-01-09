#include <iostream>

#include "pn.h"
#include "triad_optimizer.h"

using namespace std;

int main()
{
    Lexer l;
    PnGenerator pn;
    TriadOptimizer to;
    string text;
    // оптимизация присвоений
    text = "\
a = 1;\n\
a = 2;\n\
b = a * 2;\n\
a = 3;";
    // оптимизация одинаковых операций и констант
    /*text = "                                  \
a = 3*10/2 + 1;\n\
b = a*3 + 5 - 1;\n\
c = a*3 + 5 + 1;\n\
a = a + 1;\n\
d = a*3 + 5;";*/
    // string text  = "a = b+2 + (b+2);";
    // string text = "a = b + 2 + (b+2) ;";
    // string text = "a = (b + c) * 2 + (b + c) * 2;";
    // string text = "print(4,5);";
//     string text = "\
// a = 2 * 3 - 1;\n\
// b = 3;\n\
// if (2 < 3) { c = b + c; }\n\
// else { b = c + b*2; }\n\
// p = pow(2, 2+3*3);\n\
// c = a + b;";
    cout << "text is: \n" << text << endl;
    std::vector<Token> tokens = l.getTokens(text);

    std::vector<PnToken> pnTokens = pn.generate(tokens);
    cout << "pn is:\n";
    for (int i = 0; i < pnTokens.size(); i++) {
        cout << i << ", " << pnTokens[i] << "\n";
    }
    cout << "\n";
    std::vector<Triad> triads = to.getTriads(pnTokens);
    cout << "triads are:\n";
    for (int i = 0; i < triads.size(); i++) {
        cout << i << ": " << triads[i] << "\n";
    }
    cout << "\n";
    triads = to.optimizeConstants(triads);
    cout << "optimized triads (constants) are:\n";
    for (int i = 0; i < triads.size(); i++) {
        cout << i << ": " << triads[i] << "\n";
    }
    cout << "\n";

    triads = to.optimizeAssigns(triads,
                                to.getBlocks(triads));
    cout << "optimized triads (assigns) are:\n";
    for (int i = 0; i < triads.size(); i++) {
        cout << i << ": " << triads[i] << "\n";
    }
    cout << "\n";

    cout << "blocks are:\n";
    for (auto b : to.getBlocks(triads)) {
        cout << b.first  << ", " << b.second << "\n";
    }
    triads = to.optimizeSameOperations(triads,
                                       to.getBlocks(triads));
    cout << "optimized triads (same operations) are:\n";
    for (int i = 0; i < triads.size(); i++) {
        cout << i << ": " << triads[i] << "\n";
    }
    cout << "\n";
    
    pnTokens = to.generatePn(triads);
    cout << "optimized pn is:\n";
    for (int i = 0; i < pnTokens.size(); i++) {
        cout << i << ", " << pnTokens[i] << "\n";
    }
    cout << "\n";
    
}
