#include "motor_control/motorwithreferenceswitch.hpp"

pbio_error_t MotorWithReferenceSwitch::run_axis_homing(CancelToken& cancel_token) { 
    bool canRetry = true;
    float start_position = angle();

    float forward_speed = _config.start_in_positive_direction ? _config.speed : -_config.speed;
    float backward_speed = -2.0 * forward_speed;
    float backward_final_speed = get_speed_limit() * (forward_speed > 0 ? -1 : 1);

    Logger::instance().logI("Starting " + String(name()) + "-axis homing....");

    while (true)
    {
        IF_CANCELLED(cancel_token, {
            // If the homing is canceled, stop the motor
            stop();
            Logger::instance().logI("Homing canceled by the user");
            return PBIO_ERROR_CANCELED;
        });

        // Start running toward the home swtich    
        PBIO_RETURN_ON_ERROR(run(forward_speed));

        // Wait for the home switch to be pressed
        while (digitalRead(_config.home_switch_pin) != _config.switch_pressed_value) {
            delay(PBIO_CONFIG_SERVO_PERIOD_MS);
            IF_CANCELLED(cancel_token, {
                // If the homing is canceled, stop the motor
                stop();
                Logger::instance().logI("Homing canceled by the user");
                return PBIO_ERROR_CANCELED;
            });
        }

        // The switch is pressed
        float hit_position = angle();
        float travel_distance = abs(hit_position - start_position);
        if (travel_distance >= _config.minimum_travel) {
            // We hit the switch after moving the minimum distance
            reset_angle(_config.axis_position_at_home_marker);
            Logger::instance().logI(name() + String("-axis hit home switch"));
            break;
        }
        else {
            if (!canRetry) {                
                // We hit the switch too early and we already tried to retract
                stop();
                Logger::instance().logE(name() + String("-axis hit home switch too early after retrying. Aborting homing"));
                return PBIO_ERROR_HOME_SWITCH_ERR;
            }

            // Retract and try again
            canRetry = false; // Prevent further retries
            Logger::instance().logI(name() + String("-axis hit home switch too early. Retract and try again"));
            PBIO_RETURN_ON_ERROR(run_angle(backward_speed, _config.minimum_travel * 1.2, PBIO_ACTUATION_HOLD, true, &cancel_token));
            start_position = angle();
            // Test if the switch is released
            if (digitalRead(_config.home_switch_pin) == _config.switch_pressed_value) {
                // The switch should not be pressed after retracting
                Logger::instance().logE("Home switch still pressed after retracting");
                return PBIO_ERROR_HOME_SWITCH_ERR;
            }
        }
    }

    IF_CANCELLED(cancel_token, {
        // If the homing is canceled, stop the motor
        stop();
        Logger::instance().logI("Homing canceled by the user");
        return PBIO_ERROR_CANCELED;
    });

    // Move away from the switch to the final position
    PBIO_RETURN_ON_ERROR(run_target(backward_final_speed, _config.axis_position_after_home, PBIO_ACTUATION_COAST, true, &cancel_token));

    // Test if the switch is released
    if (digitalRead(_config.home_switch_pin) == _config.switch_pressed_value) {
        // The switch should not be pressed after retracting
        Logger::instance().logE("Home switch still pressed after retracting");
        return PBIO_ERROR_HOME_SWITCH_ERR;
    }

    _referenced = true;
    
    return PBIO_SUCCESS;
}