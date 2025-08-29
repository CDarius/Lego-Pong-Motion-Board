#include "web_functions/game/web_function_game_bounce.hpp"

const char* WebFunctionGameBounce::getName() const {
    return "game_bounce";
}

const char* WebFunctionGameBounce::getTitle() const {
    return "Game Bounce Overshoot";
}

const char* WebFunctionGameBounce::getDescription() const {
    return "Run multiple tests to measure the axis overshoot when motion direction is inverted";
}

uint16_t WebFunctionGameBounce::getPrerequisitesCount() const {
    return 1;
}

const char* WebFunctionGameBounce::getPrerequisiteDescription(uint16_t index) const {
    switch (index)
    {
    case 0: return "Axis must be homed";    
    default: return nullptr;
    }
}

void WebFunctionGameBounce::arePrerequisitesMet(bool* results) const {
    results[0] = _axis.referenced();
}

WebFunctionExecutionStatus WebFunctionGameBounce::start() {
    WebFunction::start(); // Call the base class start to initialize failure description and IO board

    _status = WebFunctionExecutionStatus::InProgress;

    // Start the axis max speed measurement procedure asynchronously
    _taskRunner.runAsync([](void* context) {
        WebFunctionGameBounce* self = static_cast<WebFunctionGameBounce*>(context);

        // Create a cancel token for this operation
        CancelToken cancel_token;
        self->_cancelToken = &cancel_token;

        // Get the speed steps to test
        const float* speeds;

        const float x_speeds[] = X_AXIS_BOUNCE_INVERSIONS_SPEEDS;
        const float y_speeds[] = Y_AXIS_BOUNCE_INVERSIONS_SPEEDS;

        // Set speeds and speeds_count based on axis name
        if (strcmp(self->_axis.name(), "X") == 0) {
            speeds = x_speeds;
            
        } else if (strcmp(self->_axis.name(), "Y") == 0) {
            speeds = y_speeds;
        } else {
            Logger::instance().logE("Invalid axis for axis bounce overshoot measure. Axis: " + String(self->_axis.name()));
            self->_failureDescription = "Invalid axis for axis bounce overshoot measure";
            self->_status = WebFunctionExecutionStatus::Failed;
            self->_cancelToken = nullptr;
            return;
        }

        const uint8_t speeds_count = sizeof(x_speeds) / sizeof(x_speeds[0]);

        // Run the bounce test for each speed
        for (uint8_t i = 0; i < speeds_count; i++) {
            // Run first in positive direction, then in negative
            float overshootPlus;
            float overshootMinus;

            for (uint8_t j = 0; j < 2; j++) {
                float direction = j == 0 ? 1.0 : -1.0; // First positive, then negative
                float overshoot;
                
                pbio_error_t err = self->_runBounceTest(direction * (float)speeds[i], overshoot, cancel_token);
                IF_CANCELLED(cancel_token, {
                    self->_status = WebFunctionExecutionStatus::Done;
                    self->_cancelToken = nullptr;
                    return;
                });
                if (err != PBIO_SUCCESS) {
                    Logger::instance().logE("Error during bounce test at speed " + String(speeds[i]) + ": " + String(pbio_error_str(err)));
                    self->_failureDescription = "Bounce test failed";
                    self->_status = WebFunctionExecutionStatus::Failed;
                    self->_cancelToken = nullptr;
                    return;
                }

                if (direction > 0.0) {
                    overshootPlus = overshoot;
                } else {
                    overshootMinus = overshoot;
                }
            }

            // Get the maximum overshoot
            float max_overshoot = fmaxf(overshootPlus, overshootMinus);
            self->_bounceInversionOvershootAtSpeed[i] = max_overshoot;
        }

        self->_status = WebFunctionExecutionStatus::Done;
        self->_cancelToken = nullptr;
    }, this);

    return _status;
}

void WebFunctionGameBounce::stop() {
    if (_cancelToken) {
        _cancelToken->cancel();
    }
}

pbio_error_t WebFunctionGameBounce::_runBounceTest(uint8_t speedCmdPercentage, float& overshoot, CancelToken& cancel_token) {
    float startPosition = speedCmdPercentage > 0 ? _axis.getSwLimitMinus() : _axis.getSwLimitPlus();
    float inversionPosition = speedCmdPercentage > 0 ? _axis.getSwLimitPlus() - BOUNCE_SAFE_OVERSHOOT_DISTANCE : _axis.getSwLimitMinus() + BOUNCE_SAFE_OVERSHOOT_DISTANCE;
    
    // Move to start position
    pbio_error_t err = _axis.run_target(
        _axis.get_speed_limit(),
        startPosition,
        PBIO_ACTUATION_HOLD,
        true,
        &cancel_token);

    if (err != PBIO_SUCCESS) {
        return err;
    }

    // Begin to move open loop at the given speed
    _axis.dc(speedCmdPercentage);

    // Wait for the axis to reach the inversion position
    if (speedCmdPercentage > 0) {
        while (_axis.angle() < inversionPosition) {
            IF_CANCELLED(cancel_token, {
                return PBIO_ERROR_CANCELED;
            });

            delay(1);
        }
    } else {
        while (_axis.angle() > inversionPosition) {
            IF_CANCELLED(cancel_token, {
                return PBIO_ERROR_CANCELED;
            });

            delay(1);
        }
    }

    // Invert the motion direction
    _axis.dc(-speedCmdPercentage);

    // Keep track of the axis position until the axis invert the speed
    if (speedCmdPercentage > 0) {
        while (_axis.speed() > 0) {
            IF_CANCELLED(cancel_token, {
                return PBIO_ERROR_CANCELED;
            });

            delay(1);
        }
    } else {
        while (_axis.speed() < 0) {
            IF_CANCELLED(cancel_token, {
                return PBIO_ERROR_CANCELED;
            });

            delay(1);
        }
    }

    float overshootPosition = _axis.angle();
    overshoot = fabs(overshootPosition - inversionPosition);
    
    _axis.brake();

    return PBIO_SUCCESS;
}