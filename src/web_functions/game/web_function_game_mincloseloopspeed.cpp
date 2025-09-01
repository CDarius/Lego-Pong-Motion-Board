#include "web_functions/game/web_function_game_mincloseloopspeed.hpp"

const char* WebFunctionGameMinCloseLoopSpeed::getName() const {
    return "game_min_close_loop_speed";
}

const char* WebFunctionGameMinCloseLoopSpeed::getTitle() const {
    return "Axis Min Close Loop Speed";
}

const char* WebFunctionGameMinCloseLoopSpeed::getDescription() const {
    return "Traverse the axis to at the minimum close loop speed. Use the left encoder to change the speed and press it to confirm.";
}

uint16_t WebFunctionGameMinCloseLoopSpeed::getPrerequisitesCount() const {
    return 1;
}

const char* WebFunctionGameMinCloseLoopSpeed::getPrerequisiteDescription(uint16_t index) const {
    switch (index)
    {
    case 0: return "Axis must be homed";    
    default: return nullptr;
    }
}

void WebFunctionGameMinCloseLoopSpeed::arePrerequisitesMet(bool* results) const {
    results[0] = _axis.referenced();
}

WebFunctionExecutionStatus WebFunctionGameMinCloseLoopSpeed::start() {
    WebFunction::start(); // Call the base class start to initialize failure description and IO board

    _status = WebFunctionExecutionStatus::InProgress;

    // Start the axis max speed measurement procedure asynchronously
    _taskRunner.runAsync([](void* context) {
        WebFunctionGameMinCloseLoopSpeed* self = static_cast<WebFunctionGameMinCloseLoopSpeed*>(context);

        // Create a cancel token for this operation
        CancelToken cancel_token;
        self->_cancelToken = &cancel_token;

        // Prepare the status
        bool changeSpeed = true;
        float direction = -1.0f;
        float speed = self->_closeLoopMinSpeed;
        self->_encoder.clearValue();
        int16_t encoderValue = self->_encoder.getValue();

        while (!cancel_token.isCancelled()) {            
            // Invert the axis when the limit is reached
            if (direction > 0) {
                float limit = self->_axis.getSwLimitPlus() - MIN_SPEED_SAFE_CLOSE_LOOP_OVERSHOOT_DISTANCE;
                if (self->_axis.angle() >= limit) {
                    direction = -1.0f;
                    changeSpeed = true;
                }
            } else {
                float limit = self->_axis.getSwLimitMinus() + MIN_SPEED_SAFE_CLOSE_LOOP_OVERSHOOT_DISTANCE;
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

                speed += 0.1f * (float)delta;
                if (speed < 1.0f) speed = 1.0f;
                if (speed > 25.0f) speed = 25.0f;

                changeSpeed = true;
            }

            // Apply the speed change
            if (changeSpeed) {
                changeSpeed = false;

                pbio_error_t err = self->_axis.run(speed * direction);
                if (err != PBIO_SUCCESS) {
                    self->_failureDescription = "Failed to change axis movement direction";
                    self->_status = WebFunctionExecutionStatus::Failed;
                    break;
                }

                self->_ioBoard.showText(String(speed, 1));
            }

            // Apply the new min speed if the user click the encoder
            if (self->_encoder.isButtonPressed()) {
                self->_closeLoopMinSpeed = speed;
                break;
            }
        }

        self->_axis.stop();
        self->_ioBoard.clearText();
        self->_cancelToken = nullptr;
        if (self->_status == WebFunctionExecutionStatus::InProgress) {
            self->_status = WebFunctionExecutionStatus::Done;
        }
    }, this);

    return _status;
}

void WebFunctionGameMinCloseLoopSpeed::stop() {
    if (_cancelToken) {
        _cancelToken->cancel();
    }
}
