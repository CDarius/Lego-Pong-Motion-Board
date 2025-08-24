#include "web_functions/axes/web_function_axes_jog.hpp"

const char* WebFunctionAxesJog::getName() const {
    return "jog";
}

const char* WebFunctionAxesJog::getTitle() const {
    return "Jog axes";
}

const char* WebFunctionAxesJog::getDescription() const {
    return "Jog all axes. Press the paddle to change axis.";
}

uint16_t WebFunctionAxesJog::getPrerequisitesCount() const {
    return 0; // No prerequisites
}

const char* WebFunctionAxesJog::getPrerequisiteDescription(uint16_t index) const {
    return nullptr;
}

void WebFunctionAxesJog::arePrerequisitesMet(bool* results) const {
    // No prerequisites, so nothing to check
}

void WebFunctionAxesJog::startJog(int axisIndex) {
    Axes axis = ALL_AXES[axisIndex];
    _encoderJog.start(axis);
    _ioBoard.showText("Jog:" + String(_encoderJog.getMotor()->name()));
}

WebFunctionExecutionStatus WebFunctionAxesJog::start() {
    WebFunction::start(); // Call the base class start to initialize failure description and IO board

    _status = WebFunctionExecutionStatus::InProgress;

    // Start the axis jog asynchronously
    _taskRunner.runAsync([](void* context) {
        WebFunctionAxesJog* self = static_cast<WebFunctionAxesJog*>(context);

        // Create a cancel token for this operation
        CancelToken cancel_token;
        self->_cancelToken = &cancel_token;

        int axisIndex = 0;
        self->startJog(axisIndex);

        UnitEncoder* encoder = self->_encoderJog.getEncoder();
        Button encoderButton;

        // Jog loop until the user requests to stop
        while(!self->_cancelToken->isCancelled()) {
            self->_encoderJog.update();

            // Switch axis on paddle press
            uint32_t time = (uint32_t)(millis());
            encoderButton.setRawState(time, encoder->getButtonStatus());
            if (encoderButton.wasPressed()) {
                axisIndex = (axisIndex + 1) % NUM_AXES;
                self->startJog(axisIndex);
                self->_ioBoard.playSound(IO_BOARD_SOUND_BEEP);
            }
        }
        self->_status = WebFunctionExecutionStatus::Done;

        self->_cancelToken = nullptr;
    }, this);

    return _status;
}

void WebFunctionAxesJog::stop() {
    if (_cancelToken) {
        _cancelToken->cancel();
    }
}
