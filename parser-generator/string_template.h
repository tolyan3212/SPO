#ifndef STRING_TEMPLATE_H
#define STRING_TEMPLATE_H


#include <string>

class StringTemplate
{
public:
    StringTemplate() = delete;

    static std::string getString(std::string temp,
                                 int index,
                                 std::string value);
};


#endif  // STRING_TEMPLATE_H
