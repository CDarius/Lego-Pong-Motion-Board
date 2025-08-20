#include "web_functions/web_function.hpp"

bool WebFunction::areAllPrerequisitesMet() const {
    uint16_t count = getPrerequisitesCount();
    if (count == 0)
        return true;
    
    bool results[count];
    arePrerequisitesMet(results);
    for (uint16_t i = 0; i < getPrerequisitesCount(); i++) {
        if (!results[i]) {
            return false;
        }
    }
    return true;
}