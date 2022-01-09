#include "string_template.h"

std::string StringTemplate::getString(std::string temp,
                                      int index,
                                      std::string value)
{
    bool found;
    std::string str_to_replace = "#{" + std::to_string(index) + "}";
    do {
        found = false;
        for (int i = 0; i < (int)temp.size(); i++) {
            for (int j = 0; (j < (int)str_to_replace.size()
                             && j+i < (int)temp.size());
                 j++) {
                if (temp[i+j] != str_to_replace[j]) {
                    break;
                }
                if (j+1 == (int) str_to_replace.size()) {
                    // подстрока temp совпала с str_to_replace
                    found = true;
                    temp.erase(temp.begin() + i,
                               temp.begin() + i + j + 1);
                    temp.insert(temp.begin() + i,
                                value.begin(), value.end());
                }
            }
        }
    } while (found);

    return temp;
}
