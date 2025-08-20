#include "web_functions/axis/web_function_axis_homing.hpp"

const char* WebFunctionAxisHoming::getName() const {
    return "axis_homing";
}

const char* WebFunctionAxisHoming::getTitle() const {
    return "Axis Homing";
}

const char* WebFunctionAxisHoming::getDescription() const {
    return "Perform the axis homing procedure";
}

uint16_t WebFunctionAxisHoming::getPrerequisitesCount() const {
    return 0; // No prerequisites
}

const char* WebFunctionAxisHoming::getPrerequisiteDescription(uint16_t index) const {
    return nullptr;
}

void WebFunctionAxisHoming::arePrerequisitesMet(bool* results) const {
    // No prerequisites, so nothing to check
}

WebFunctionExecutionStatus WebFunctionAxisHoming::start() {
    _failureDescription = nullptr;

    if (!areAllPrerequisitesMet()) {
        _failureDescription = "Prerequisites not met for axis homing.";
        _status = WebFunctionExecutionStatus::Failed;
        return _status;
    }

    _status = WebFunctionExecutionStatus::InProgress;

    // Start the axis homing procedure asynchronously
    _taskRunner.runAsync([](void* context) {
        WebFunctionAxisHoming* self = static_cast<WebFunctionAxisHoming*>(context);

        // Create a cancel token for this operation
        CancelToken cancel_token;
        self->_cancelToken = &cancel_token;

        // Run the homing
        pbio_error_t err = self->_axis.run_axis_homing(cancel_token);
        if (err != PBIO_SUCCESS) {
            Logger::instance().logE("Error during (web) " + String(self->_axis.name()) + "-axis homing: " + String(pbio_error_str(err)));
            self->_failureDescription = "Procedure failed. See logs.";
            self->_status = WebFunctionExecutionStatus::Failed;
        } else {
            self->_status = WebFunctionExecutionStatus::Done;
        }

        self->_cancelToken = nullptr;
    }, this);

    return _status;
}

void WebFunctionAxisHoming::stop() {
    if (_cancelToken) {
        _cancelToken->cancel();
        _cancelToken = nullptr;
    }
}
