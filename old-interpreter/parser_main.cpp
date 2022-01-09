#include <iostream>

#include "parser.h"


using namespace std;


int main()
{
    Parser p;
    Lexer l;
    std::string text = "\
def function(a, b) { \n\
    while (a < b) {\n\
        a = a + b;\n\
    }\n\
}\n\
function(1,3);\n\
c=5*2 + 8/3 - 5/2;\
";
    cout << "text is:\n" << text << "\n";
    std::vector<Token> tokens = l.getTokens(text);
    cout << "tokens are:\n";
    for (int i = 0; i < tokens.size(); i++) {
        cout << i << ": " << tokens[i] << "\n";
    }
    cout << "\n";

    int pos = p.getErrorPlace(tokens);
    if (pos == -1) {
        cout << "no errors\n";
    }
    else {
        cout << "error at token nuber " << pos << "\n";
    }
}
