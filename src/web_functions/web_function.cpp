#include "web_functions/web_function.hpp"

WebFunctionExecutionStatus WebFunction::start() {
    _failureDescription = nullptr;
    _ioBoard.clearText();

    if (!areAllPrerequisitesMet()) {
        _failureDescription = "Prerequisites not met for this function";
        _status = WebFunctionExecutionStatus::Failed;
        return _status;
    }

    _ioBoard.playSound(IO_BOARD_SOUND_BEEP);
    
    return _status;
}

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