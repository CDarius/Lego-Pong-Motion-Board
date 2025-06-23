#include "motor_control/motorhoming.hpp"

pbio_error_t home_with_switch(Motor& motor, uint8_t home_switch_pin, int switch_pressed_value, switch_homing_config_t& config) {
    CancelToken cancel_token; 
    bool canRetry = true;
    float start_position = motor.angle();

    float forward_speed = config.start_in_positive_direction ? config.speed : -config.speed;
    float backward_speed = -2.0 * forward_speed;

    Logger::instance().logI("Starting " + String(motor.name()) + "-axis homing....");

    while (true)
    {
        // Start running toward the home swtich    
        PBIO_RETURN_ON_ERROR(motor.run(forward_speed));

        // Wait for the home switch to be pressed
        while (digitalRead(home_switch_pin) != switch_pressed_value) {
            delay(PBIO_CONFIG_SERVO_PERIOD_MS);
            IF_CANCELLED(cancel_token, {
                // If the homing is canceled, stop the motor
                motor.stop();
                Logger::instance().logI("Homing canceled by the user");
                return PBIO_ERROR_CANCELED;
            });
        }

        // The switch is pressed
        float hit_position = motor.angle();
        float travel_distance = abs(hit_position - start_position);
        if (travel_distance >= config.minimum_travel) {
            // We hit the switch after moving the minimum distance
            motor.reset_angle(config.switch_axis_position);
            Logger::instance().logI(motor.name() + String("-axis hit home switch"));
            break;
        }
        else {
            if (!canRetry) {                
                // We hit the switch too early and we already tried to retract
                motor.stop();
                Logger::instance().logE(motor.name() + String("-axis hit home switch too early after retrying. Aborting homing"));
                return PBIO_ERROR_HOME_SWITCH_ERR;
            }

            // Retract and try again
            canRetry = false; // Prevent further retries
            Logger::instance().logI(motor.name() + String("-axis hit home switch too early. Retract and try again"));
            PBIO_RETURN_ON_ERROR(motor.run_angle(backward_speed, config.minimum_travel * 1.05, PBIO_ACTUATION_HOLD, true));
            start_position = motor.angle();
            // Test if the switch is released
            if (digitalRead(home_switch_pin) == switch_pressed_value) {
                // The switch should not be pressed after retracting
                Logger::instance().logE("Home switch still pressed after retracting");
                return PBIO_ERROR_HOME_SWITCH_ERR;
            }
        }
    }
    
    // Move away from the switch 
    PBIO_RETURN_ON_ERROR(motor.run_angle(backward_speed, config.retract_distance, PBIO_ACTUATION_COAST, true));

    // Test if the switch is released
    if (digitalRead(home_switch_pin) == switch_pressed_value) {
        // The switch should not be pressed after retracting
        Logger::instance().logE("Home switch still pressed after retracting");
        return PBIO_ERROR_HOME_SWITCH_ERR;
    }

    return PBIO_SUCCESS;
}