#include <iostream>
#include <regex>
#include <string>

#include "lexer.h"

using namespace std;


int main()
{
    Lexer l;
    string s = "a = b +c; if (1-3>=3) { 4-8*2; } else {while (a < b) a = a + b;}";
    cout << "string is:\n";
    cout << s << "\n";
    cout << "tokens are:\n";
    for (Token t : l.getTokens(s)) {
        cout << t << ",";
    }
    cout << "\n";
    return 0;
}
