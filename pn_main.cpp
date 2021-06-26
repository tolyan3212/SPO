#include <iostream>

#include "pn.h"


using namespace std;


int main()
{
    PnGenerator pn;

    string s = "\
a = 5;\
b = 6;\
while (a < b + 2) {\
    while (c < 2) {a = a + 1;} \
    a = b;\
}";
    Lexer l;

    cout << "text is:\n";
    cout << s << "\n";

    auto tokens = l.getTokens(s);

    auto v = pn.generate(tokens);

    cout << "PN is:\n";
    for (int i = 0; i < v.size(); i++) {
        cout << i << ":" << v[i] << " ";
    }
    cout << "\n";
}
