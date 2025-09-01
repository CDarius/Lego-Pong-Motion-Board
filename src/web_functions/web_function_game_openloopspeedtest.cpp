#include "web_functions/game/web_function_game_openloopspeedtest.hpp"

const char* WebFunctionGameOpenLoopSpeedTest::getName() const {
    return "game_open_loop_speed_test";
}

const char* WebFunctionGameOpenLoopSpeedTest::getTitle() const {
    return "Axis Open Loop Speed Test";
}

const char* WebFunctionGameOpenLoopSpeedTest::getDescription() const {
    return "Traverse the axis in open loop. Use the left encoder to change the speed. This function is only for testing purposes, no settings are changed.";
}

uint16_t WebFunctionGameOpenLoopSpeedTest::getPrerequisitesCount() const {
    return 1;
}

const char* WebFunctionGameOpenLoopSpeedTest::getPrerequisiteDescription(uint16_t index) const {
    switch (index)
    {
    case 0: return "Axis must be homed";    
    default: return nullptr;
    }
}

void WebFunctionGameOpenLoopSpeedTest::arePrerequisitesMet(bool* results) const {
    results[0] = _axis.referenced();
}

WebFunctionExecutionStatus WebFunctionGameOpenLoopSpeedTest::start() {
    WebFunction::start(); // Call the base class start to initialize failure description and IO board

    _status = WebFunctionExecutionStatus::InProgress;

    // Start the axis max speed measurement procedure asynchronously
    _taskRunner.runAsync([](void* context) {
        WebFunctionGameOpenLoopSpeedTest* self = static_cast<WebFunctionGameOpenLoopSpeedTest*>(context);

        // Create a cancel token for this operation
        CancelToken cancel_token;
        self->_cancelToken = &cancel_token;

        // Prepare the status
        bool changeSpeed = true;
        float direction = -1.0f;
        float speed = 50.0f;
        self->_encoder.clearValue();
        int16_t encoderValue = self->_encoder.getValue();

        while (!cancel_token.isCancelled()) {            
            // Invert the axis when the limit is reached
            if (direction > 0) {
                float limit = self->_axis.getSwLimitPlus() - MIN_SPEED_SAFE_OPEN_LOOPOVERSHOOT_DISTANCE;
                if (self->_axis.angle() >= limit) {
                    direction = -1.0f;
                    changeSpeed = true;
                }
            } else {
                float limit = self->_axis.getSwLimitMinus() + MIN_SPEED_SAFE_OPEN_LOOPOVERSHOOT_DISTANCE;
                if (self->_axis.angle() <= limit) {
                    direction = 1.0f;
                    changeSpeed = true;
                }
            }
            
            // Read the encoder to get user speed changes requests
            int16_t newEncoderValue = self->_encoder.getValue();
            if (newEncoderValue != encoderValue) {
                int16_t delta = newEncoderValue - encoderValue;
                encoderValue = newEncoderValue;

                speed += (float)delta;
                if (speed < -100.0f) speed = -100.0f;
                if (speed > 100.0f) speed = 100.0f;

                changeSpeed = true;
            }

            // Apply the speed change
            if (changeSpeed) {
                changeSpeed = false;

                self->_axis.dc(speed * direction);
                self->_ioBoard.showText(String((uint8_t)speed)+ "%");
            }
        }

        self->_axis.stop();
        self->_ioBoard.clearText();
        self->_cancelToken = nullptr;
        self->_status = WebFunctionExecutionStatus::Done;
    }, this);

    return _status;
}

void WebFunctionGameOpenLoopSpeedTest::stop() {
    if (_cancelToken) {
        _cancelToken->cancel();
    }
}
