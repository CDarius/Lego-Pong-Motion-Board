#include "motor_control/servo.hpp"

void pbio_servo_setup(pbio_servo_t *srv, DCMotor *dcmotor, Tacho *tacho, float counts_per_unit, pbio_control_settings_t *settings) {
    srv->tacho = tacho;
    srv->dcmotor = dcmotor;

    // Reset state
    pbio_control_stop(&srv->control);

    // Load default settings for this device type
    srv->control.settings = *settings;

    // For a servo, counts per output unit is counts per degree at the gear train output
    srv->control.settings.counts_per_unit = counts_per_unit;
}

/** 
Sets the rotation angle of the sensor to a desired value.

:param reset_angle: Value to which the angle should be reset in user unit
*/
void pbio_servo_reset_angle(pbio_servo_t *srv, float reset_angle) {

    pbio_error_t err;

    // If the motor was in a passive mode (coast, brake, user duty),
    // just reset angle and leave motor state unchanged.
    if (srv->control.type == PBIO_CONTROL_NONE) {
        srv->tacho->resetAngle(reset_angle);
        return;
    }

    // If are were busy moving, that means the reset was called while a motor
    // was running in the background. To avoid confusion as to where the motor
    // must go after the reset, we'll make it stop and hold right here right now.

    // Get the old angle
    float angle_old = srv->tacho->getAngle();

    // Get the old target angle that we were tracking until now
    int32_t time_ref = pbio_control_get_ref_time(&srv->control, monotonic_us());
    int32_t count_ref, unused;
    pbio_trajectory_get_reference(&srv->control.trajectory, time_ref, &count_ref, &unused, &unused, &unused);
    float target_old = pbio_control_counts_to_user(&srv->control.settings, count_ref);

    // Reset the angle
    srv->tacho->resetAngle(reset_angle);

    // Set the new target based on the old angle and the old target, after the angle reset
    float new_target = reset_angle + target_old - angle_old;
    pbio_servo_track_target(srv, new_target);
}

/**
Get the physical state of a single motor: current time, speed, and position

:param time_now: Return current time(us)
:param count_now: Return position (count)
:param rate_now: Return speed (count/sec)
 */
static void servo_get_state(pbio_servo_t *srv, int32_t *time_now, int32_t *count_now, int32_t *rate_now) {

    // Read current state of this motor: current time, speed, and position
    *time_now = monotonic_us();
    *count_now = srv->tacho->getCount();
    *rate_now = srv->tacho->getRate();
}

/**
Actuate a single motor

:param actuation_type: Type of actuation: coast, brake, hold or duty
:param control: Target angle in count when actuation is HOLD or PWM duty steps when actuation is DUTY
 */
static void pbio_servo_actuate(pbio_servo_t *srv, pbio_actuation_t actuation_type, int32_t control) {

    // Apply the calculated actuation, by type
    switch (actuation_type)
    {
    case PBIO_ACTUATION_COAST:
        srv->dcmotor->coast();
        break; 
    case PBIO_ACTUATION_BRAKE:
        srv->dcmotor->brake();
        break;
    case PBIO_ACTUATION_HOLD:
        pbio_control_start_hold_control(&srv->control, monotonic_us(), control);
        break;
    case PBIO_ACTUATION_DUTY:
        srv->dcmotor->set_duty_cycle(control);
        break;
    }
}

/**
Log motor data for a motor that is being actively controlled

:param time_now: Current time(us)
:param count_now: Current position (count)
:param rate_now: Current speed (count/sec)
:param actuation: Current servo actuation (pbio_actuation_t)
:param control: Current actualtion value (duty steps)
*/
static void pbio_servo_log_update(pbio_servo_t *srv, int32_t time_now, int32_t count_now, int32_t rate_now, pbio_actuation_t actuation, int32_t control) {

    int32_t buf[SERVO_LOG_NUM_VALUES];
    memset(buf, 0, sizeof(buf));
    
    // Log the physical state of the motor
    buf[1] = count_now;
    buf[2] = rate_now;

    // Log the applied control signal
    buf[3] = actuation;
    buf[4] = control;

    // If control is active, log additional data about the maneuver
    if (srv->control.type != PBIO_CONTROL_NONE) {
        
        // Get the time of reference evaluation
        int32_t time_ref = pbio_control_get_ref_time(&srv->control, time_now);

        // Log the time since start of control trajectory
        buf[0] = (time_ref - srv->control.trajectory.t0) / 1000;

        // Log reference signals. These values are only meaningful for time based commands
        int32_t count_ref, count_ref_ext, rate_ref, err, err_integral, acceleration_ref;
        pbio_trajectory_get_reference(&srv->control.trajectory, time_ref, &count_ref, &count_ref_ext, &rate_ref, &acceleration_ref);

        if (srv->control.type == PBIO_CONTROL_ANGLE) {
            pbio_count_integrator_get_errors(&srv->control.count_integrator, count_now, count_ref, &err, &err_integral);
        }
        else {
            pbio_rate_integrator_get_errors(&srv->control.rate_integrator, rate_now, rate_ref, count_now, count_ref, &err, &err_integral);
        }

        buf[5] = count_ref;
        buf[6] = rate_ref;
        buf[7] = err; // count err for angle control, rate err for timed control
        buf[8] = err_integral;
    }
}

/**
Loop function that control and actuate the motor
 */
pbio_error_t pbio_servo_control_update(pbio_servo_t *srv) {

    // Read the physical state
    int32_t time_now;
    int32_t count_now;
    int32_t rate_now;
    servo_get_state(srv, &time_now, &count_now, &rate_now);
    if (srv->tacho->isSequenceError())
        return PBIO_ERROR_TACHO_SEQUENCE;

    // Control action to be calculated
    pbio_actuation_t actuation;
    int32_t control;

    // Do not service a passive motor
    if (srv->control.type == PBIO_CONTROL_NONE) {
        // No control, but still log state data
        pbio_passivity_t state;
        pbio_actuation_t actuation;        
        srv->dcmotor->getState(&state, &control);
        switch (state) {
            case PBIO_DCMOTOR_COAST:
                actuation = PBIO_ACTUATION_COAST;
                break;
            case PBIO_DCMOTOR_BRAKE:
                actuation = PBIO_ACTUATION_BRAKE;
                break;
            case PBIO_DCMOTOR_DUTY_PASSIVE:
                actuation = PBIO_ACTUATION_DUTY;
                break;
        };
        pbio_servo_log_update(srv, time_now, count_now, rate_now, actuation, control);
        return PBIO_SUCCESS;
    }

    // Calculate control signal
    control_update(&srv->control, time_now, count_now, rate_now, &actuation, &control);

    // Apply the control type and signal
    pbio_servo_actuate(srv, actuation, control);

    // Log data if logger enabled
    pbio_servo_log_update(srv, time_now, count_now, rate_now, actuation, control);

    return PBIO_SUCCESS;
}

/**
 Actuate the motor in open loop

:param duty_steps: PWM duty steps Range -100 to 100
 */
void pbio_servo_set_duty_cycle(pbio_servo_t *srv, float duty_steps) {
    pbio_control_stop(&srv->control);

    int32_t control = (int32_t)(duty_steps * srv->dcmotor->getUserPwmMax() / 100.0);
    srv->dcmotor->set_duty_cycle(control);
}

/**
Stops the motor

:param after_stop: How to stop: coast, brake or hold
 */
void pbio_servo_stop(pbio_servo_t *srv, pbio_actuation_t after_stop) {

    // Get control payload
    int32_t control;
    if (after_stop == PBIO_ACTUATION_HOLD) {
        // For hold, the actuation payload is the current count
        control = srv->tacho->getCount();
    }
    else {
        // Otherwise the payload is zero and control stops
        control = 0;
        pbio_control_stop(&srv->control);
    }

    // Apply the actuation
    pbio_servo_actuate(srv, after_stop, control);
}

/**
Runs the motor at a constant speed.

The motor accelerates to the given speed and keeps running at this speed until you give a new command.

:param speed: Speed of the motor in deg/s
 */
pbio_error_t pbio_servo_run(pbio_servo_t *srv, float speed) {
    // Get target rate in unit of counts
    int32_t target_rate = pbio_control_user_to_counts(&srv->control.settings, speed);

    // Get the initial physical motor state.
    int32_t time_now, count_now, rate_now;

    // FIXME: Make state getter function a control property. That way, it can
    // decide whether reading the state is needed, instead of checking control
    // status here. We do it here for now anyway to reduce I/O if the initial
    // state value is not actually used, like when control is already active.
    if (srv->control.type == PBIO_CONTROL_NONE) {
        // Get the current physical state.
        servo_get_state(srv, &time_now, &count_now, &rate_now);
    }
    else {
        time_now = monotonic_us();
    }

    // Start a timed maneuver, duration forever
    return pbio_control_start_timed_control(&srv->control, time_now, DURATION_FOREVER, count_now, rate_now, target_rate, srv->control.settings.abs_acceleration, pbio_control_on_target_never, PBIO_ACTUATION_COAST);
}

/**
Runs the motor at a constant speed for a given amount of time.

The motor accelerates to the given speed, keeps running at this speed, and then decelerates. The total maneuver lasts for exactly the given amount of time.

:param speed: Speed of the motor in deg/s
:param duration: Duration of the maneuver in ms
:param after_stop: What to do after coming to a standstill
 */
pbio_error_t pbio_servo_run_time(pbio_servo_t *srv, float speed, int32_t duration, pbio_actuation_t after_stop) {
    // Get target rate in unit of counts
    int32_t target_rate = pbio_control_user_to_counts(&srv->control.settings, speed);

    // Get the initial physical motor state.
    int32_t time_now, count_now, rate_now;
    servo_get_state(srv, &time_now, &count_now, &rate_now);

    // Start a timed maneuver, duration finite
    return pbio_control_start_timed_control(&srv->control, time_now, duration*US_PER_MS, count_now, rate_now, target_rate, srv->control.settings.abs_acceleration, pbio_control_on_target_time, after_stop);
}

/**
Runs the motor at a constant speed until it stalls

:param speed: Speed of the motor in deg/s
:param after_stop: What to do after coming to a standstill
 */
pbio_error_t pbio_servo_run_until_stalled(pbio_servo_t *srv, float speed, pbio_actuation_t after_stop) {
    // Get target rate in unit of counts
    int32_t target_rate = pbio_control_user_to_counts(&srv->control.settings, speed);

    // Get the initial physical motor state.
    int32_t time_now, count_now, rate_now;
    servo_get_state(srv, &time_now, &count_now, &rate_now);

    // Start a timed maneuver, duration forever and ending on stall
    return pbio_control_start_timed_control(&srv->control, time_now, DURATION_FOREVER, count_now, rate_now, target_rate, srv->control.settings.abs_acceleration, pbio_control_on_target_stalled, after_stop);
}

/**
Runs the motor at a constant speed towards a given target angle.

The direction of rotation is automatically selected based on the target angle. It does not matter 
if speed is positive or negative.

:param speed: Speed of the motor in deg/s
:param target: Angle that the motor should rotate to in deg
:param after_stop: What to do after coming to a standstill
 */
pbio_error_t pbio_servo_run_target(pbio_servo_t *srv, float speed, float target, pbio_actuation_t after_stop) {
    // Get targets in unit of counts
    int32_t target_rate = pbio_control_user_to_counts(&srv->control.settings, speed);
    int32_t target_count = pbio_control_user_to_counts(&srv->control.settings, target);

    // Get the initial physical motor state.
    int32_t time_now, count_now, rate_now;
    servo_get_state(srv, &time_now, &count_now, &rate_now);

    return pbio_control_start_angle_control(&srv->control, time_now, count_now, target_count, rate_now, target_rate, srv->control.settings.abs_acceleration, after_stop);
}

/**
Runs the motor at a constant speed by a given angle (relative)

:param speed: Speed of the motor in deg/s
:param angle: Angle by which the motor should rotate in deg
:param after_stop: What to do after coming to a standstill
*/
pbio_error_t pbio_servo_run_angle(pbio_servo_t *srv, float speed, float angle, pbio_actuation_t after_stop) {
    // Get targets in unit of counts
    int32_t target_rate = pbio_control_user_to_counts(&srv->control.settings, speed);
    int32_t relative_target_count = pbio_control_user_to_counts(&srv->control.settings, angle);

    // Get the initial physical motor state.
    int32_t time_now, count_now, rate_now;
    servo_get_state(srv, &time_now, &count_now, &rate_now);

    // Start the relative angle control
    return pbio_control_start_relative_angle_control(&srv->control, time_now, count_now, relative_target_count, rate_now, target_rate, srv->control.settings.abs_acceleration, after_stop);
}

/**
Tracks a target angle

This is similar to run_target(), but the usual smooth acceleration is skipped: it will move 
to the target angle as fast as possible. This method is useful if you want to continuously 
change the target angle

:param target: Angle that the motor should rotate to in deg
*/
void pbio_servo_track_target(pbio_servo_t *srv, float target) {
    // Get the intitial state, either based on physical motor state or ongoing maneuver
    int32_t time_start = monotonic_us();
    int32_t target_count = pbio_control_user_to_counts(&srv->control.settings, target);

    pbio_control_start_hold_control(&srv->control, time_start, target_count);
}