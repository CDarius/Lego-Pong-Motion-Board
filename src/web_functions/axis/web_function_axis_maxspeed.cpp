#include "web_functions/axis/web_function_axis_maxspeed.hpp"

const char* WebFunctionAxisMaxSpeed::getName() const {
    return "axis_maxspeed";
}

const char* WebFunctionAxisMaxSpeed::getTitle() const {
    return "Measure Axis Max Speed";
}

const char* WebFunctionAxisMaxSpeed::getDescription() const {
    return "Run a couple of tests to measure the maximum speed of the axis";
}

uint16_t WebFunctionAxisMaxSpeed::getPrerequisitesCount() const {
    return 1;
}

const char* WebFunctionAxisMaxSpeed::getPrerequisiteDescription(uint16_t index) const {
    switch (index)
    {
    case 0: return "Axis must be homed";    
    default: return nullptr;
    }
}

void WebFunctionAxisMaxSpeed::arePrerequisitesMet(bool* results) const {
    results[0] = _axis.referenced();
}

WebFunctionExecutionStatus WebFunctionAxisMaxSpeed::start() {
    WebFunction::start(); // Call the base class start to initialize failure description and IO board

    _status = WebFunctionExecutionStatus::InProgress;

    // Start the axis max speed measurement procedure asynchronously
    _taskRunner.runAsync([](void* context) {
        WebFunctionAxisMaxSpeed* self = static_cast<WebFunctionAxisMaxSpeed*>(context);

        // Create a cancel token for this operation
        CancelToken cancel_token;
        self->_cancelToken = &cancel_token;
        float sw_limit_plus = self->_axis.getSwLimitPlus();
        float sw_limit_minus = self->_axis.getSwLimitMinus();
        float max_positive_speed[MAX_SPEED_TEST_NUM_ITERATIONS] = {0.0f};
        float max_negative_speed[MAX_SPEED_TEST_NUM_ITERATIONS] = {0.0f};
        float speed;

        for (int i = 0; i < MAX_SPEED_TEST_NUM_ITERATIONS; i++) {
            IF_CANCELLED(cancel_token, {
                self->_status = WebFunctionExecutionStatus::Done;
                self->_cancelToken = nullptr;
                return;
            });

            // ****************************************
            // Measure max speed in positive direction        
            // ****************************************

            // Move to sw limit minus
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
                Logger::instance().logE("Error during step 1 " + String(self->_axis.name()) + "-axis max speed measurement: " + String(pbio_error_str(err)));
                self->_failureDescription = "Failed to reach start position (sw limit -)";
                self->_status = WebFunctionExecutionStatus::Failed;
                self->_cancelToken = nullptr;
                return;
            }
            
            float stop_position = sw_limit_plus - 8.0; // Stop 8 studs before the end of the travel

            // Start the axis the maximum power
            self->_axis.dc(100);

            while (true) {
                IF_CANCELLED(cancel_token, {
                    self->_status = WebFunctionExecutionStatus::Done;
                    self->_cancelToken = nullptr;
                    return;
                });

                // Track the maximum speed
                speed = self->_axis.speed();
                if (speed > max_positive_speed[i]) {
                    max_positive_speed[i] = speed;
                }

                // Stop the axis before the end of the travel
                if (self->_axis.angle() >= stop_position) {
                    self->_axis.brake();
                    break;
                }

                delay(PBIO_CONFIG_SERVO_PERIOD_MS);
            }

            // ****************************************
            // Measure max speed in negative direction
            // ****************************************

            // Move to sw limit minus
            err = self->_axis.run_target(
                self->_axis.get_speed_limit(),
                sw_limit_plus,
                PBIO_ACTUATION_HOLD,
                true,
                &cancel_token);

            IF_CANCELLED(cancel_token, {
                self->_status = WebFunctionExecutionStatus::Done;
                self->_cancelToken = nullptr;
                return;
            });

            if (err != PBIO_SUCCESS) {
                Logger::instance().logE("Error during step 2 " + String(self->_axis.name()) + "-axis max speed measurement: " + String(pbio_error_str(err)));
                self->_failureDescription = "Failed to reach start position (sw limit +)";
                self->_status = WebFunctionExecutionStatus::Failed;
                self->_cancelToken = nullptr;
                return;
            }
            
            stop_position = sw_limit_minus + 8.0; // Stop 8 studs before the end of the travel

            // Start the axis the maximum power
            self->_axis.dc(-100);

            while (true) {
                IF_CANCELLED(cancel_token, {
                    self->_status = WebFunctionExecutionStatus::Done;
                    self->_cancelToken = nullptr;
                    return;
                });

                // Track the maximum speed
                speed = self->_axis.speed();
                if (speed < max_negative_speed[i]) {
                    max_negative_speed[i] = speed;
                }

                // Stop the axis before the end of the travel
                if (self->_axis.angle() <= stop_position) {
                    self->_axis.brake();
                    break;
                }

                delay(PBIO_CONFIG_SERVO_PERIOD_MS);
            }
        }

        // Find minimum of max_positive_speed and absolute minimum of max_negative_speed
        float min_positive = max_positive_speed[0];
        float min_negative = fabs(max_negative_speed[0]);
        for (int i = 1; i < MAX_SPEED_TEST_NUM_ITERATIONS; i++) {
            if (max_positive_speed[i] < min_positive) {
                min_positive = max_positive_speed[i];
            }
            if (fabs(max_negative_speed[i]) < min_negative) {
                min_negative = fabs(max_negative_speed[i]);
            }
        }

        // As a result take the 95% of the smaller of the two speeds
        float speed_limit = min(min_positive, min_negative);
        speed_limit = roundf(speed_limit * 10.0f) / 10.0f; // Round to one decimal place
        self->_axis.set_speed_limit(speed_limit);

        self->_status = WebFunctionExecutionStatus::Done;
        self->_cancelToken = nullptr;
    }, this);

    return _status;
}

void WebFunctionAxisMaxSpeed::stop() {
    if (_cancelToken) {
        _cancelToken->cancel();
    }
}
