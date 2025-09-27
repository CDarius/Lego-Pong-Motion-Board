#include "web_functions/axes/web_function_axes_homeall.hpp"

const char* WebFunctionAxesHomeAll::getName() const {
    return "home_all";
}

const char* WebFunctionAxesHomeAll::getTitle() const {
    return "Home all axes";
}

const char* WebFunctionAxesHomeAll::getDescription() const {
    return "Home all axes. Use the paddle to move the ball out of the way and press the start button to begin.";
}

uint16_t WebFunctionAxesHomeAll::getPrerequisitesCount() const {
    return 0; // No prerequisites
}

const char* WebFunctionAxesHomeAll::getPrerequisiteDescription(uint16_t index) const {
    return nullptr;
}

void WebFunctionAxesHomeAll::arePrerequisitesMet(bool* results) const {
    // No prerequisites, so nothing to check
}

WebFunctionExecutionStatus WebFunctionAxesHomeAll::start() {
    WebFunction::start(); // Call the base class start to initialize failure description and IO board

    _status = WebFunctionExecutionStatus::InProgress;

    // Start axes homing asynchronously
    _taskRunner.runAsync([](void* context) {
        WebFunctionAxesHomeAll* self = static_cast<WebFunctionAxesHomeAll*>(context);

        // Create a cancel token for this operation
        CancelToken cancel_token;
        self->_cancelToken = &cancel_token;

        pbio_error_t err = homeAllAxes(
            self->_xMotor, self->_yMotor, 
            self->_lMotor, self->_rMotor,
            self->_ioBoard,
            self->_lEncoderJog, self->_rEncoderJog,
            cancel_token);
        if (err != PBIO_SUCCESS && err != PBIO_ERROR_CANCELED) {
            Logger::instance().logE("Error during (web) all axes homing: " + String(pbio_error_str(err)));
            self->_failureDescription = "Procedure failed. See logs.";
            self->_status = WebFunctionExecutionStatus::Failed;
        } else {
            self->_status = WebFunctionExecutionStatus::Done;
        }

        self->_cancelToken = nullptr;
    }, this);

    return _status;
}

void WebFunctionAxesHomeAll::stop() {
    if (_cancelToken) {
        _cancelToken->cancel();
    }
}
