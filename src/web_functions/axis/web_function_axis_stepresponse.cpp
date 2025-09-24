#include "web_functions/axis/web_function_axis_stepresponse.hpp"

const char* WebFunctionAxisStepResponse::getName() const {
    return "axis_stepresponse";
}

const char* WebFunctionAxisStepResponse::getTitle() const {
    return "Axis Step Response";
}

const char* WebFunctionAxisStepResponse::getDescription() const {
    return "Give a step command to the axis and log the response";
}

uint16_t WebFunctionAxisStepResponse::getPrerequisitesCount() const {
    return 1;
}

const char* WebFunctionAxisStepResponse::getPrerequisiteDescription(uint16_t index) const {
    switch (index)
    {
    case 0: return "Axis must be homed";    
    default: return nullptr;
    }
}

void WebFunctionAxisStepResponse::arePrerequisitesMet(bool* results) const {
    results[0] = _axis.referenced();
}

WebFunctionExecutionStatus WebFunctionAxisStepResponse::start() {
    WebFunction::start(); // Call the base class start to initialize failure description and IO board

    _status = WebFunctionExecutionStatus::InProgress;

    // Start the axis step log asynchronously
    _taskRunner.runAsync([](void* context) {
        WebFunctionAxisStepResponse* self = static_cast<WebFunctionAxisStepResponse*>(context);

        // Create a cancel token for this operation
        CancelToken cancel_token;
        self->_cancelToken = &cancel_token;

        float sw_limit_plus = self->_axis.getSwLimitPlus();
        float sw_limit_minus = self->_axis.getSwLimitMinus();

        // Move the axis at minus sw limit
        pbio_error_t err = self->_axis.run_target(
            self->_axis.get_speed_limit(),
            sw_limit_minus,
            PBIO_ACTUATION_HOLD,
            true,
            &cancel_token);

        IF_CANCELLED(cancel_token, {
            self->_status = WebFunctionExecutionStatus::Done;
            self->_cancelToken = nullptr;
            return;
        });

        if (err != PBIO_SUCCESS) {
            Logger::instance().logE("Error during step 1 " + String(self->_axis.name()) + "-axis step response: " + String(pbio_error_str(err)));
            self->_failureDescription = "Failed to reach start position (sw limit -)";
            self->_status = WebFunctionExecutionStatus::Failed;
            self->_cancelToken = nullptr;
            return;
        }

        // Let the axis settle and then kill the PWM command
        delay(1000); // Let the axis settle
        self->_axis.stop(); // Ensure the motor is stopped
        delay(20);

        // Start axis log and give a step move command towards plus sw limit
        pbio_logger_start(self->_axis.get_log(), RUN_STEP_RESPONSE_LOG_DURATION_MS, 1);
        delay(100); // Wait a bit to ensure the logger is started
        float target_position = sw_limit_plus - 4.0; // Stop 4 studs before the end of the travel
        self->_axis.track_target(target_position);
        
        // Wait for the axis to reach the target position
        float axis_speed_tolerance, axis_position_tolerance;
        self->_axis.get_target_tolerances(&axis_speed_tolerance, &axis_position_tolerance);
        unsigned long begin_on_target_time = 0;
        while(true) {
            IF_CANCELLED(cancel_token, {
                self->_status = WebFunctionExecutionStatus::Done;
                self->_cancelToken = nullptr;
                return;
            });

            bool on_target = fabs(self->_axis.angle() - target_position) < axis_position_tolerance &&
                fabs(self->_axis.speed()) < axis_speed_tolerance;

            if (on_target) {
                if (begin_on_target_time == 0) {
                    begin_on_target_time = millis();
                } else if ((millis() - begin_on_target_time) >= RUN_STEP_RESPONSE_SETTLE_TIME_MS) {
                    // Axis is on target for the required time
                    break;
                }
            } else {
                begin_on_target_time = 0;
            }

            delay(PBIO_CONFIG_SERVO_PERIOD_MS * 10);
        }

        // Stop the log and release the axis
        pbio_logger_stop(self->_axis.get_log());
        self->_axis.stop();

        self->_status = WebFunctionExecutionStatus::Done;
        self->_cancelToken = nullptr;
    }, this);

    return _status;
}

void WebFunctionAxisStepResponse::stop() {
    if (_cancelToken) {
        _cancelToken->cancel();
    }
}
