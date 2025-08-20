#include "web_functions/axis/web_function_axis_set_zero.hpp"

const char* WebFunctionAxisSetZero::getName() const {
    return "axis_set_zero";
}

const char* WebFunctionAxisSetZero::getTitle() const {
    return "Axis set zero";
}

const char* WebFunctionAxisSetZero::getDescription() const {
    return "Set the axis to zero position at current location";
}

uint16_t WebFunctionAxisSetZero::getPrerequisitesCount() const {
    return 1;
}

const char* WebFunctionAxisSetZero::getPrerequisiteDescription(uint16_t index) const {
    switch (index)
    {
    case 0: return "Axis must be homed";    
    default: return nullptr;
    }
}

void WebFunctionAxisSetZero::arePrerequisitesMet(bool* results) const {
    results[0] = _axis.referenced();
}

WebFunctionExecutionStatus WebFunctionAxisSetZero::start() {
    WebFunction::start(); // Call the base class start to initialize failure description and IO board

    _axis.axis_position_at_home_marker = _axis.axis_position_at_home_marker - _axis.angle();
    return _status;
}

void WebFunctionAxisSetZero::stop() {
    // Nothing to do here
}
