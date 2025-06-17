// SPDX-License-Identifier: MIT
// Copyright (c) 2018-2020 The Pybricks Authors

#include "motor_control/control.hpp"

/**
Loop function that control the motor

:param time_now: Current time (us)
:param count_now: Current encoder angle (count)
:param rate_now: Current speed (count/sec)
:param update_period_ms: Time interval when the loop funtion is called (ms)
:param actuation_type: Return current control actuation (pbio_actuation_t)
:param control: Return motor output (duty steps)
 */
void control_update(pbio_control_t *ctl, int32_t time_now, int32_t count_now, int32_t rate_now, pbio_actuation_t *actuation_type, int32_t *control) {

    // Declare current time, positions, rates, and their reference value and error
    int32_t time_ref;
    int32_t count_ref, count_ref_ext, count_err, count_err_integral, rate_err_integral;
    int32_t rate_ref, rate_err;
    int32_t acceleration_ref;
    int32_t duty, duty_due_to_proportional, duty_due_to_integral, duty_due_to_derivative, duty_feedforward;

    // Get the time at which we want to evaluate the reference position/velocities.
    // This compensates for any time we may have spent pausing when the motor was stalled.
    time_ref = pbio_control_get_ref_time(ctl, time_now);

    // Get reference signals
    pbio_trajectory_get_reference(&ctl->trajectory, time_ref, &count_ref, &count_ref_ext, &rate_ref, &acceleration_ref);

    // Calculate control errors, depending on whether we do angle control or speed control
    if (ctl->type == PBIO_CONTROL_ANGLE) {
        // Update count integral error and get current error state
        pbio_count_integrator_update(&ctl->count_integrator, time_now, count_now, count_ref, ctl->trajectory.th3, ctl->settings.integral_range, ctl->settings.integral_rate);
        pbio_count_integrator_get_errors(&ctl->count_integrator, count_now, count_ref, &count_err, &count_err_integral);
        rate_err = rate_ref - rate_now;
    }
    else {
        // For time/speed based commands, the main error is speed. It integrates into a quantity with unit of position.
        // There is no count integral control, because we do not need a second order integrator for speed control.
        pbio_rate_integrator_get_errors(&ctl->rate_integrator, rate_now, rate_ref, count_now, count_ref, &rate_err, &rate_err_integral);
        count_err = rate_err_integral;
        count_err_integral = 0;
    }

    // Corresponding PID control signal
    duty_due_to_proportional = ctl->settings.pid_kp*count_err;
    duty_due_to_derivative = ctl->settings.pid_kd*rate_err;
    duty_due_to_integral = (ctl->settings.pid_ki*(count_err_integral/US_PER_MS))/MS_PER_SECOND;
    duty_feedforward = pbio_math_sign(rate_ref)*ctl->settings.control_offset;

    // Total duty signal, capped by the actuation limit
    duty = duty_due_to_proportional + duty_due_to_integral + duty_due_to_derivative + duty_feedforward;
    duty = PIO_MAX(-ctl->settings.max_control, PIO_MIN(duty, ctl->settings.max_control));

    // This completes the computation of the control signal.
    // The next steps take care of handling windup, or triggering a stop if we are on target.

    // We want to stop building up further errors if we are at the proportional duty limit. So, we pause the trajectory
    // if we get at this limit. We wait a little longer though, to make sure it does not fall back to below the limit
    // within one sample, which we can predict using the current rate times the loop time, with a factor two tolerance.
    int32_t max_windup_duty = (ctl->settings.max_control - ctl->settings.control_offset) + (ctl->settings.pid_kp * abs(rate_now) * PBIO_CONFIG_SERVO_PERIOD_MS * 2) / MS_PER_SECOND;

    // Position anti-windup: pause trajectory or integration if falling behind despite using maximum duty

    // Position anti-windup in case of angle control (position error may not get too high)
    if (ctl->type == PBIO_CONTROL_ANGLE) {
        if (abs(duty_due_to_proportional) >= max_windup_duty) {
            // We are at the duty limit and we should prevent further position error integration.
            pbio_count_integrator_pause(&ctl->count_integrator, time_now, count_now, count_ref);
        }
        else {
            // Not at the limitm so continue integrating errors
            pbio_count_integrator_resume(&ctl->count_integrator, time_now, count_now, count_ref);
        }
    }
    // Position anti-windup in case of timed speed control (speed integral may not get too high)
    else {
        if (abs(duty_due_to_proportional) >= max_windup_duty && pbio_math_sign(duty_due_to_proportional) == pbio_math_sign(rate_err)) {
            // We are at the duty limit and we should prevent further speed error integration.
            pbio_rate_integrator_pause(&ctl->rate_integrator, time_now, count_now, count_ref);
        }
        else {
            // Not at the limitm so continue integrating errors
            pbio_rate_integrator_resume(&ctl->rate_integrator, time_now, count_now, count_ref);
        }
    }

    // Check if controller is stalled
    ctl->stalled = ctl->type == PBIO_CONTROL_ANGLE ? 
                   pbio_count_integrator_stalled(&ctl->count_integrator, time_now, rate_now, ctl->settings.stall_time, ctl->settings.stall_rate_limit) :
                   pbio_rate_integrator_stalled(&ctl->rate_integrator, time_now, rate_now, ctl->settings.stall_time, ctl->settings.stall_rate_limit);

    // Check if we are on target
    ctl->on_target = ctl->on_target_func(&ctl->trajectory, &ctl->settings, time_ref, count_now, rate_now, ctl->stalled);

    // If we are done and the next action is passive then return zero actuation
    if (ctl->on_target && ctl->after_stop != PBIO_ACTUATION_HOLD) {
        *actuation_type = ctl->after_stop;
        *control = 0;
        pbio_control_stop(ctl);
    }
    else if (ctl->on_target && ctl->after_stop == PBIO_ACTUATION_HOLD && ctl->type == PBIO_CONTROL_TIMED) {
        // If we are going to hold and we are already doing angle control, there is nothing we need to do.
        // But if we are going to hold when we are doing speed control right now, we must trigger a hold first.
        pbio_control_start_hold_control(ctl, time_now, count_now);

        // The new hold control does not take effect until the next iteration, so keep actuating for now.
        *actuation_type = PBIO_ACTUATION_DUTY;
        *control = duty;
    }
    else {
        // The end point not reached, or we have to keep holding, so return the calculated duty for actuation
        *actuation_type = PBIO_ACTUATION_DUTY;
        *control = duty;
    }
}

/**
Stops the motor and lets it spin freely
 */
void pbio_control_stop(pbio_control_t *ctl) {
    ctl->type = PBIO_CONTROL_NONE;
    ctl->on_target = true;
    ctl->on_target_func = pbio_control_on_target_always;
    ctl->stalled = false;
}

/**
Runs the motor at a constant speed towards a given target angle

:param time_now: Current time (us)
:param count_now: Current encoder angle (count)
:param target_count: Target encoder angle (count)
:param rate_now: Current speed (count/sec)
:param target_rate: Target speed (count/sec)
:param acceleration: Acceleration/deceleration rate (count/sec^2)
:param after_stop: What to do after reaching target angle
 */
pbio_error_t pbio_control_start_angle_control(pbio_control_t *ctl, int32_t time_now, int32_t count_now, int32_t target_count, int32_t rate_now, int32_t target_rate, int32_t acceleration, pbio_actuation_t after_stop) {

    pbio_error_t err;

    // Set new maneuver action and stop type, and state
    ctl->after_stop = after_stop;
    ctl->on_target = false;
    ctl->on_target_func = pbio_control_on_target_angle;

    // Compute the trajectory
    if (ctl->type == PBIO_CONTROL_NONE) {
        // If no control is ongoing, start from physical state
        err = pbio_trajectory_make_angle_based(&ctl->trajectory, time_now, count_now, target_count, rate_now, target_rate, ctl->settings.max_rate, acceleration, ctl->settings.abs_acceleration);
        if (err != PBIO_SUCCESS) {
            return err;
        }
    }
    else {
        // If control is ongoing, start from its current reference. First get time on current reference signal
        int32_t time_ref = pbio_control_get_ref_time(ctl, time_now);

        // Make the new trajectory and try to patch to existing one
        err = pbio_trajectory_make_angle_based_patched(&ctl->trajectory, time_ref, target_count, target_rate, ctl->settings.max_rate, acceleration, ctl->settings.abs_acceleration);
        if (err != PBIO_SUCCESS) {
            return err;
        }
    }

    // Reset PID control if needed
    if (ctl->type != PBIO_CONTROL_ANGLE) {
        // New angle maneuver, so reset the rate integrator
        int32_t integrator_max = pbio_control_settings_get_max_integrator(&ctl->settings);
        pbio_count_integrator_reset(&ctl->count_integrator, ctl->trajectory.t0, ctl->trajectory.th0, ctl->trajectory.th0, integrator_max);

        // Set the new control state
        ctl->type = PBIO_CONTROL_ANGLE;
    }

    return PBIO_SUCCESS;
}

/**
Runs the motor at a constant speed by a given angle

:param time_now: Current time (us)
:param count_now: Current encoder angle (count)
:param relative_target_count: Angle by which the motor should rotate (count)
:param rate_now: Current speed (count/sec)
:param target_rate: Target speed (count/sec)
:param acceleration: Acceleration/deceleration rate (count/sec^2)
:param after_stop: What to do after reaching target angle
 */
pbio_error_t pbio_control_start_relative_angle_control(pbio_control_t *ctl, int32_t time_now, int32_t count_now, int32_t relative_target_count, int32_t rate_now, int32_t target_rate, int32_t acceleration, pbio_actuation_t after_stop) {

    // Get the count from which the relative count is to be counted
    int32_t count_start;

    // If no control is active, count from the physical count
    if (ctl->type == PBIO_CONTROL_NONE) {
        count_start = count_now;
    }
    // Otherwise count from the current reference
    else {
        int32_t time_ref = pbio_control_get_ref_time(ctl, time_now);
        int32_t unused;
        pbio_trajectory_get_reference(&ctl->trajectory, time_ref, &count_start, &unused, &unused, &unused);
    }

    // The target count is the start count plus the count to be traveled.  If speed is negative, traveled count also flips.
    int32_t target_count = count_start + (target_rate < 0 ? -relative_target_count: relative_target_count);

    // FIXME: Enable 0 angle and angle > 0 with excess speed as standard case instead to decelerate & return.
    if (target_count == count_start) {
        pbio_control_start_hold_control(ctl, time_now, target_count);
        return PBIO_SUCCESS;
    }

    return pbio_control_start_angle_control(ctl, time_now, count_now, target_count, rate_now, target_rate, acceleration, after_stop);
}

/**
Hold the motor at a given angle

:param time_now: Current time (us)
:param target_count: Angle to hold (count)
 */
void pbio_control_start_hold_control(pbio_control_t *ctl, int32_t time_now, int32_t target_count) {

    // Set new maneuver action and stop type, and state
    ctl->after_stop = PBIO_ACTUATION_HOLD;
    ctl->on_target = false;
    ctl->on_target_func = pbio_control_on_target_always;

    // Compute new maneuver based on user argument, starting from the initial state
    pbio_trajectory_make_stationary(&ctl->trajectory, time_now, target_count);
    // If called for the first time, set state and reset PID
    if (ctl->type != PBIO_CONTROL_ANGLE) {
        // Initialize or reset the PID control status for the given maneuver
        int32_t integrator_max = pbio_control_settings_get_max_integrator(&ctl->settings);
        pbio_count_integrator_reset(&ctl->count_integrator, ctl->trajectory.t0, ctl->trajectory.th0, ctl->trajectory.th0, integrator_max);

        // This is an angular control maneuver
        ctl->type = PBIO_CONTROL_ANGLE;
    }
}

/**
Runs the motor at a constant speed for a given amount of time

:param time_now: Current time (us)
:param duration: TDuration of the maneuver (us)
:param count_now: Current encoder angle (count)
:param rate_now: Current speed (count/sec)
:param target_rate: Target speed (count/sec)
:param acceleration: Acceleration/deceleration rate (count/sec^2)
:param stop_func: On target function to call
:param after_stop: What to do after reaching target angle
 */
pbio_error_t pbio_control_start_timed_control(pbio_control_t *ctl, int32_t time_now, int32_t duration, int32_t count_now, int32_t rate_now, int32_t target_rate, int32_t acceleration, pbio_control_on_target_t stop_func, pbio_actuation_t after_stop) {

    pbio_error_t err;

    // Set new maneuver action and stop type, and state
    ctl->after_stop = after_stop;
    ctl->on_target = false;
    ctl->on_target_func = stop_func;

    // Compute the trajectory
    if (ctl->type == PBIO_CONTROL_TIMED) {
        // If timed control is already ongoing make the new trajectory and try to patch to existing one
        err = pbio_trajectory_make_time_based_patched(&ctl->trajectory, time_now, duration, target_rate, ctl->settings.max_rate, acceleration, ctl->settings.abs_acceleration);
        if (err != PBIO_SUCCESS) {
            return err;
        }
    }
    else if (ctl->type == PBIO_CONTROL_ANGLE) {
        // If position based control is ongoing, start from its current reference. First get current reference signal.
        int32_t time_ref = pbio_control_get_ref_time(ctl, time_now);
        int32_t count_start, rate_start, unused;
        pbio_trajectory_get_reference(&ctl->trajectory, time_ref, &count_start, &unused, &rate_start, &unused);

        // Now start the timed trajectory from there
        err = pbio_trajectory_make_time_based(&ctl->trajectory, time_now, duration, count_start, 0, rate_start, target_rate, ctl->settings.max_rate, acceleration, ctl->settings.abs_acceleration);
        if (err != PBIO_SUCCESS) {
            return err;
        }
    }
    else {
        // If no control is ongoing, start from physical state
        err = pbio_trajectory_make_time_based(&ctl->trajectory, time_now, duration, count_now, 0, rate_now, target_rate, ctl->settings.max_rate, acceleration, ctl->settings.abs_acceleration);
        if (err != PBIO_SUCCESS) {
            return err;
        }
    }

    // Reset PD control if needed
    if (ctl->type != PBIO_CONTROL_TIMED) {
        // New maneuver, so reset the rate integrator
        pbio_rate_integrator_reset(&ctl->rate_integrator, time_now, count_now, count_now);

        // Set the new control state
        ctl->type = PBIO_CONTROL_TIMED;
    }

    return PBIO_SUCCESS;
}

/**
On target function that return always True

:param time: Actual time (us)
:param count: Actual encoder count (count)
:param rate: Actual speed (count/s)
:param stalled: True when the motor is stalled
:return: Always True
*/
static bool _pbio_control_on_target_always(pbio_trajectory_t *trajectory, pbio_control_settings_t *settings, int32_t time, int32_t count, int32_t rate, bool stalled) {
    return true;
}
pbio_control_on_target_t pbio_control_on_target_always = _pbio_control_on_target_always;

/**
On target function that return always False

:param time: Actual time (us)
:param count: Actual encoder count (count)
:param rate: Actual speed (count/s)
:param stalled: True when the motor is stalled
:return: Always False
 */
static bool _pbio_control_on_target_never(pbio_trajectory_t *trajectory, pbio_control_settings_t *settings, int32_t time, int32_t count, int32_t rate, bool stalled) {
    return false;
}
pbio_control_on_target_t pbio_control_on_target_never = _pbio_control_on_target_never;

/** 
On target function that return true when trajectory angle (th3) is reached and the motor is standstill

:param time: Actual time (us)
:param count: Actual encoder count (count)
:param rate: Actual speed (count/s)
:param stalled: True when the motor is stalled
:return: True when trajectory angle (th3) is reached and the motor is standstill
*/
static bool _pbio_control_on_target_angle(pbio_trajectory_t *trajectory, pbio_control_settings_t *settings, int32_t time, int32_t count, int32_t rate, bool stalled) {
    // if not enough time has expired to be done even in the ideal case, we are certainly not done
    if (time - trajectory->t3 < 0) {
        return false;
    }

    // If distance to target is still bigger than the tolerance, we are not there yet.
    if (trajectory->th3 - count > settings->count_tolerance) {
        return false;
    }

    // // If distance past target is still bigger than the tolerance, we are too far, so not there yet
    if (count - trajectory->th3 > settings->count_tolerance) {
        return false;
    }

    // If the motor is not standing still, we are not there yet
    if (abs(rate) > settings->rate_tolerance) {
        return false;
    }

    // There's nothing left to do, so we must be on target
    return true;
}
pbio_control_on_target_t pbio_control_on_target_angle = _pbio_control_on_target_angle;

/**
On target function that return true when trajectory time (t3) is elapsed

:param time: Actual time (us)
:param count: Actual encoder count (count)
:param rate: Actual speed (count/s)
:param stalled: True when the motor is stalled
:return: True when trajectory time (t3) is elapsed
 */
static bool _pbio_control_on_target_time(pbio_trajectory_t *trajectory, pbio_control_settings_t *settings, int32_t time, int32_t count, int32_t rate, bool stalled) {
    return time >= trajectory->t3;
}
pbio_control_on_target_t pbio_control_on_target_time = _pbio_control_on_target_time;

/**
On target function that return true when the motor is stalled

:param time: Actual time (us)
:param count: Actual encoder count (count)
:param rate: Actual speed (count/s)
:param stalled: True when the motor is stalled
:return: True when the motor is stalled
 */
static bool _pbio_control_on_target_stalled(pbio_trajectory_t *trajectory, pbio_control_settings_t *settings, int32_t time, int32_t count, int32_t rate, bool stalled) {
    return stalled;
}
pbio_control_on_target_t pbio_control_on_target_stalled = _pbio_control_on_target_stalled;

/**
Convert control units (counts, rate) in physical user units (deg or mm, deg/s or mm/s)

:param: Counts or rate in encoder count
:return: Counts or rate in user units (deg or mm, deg/s or mm/s)
*/
float pbio_control_counts_to_user(const pbio_control_settings_t *s, int32_t counts) {
    return counts / s->counts_per_unit;
}

/**
Convert physical user units (deg or mm, deg/s or mm/s) in control units (counts, rate)

:param: Counts or rate in user units (deg or mm, deg/s or mm/s)
:return: Counts or rate in encoder count
 */
int32_t pbio_control_user_to_counts(const pbio_control_settings_t *s, float user) {
    return (int32_t)(user * s->counts_per_unit);
}

/**
Return control limits in user units (gear ratio)

:param speed: Return max speed
:param acceleration: Return max acceleration
:param actuation: Return max actuation percentage (0-100)
 */
void pbio_control_settings_get_limits(const pbio_control_settings_t *s, float *speed, float *acceleration, int32_t *actuation) {
    *speed = pbio_control_counts_to_user(s, s->max_rate);
    *acceleration = pbio_control_counts_to_user(s, s->abs_acceleration);
    *actuation = s->max_control / s->actuation_scale;
}

/**
Set control limits in user units (gear ratio)

:param speed: Maximum speed in user units
:param acceleration: Maximum acceleration in user units
:param actuation: Maximum actuation percentage (0 to 100)
 */
pbio_error_t pbio_control_settings_set_limits(pbio_control_settings_t *s, float speed, float acceleration, int32_t actuation) {
    if (speed < 1 || acceleration < 1 || actuation < 1) {
        return PBIO_ERROR_INVALID_ARG;
    }
    if (actuation * s->actuation_scale <= s->control_offset) {
        return PBIO_ERROR_INVALID_OP;
    }
    s->max_rate = pbio_control_user_to_counts(s, speed);
    s->abs_acceleration = pbio_control_user_to_counts(s, acceleration);
    s->max_control = actuation * s->actuation_scale;
    return PBIO_SUCCESS;
}

/**
Return pid settings

:param pid_kp: Return PID Kp
:param pid_ki: Return PID Ki
:param pid_kd: Return PID Kd
:param integral_range: Return integral range: Region around the target count in which integral errors are accumulated (user units)
:param integral_rate: Return integral rate: Maximum rate at which the integrator is allowed to increase  (user units/s)
:param control_offset: Return control offset: Constant feedforward signal added in the reference direction percentage (0 to 100)
 */
void pbio_control_settings_get_pid(const pbio_control_settings_t *s, int16_t *pid_kp, int16_t *pid_ki, int16_t *pid_kd, float *integral_range, float *integral_rate, int32_t *control_offset) {
    *pid_kp = s->pid_kp;
    *pid_ki = s->pid_ki;
    *pid_kd = s->pid_kd;
    *integral_range = pbio_control_counts_to_user(s, s->integral_range);
    *integral_rate = pbio_control_counts_to_user(s, s->integral_rate);
    *control_offset = s->control_offset / s->actuation_scale;
}

/**
Set PID settings

:param pid_kp: PID Kp
:param pid_ki: PID Ki
:param pid_kd: PID Kd
:param integral_range: Region around the target count in which integral errors are accumulated (user units)
:param integral_rate: Maximum rate at which the integrator is allowed to increase  (user units/s)
:param control_offset: Constant feedforward signal added in the reference direction percentage (0 to 100)
 */
pbio_error_t pbio_control_settings_set_pid(pbio_control_settings_t *s, int16_t pid_kp, int16_t pid_ki, int16_t pid_kd, float integral_range, float integral_rate, int32_t control_offset) {
    if (pid_kp < 0 || pid_ki < 0 || pid_kd < 0 || integral_range < 0 || integral_rate < 0 || control_offset < 0) {
        return PBIO_ERROR_INVALID_ARG;
    }
    if (control_offset * s->actuation_scale >= s->max_control) {
        return PBIO_ERROR_INVALID_OP;
    }

    s->pid_kp = pid_kp;
    s->pid_ki = pid_ki;
    s->pid_kd = pid_kd;
    s->integral_range = pbio_control_user_to_counts(s, integral_range);
    s->integral_rate = pbio_control_user_to_counts(s, integral_rate);
    s->control_offset = control_offset * s->actuation_scale;
    return PBIO_SUCCESS;
}

/**
Return position and speed tolerance in user units to consider the movement done

:param speed: Return speed tolerance
:param position: Return position tolerance 
 */
void pbio_control_settings_get_target_tolerances(const pbio_control_settings_t *s, float *speed, float *position) {
    *position = pbio_control_counts_to_user(s, s->count_tolerance);
    *speed = pbio_control_counts_to_user(s, s->rate_tolerance);
}

/**
Set position and speed tolerance in user units to consider the movement done

:param speed: Speed tolerance in user units
:param position: Position tolerance in user units
 */
pbio_error_t pbio_control_settings_set_target_tolerances(pbio_control_settings_t *s, float speed, float position) {
    if (position < 0 || speed < 0) {
        return PBIO_ERROR_INVALID_ARG;
    }

    s->count_tolerance = pbio_control_user_to_counts(s, position);
    s->rate_tolerance = pbio_control_user_to_counts(s, speed);
    return PBIO_SUCCESS;
}

/**
Return stall tolerances in user units

:param speed: Return speed tolerance (user unit)
:param time: Reutrn time tolerance (ms)
 */
void pbio_control_settings_get_stall_tolerances(const pbio_control_settings_t *s,  float *speed, int32_t *time) {
    *speed = pbio_control_counts_to_user(s, s->stall_rate_limit);
    *time = s->stall_time / US_PER_MS;
}

/**
 Set stall tolerances in user units

:param speed: Speed tolerance in user units
:param time: Time tolerance in ms
 */
pbio_error_t pbio_control_settings_set_stall_tolerances(pbio_control_settings_t *s, float speed, int32_t time) {
    if (speed < 0 || time < 0) {
        return PBIO_ERROR_INVALID_ARG;
    }

    s->stall_rate_limit = pbio_control_user_to_counts(s, speed);
    s->stall_time = time * US_PER_MS; 
    return PBIO_SUCCESS;
}

/**
Calculate the maximum integrator value for witch ki*integrator does not exceed max_control

:return: Integrator max value
 */
int32_t pbio_control_settings_get_max_integrator(const pbio_control_settings_t *s) {
    // If ki is very small, then the integrator is "unlimited"
    if (s->pid_ki <= 10) {
        return 1000000000;
    }
    // Get the maximum integrator value for which ki*integrator does not exceed max_control
    return ((s->max_control*US_PER_MS)/s->pid_ki)*MS_PER_SECOND;
}

/**
Return the integrator current time

:param time_now: Current time in us
:return: Integrator current reference time (us)
 */
int32_t pbio_control_get_ref_time(const pbio_control_t *ctl, int32_t time_now) {

    if (ctl->type == PBIO_CONTROL_ANGLE) {
        return pbio_count_integrator_get_ref_time(&ctl->count_integrator, time_now);
    }
    if (ctl->type == PBIO_CONTROL_TIMED) {
        return time_now;
    }
    return 0;
}

/**
Return true when there is an ongoing command and the motor is stalled
*/
bool pbio_control_is_stalled(pbio_control_t *ctl) {
    return ctl->type != PBIO_CONTROL_NONE && ctl->stalled;
}

/**
Checks if an ongoing command or maneuver is done

:return: True is the maneuver is done
*/
bool pbio_control_is_done(pbio_control_t *ctl) {
    return ctl->type == PBIO_CONTROL_NONE || ctl->on_target;
}

/**
Return max speed control limit in user units

:return: Return max speed (user unit/s)
*/
float pbio_control_settings_get_speed_limit(const pbio_control_settings_t *s) {
    return pbio_control_counts_to_user(s, s->max_rate);
}

/**
Return max acceleration control limit in user units

:return: Return max acceleration (user unit/s^2)
*/
float pbio_control_settings_get_acceleration_limit(const pbio_control_settings_t *s) {
    return pbio_control_counts_to_user(s, s->abs_acceleration);
}

/**
Return actuation control limit in user units

:return: Return max actuation percentage (0-100%)
 */
int32_t pbio_control_settings_get_actuation_limit(const pbio_control_settings_t *s) {
    return s->max_control / s->actuation_scale;
}

/**
Set speed limit in user units

:param speed: Maximum speed (user units/s)
*/
pbio_error_t pbio_control_settings_set_speed_limit(pbio_control_settings_t *s, float speed) {
    if (speed < 1) {
        return PBIO_ERROR_INVALID_ARG;
    }

    s->max_rate = pbio_control_user_to_counts(s, speed);
    return PBIO_SUCCESS;
}

/**
Set acceleration limit in user units

:param acceleration: Maximum acceleration (user units/s^2)
*/
pbio_error_t pbio_control_settings_set_acceleration_limit(pbio_control_settings_t *s, float acceleration) {
    if (acceleration < 1) {
        return PBIO_ERROR_INVALID_ARG;
    }

    s->abs_acceleration = pbio_control_user_to_counts(s, acceleration);
    return PBIO_SUCCESS;
}

/**
Set actuation limit in percentage

:param actuation: Maximum actuation percentage (0 to 100%)
*/
pbio_error_t pbio_control_settings_set_actuation_limit(pbio_control_settings_t *s, int32_t actuation) {
    if (actuation < 1) {
        return PBIO_ERROR_INVALID_ARG;
    }
    if (actuation * s->actuation_scale <= s->control_offset) {
        return PBIO_ERROR_INVALID_OP;
    }

    s->max_control = actuation * s->actuation_scale;
    return PBIO_SUCCESS;
}
