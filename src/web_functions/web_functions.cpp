#include "web_functions/web_functions.hpp"

WebFunctionGroup* WebFunctions::getGroup(const char* name) {
    for(uint16_t i=0; i < _groupsCount; i++) {
        WebFunctionGroup* group = _groups[i];
        if (strcmp(name, group->getName()) == 0)
        {
            return group;
        }
    }

    return nullptr;
}

WebFunctionGroup** WebFunctions::getGroups() {
    return _groups;
}

uint16_t WebFunctions::getGroupsCount() const {
    return _groupsCount;
}

bool WebFunctions::checkAnyFunctionInProgress() const {
    for (uint16_t i = 0; i < _groupsCount; ++i) {
        WebFunctionGroup* group = _groups[i];
        uint16_t funcCount = group->getFunctionsCount();
        WebFunction** functions = group->getFunctions();
        for (uint16_t j = 0; j < funcCount; ++j) {
            if (functions[j]->getRunningStatus() == WebFunctionExecutionStatus::InProgress) {
                return true;
            }
        }
    }
    return false;
}