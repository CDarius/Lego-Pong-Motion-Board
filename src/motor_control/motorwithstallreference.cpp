#include "motor_control/motorwithstallreference.hpp"

pbio_error_t MotorWithStallReference::run_axis_homing(CancelToken& cancel_token) { 
    bool canRetry = true;
    float start_position = angle();

    float forward_speed = _config.start_in_positive_direction ? _config.speed : -_config.speed;
    float backward_speed = -2.0 * forward_speed;

    Logger::instance().logI("Starting " + String(name()) + "-axis homing....");

    while (true)
    {
        IF_CANCELLED(cancel_token, {
            // If the homing is canceled, stop the motor
            stop();
            Logger::instance().logI("Homing canceled by the user");
            return PBIO_ERROR_CANCELED;
        });

        // Start running toward the home obstacle until stall
        PBIO_RETURN_ON_ERROR(run_until_stalled(forward_speed, _config.duty_limit, PBIO_ACTUATION_HOLD, &cancel_token));

        // Reference obstacle hit
        float hit_position = angle();
        float travel_distance = abs(hit_position - start_position);
        if (travel_distance >= _config.minimum_travel) {
            // We hit the switch after moving the minimum distance
            reset_angle(axis_position_at_home_marker);
            Logger::instance().logI(name() + String("-axis hit the reference obstacle"));
            break;
        }
        else {
            if (!canRetry) {                
                // We hit the obstacle too early and we already tried to retract
                stop();
                Logger::instance().logE(name() + String("-axis hit the reference obstacle too early after retrying. Aborting homing"));
                return PBIO_ERROR_HOME_SWITCH_ERR;
            }

            // Retract and try again
            canRetry = false; // Prevent further retries
            Logger::instance().logI(name() + String("-axis hit the reference obstacle too early. Retract and try again"));
            PBIO_RETURN_ON_ERROR(run_angle(backward_speed, _config.minimum_travel * 1.05, PBIO_ACTUATION_HOLD, true, &cancel_token));
            start_position = angle();
        }
    }

    IF_CANCELLED(cancel_token, {
        // If the homing is canceled, stop the motor
        stop();
        Logger::instance().logI("Homing canceled by the user");
        return PBIO_ERROR_CANCELED;
    });

    // Move away from the reference obstacle
    PBIO_RETURN_ON_ERROR(run_angle(backward_speed, _config.retract_distance, PBIO_ACTUATION_COAST, true, &cancel_token));

    _referenced = true;
    
    return PBIO_SUCCESS;
}