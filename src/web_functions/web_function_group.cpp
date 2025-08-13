#include "web_functions/web_function_group.hpp"

WebFunction* WebFunctionGroup::getFunction(const char* name) {
    WebFunction** functions = getFunctions();
    uint16_t count = getFunctionsCount();

    for (uint16_t i = 0; i < count; i++) {
        if (strcmp(functions[i]->getName(), name) == 0) {
            return functions[i];
        }
    }

    return nullptr;
}