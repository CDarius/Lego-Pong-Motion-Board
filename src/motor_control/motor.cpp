#include "motor_control/motor.hpp"

void Motor::begin(
    const char *name,
    uint8_t encoderPi1,
    uint8_t encoderPi2,
    uint8_t pwmPi1,
    uint8_t pwmPi2,
    pbio_direction_t direction, 
    float gearRatio, 
    pbio_control_settings_t *settings,
    motor_error_output_func_t error_output_func) {
        _name = name;
        float counts_per_unit = gearRatio * ENCODER_COUNTS_PER_DEGREE;
        _tacho.begin(encoderPi1, encoderPi2, counts_per_unit, direction);
        _dcmotor.begin(pwmPi1, pwmPi2, direction, MOTOR_MAX_CONTROL);
        _current_error_output_func = error_output_func;

        pbio_servo_setup(&_servo, &_dcmotor, &_tacho, counts_per_unit, settings);

        update();
}

/**
Gets the rotation angle of the motor (deg)
*/
float Motor::angle() const {
    return _tacho.getAngle();
}

/**
Sets the accumulated rotation angle of the motor to a desired value.

If this motor is also being used by a drive base, its distance and angle values will also be affected. 
You might want to use its reset method instead.

:param angle: Value in deg to which the angle should be reset
*/
void Motor::reset_angle(float angle){
    if (xSemaphoreTake(_xMutex, portMAX_DELAY)) {
        pbio_servo_reset_angle(&_servo, angle);
        xSemaphoreGive(_xMutex);
    }
}

/**
Gets the speed of the motor in deg/s
*/
float Motor::speed() const {
    return _tacho.getAngularRate();
}

/**
 * Gets the speed of the motor in deg/s, but does not take into account the gear ratio.
 */
float Motor::motor_speed() const {
    return ((float)_tacho.getRate()) / ENCODER_COUNTS_PER_DEGREE;
}

void Motor::getState(pbio_passivity_t *state, int32_t *duty_now) const {
    _dcmotor.getState(state, duty_now);
}

/**
Stops the motor and lets it spin freely

The motor gradually stops due to friction
*/
void Motor::stop() {
    if (xSemaphoreTake(_xMutex, portMAX_DELAY)) {
        pbio_servo_stop(&_servo, PBIO_ACTUATION_COAST);
        xSemaphoreGive(_xMutex);
    }
}

/**
Passively brakes the motor

The motor stops due to friction, plus the voltage that is generated while the motor is still moving
*/
void Motor::brake() {
    if (xSemaphoreTake(_xMutex, portMAX_DELAY)) {
        pbio_servo_stop(&_servo, PBIO_ACTUATION_BRAKE);
        xSemaphoreGive(_xMutex);
    }
}

/**
Stops the motor and actively holds it at its current angle.
*/
void Motor::hold() {
    if (xSemaphoreTake(_xMutex, portMAX_DELAY)) {
        pbio_servo_stop(&_servo, PBIO_ACTUATION_HOLD);
        xSemaphoreGive(_xMutex);
    }
}

/**
Rotates the motor at a given duty cycle (also known as “power”).

:param duty: The duty cycle (-100.0 to 100)
 */
void Motor::dc(float duty) {
    if (xSemaphoreTake(_xMutex, portMAX_DELAY)) {
        pbio_servo_set_duty_cycle(&_servo, duty);
        xSemaphoreGive(_xMutex);
    }
}

/**
Runs the motor at a constant speed.

The motor accelerates to the given speed and keeps running at this speed until you give a new command.

:param speed: Speed of the motor in deg/s
*/
pbio_error_t Motor::run(float speed) {
    pbio_error_t err;
    if (xSemaphoreTake(_xMutex, portMAX_DELAY)) {
        err = pbio_servo_run(&_servo, speed);
        xSemaphoreGive(_xMutex);
    }

    output_motor_error(err, "Motor::run(%f)", speed);

    return err;
}

/**
Runs the motor at a constant speed for a given amount of time.

The motor accelerates to the given speed, keeps running at this speed, and then decelerates. 
The total maneuver lasts for exactly the given amount of time.

:param speed: Speed of the motor in deg/s
:param time_ms: Duration of the maneuver in ms
:param then: What to do after coming to a standstill
:param wait: Wait for the maneuver to complete before continuing with the rest of the program

*/
pbio_error_t Motor::run_time(float speed, uint32_t time_ms, pbio_actuation_t then, bool wait, CancelToken* cancel_token) {
    pbio_error_t err;

    if (speed < 0 || time_ms < 0 || time_ms > DURATION_MAX_S*MS_PER_SECOND) {
        err = PBIO_ERROR_INVALID_ARG;

        output_motor_error(err, "Motor::run_time(%f, %u)", speed, time_ms);

        return err;
    }
    
    if (xSemaphoreTake(_xMutex, portMAX_DELAY)) {
        err = pbio_servo_run_time(&_servo, speed, time_ms, then);
        xSemaphoreGive(_xMutex);
    }
    if (err != PBIO_SUCCESS) {
        output_motor_error(err, "Motor::run_time(%f, %u) init failed", speed, time_ms);
        return err;
    }

    if (wait) {
        err = wait_for_completion(cancel_token);
        if (err != PBIO_SUCCESS) {
            output_motor_error(err, "Motor::run_time(%f, %u) movement failed", speed, time_ms);
            return err;
        }
    }

    return PBIO_SUCCESS;
}

/**
Runs the motor at a constant speed until it stalls.

:param speed: Speed of the motor in deg/s
:param duty_limit: Duty cycle limit % during this command. This is useful to avoid applying the full motor torque to a geared or lever mechanism. If it is None, the duty limit won’t be changed during this command
:param then: What to do after coming to a standstill
 */
pbio_error_t Motor::run_until_stalled(float speed, float duty_limit, pbio_actuation_t then, CancelToken* cancel_token) {
    pbio_error_t err;

    // If duty_limit argument, given, limit actuation during this maneuver
    bool override_duty_limit = duty_limit != 100.0;

    float _speed, _acceleration;
    int32_t _actuation, user_limit;

    if (override_duty_limit) {
        // Read original values so we can restore them when we're done
        if (xSemaphoreTake(_xMutex, portMAX_DELAY)) {
            pbio_control_settings_get_limits(&_servo.control.settings, &_speed, &_acceleration, &_actuation);
            xSemaphoreGive(_xMutex);
        }
    
        // Get user given limit
        user_limit = duty_limit;
        user_limit = user_limit < 0 ? -user_limit : user_limit;
        user_limit = user_limit > 100 ? 100 : user_limit;

        // Apply the user limit        
        if (xSemaphoreTake(_xMutex, portMAX_DELAY)) {
            err = pbio_control_settings_set_limits(&_servo.control.settings, _speed, _acceleration, user_limit);
            xSemaphoreGive(_xMutex);
        }
        if (err != PBIO_SUCCESS) {            
            // Try to restore original settings
            if (xSemaphoreTake(_xMutex, portMAX_DELAY)) {
                pbio_control_settings_set_limits(&_servo.control.settings, _speed, _acceleration, _actuation);
                xSemaphoreGive(_xMutex);
            }

            output_motor_error(err, "Motor::run_until_stalled(%f, %f) override limit failed", speed, duty_limit);

            return err;
        }
    }

    // Call pbio with parsed user/default arguments
    if (xSemaphoreTake(_xMutex, portMAX_DELAY)) {
        err = pbio_servo_run_until_stalled(&_servo, speed, then);
        xSemaphoreGive(_xMutex);
    }
    if (err != PBIO_SUCCESS) {
        if (override_duty_limit) {
            // Try to restore original settings
            if (xSemaphoreTake(_xMutex, portMAX_DELAY)) {
                pbio_control_settings_set_limits(&_servo.control.settings, _speed, _acceleration, _actuation);
                xSemaphoreGive(_xMutex);
            }
        }

        output_motor_error(err, "Motor::run_until_stalled(%f, %f) init failed", speed, duty_limit);

        return err;
    }

    // In this command we always wait for completion, so we can return the
    // final angle below.
    err = wait_for_completion(cancel_token);
    if (err != PBIO_SUCCESS) {
        if (override_duty_limit) {
            // Try to restore original settings
            if (xSemaphoreTake(_xMutex, portMAX_DELAY)) {
                pbio_control_settings_set_limits(&_servo.control.settings, _speed, _acceleration, _actuation);
                xSemaphoreGive(_xMutex);
            }
        }

        output_motor_error(err, "Motor::run_until_stalled(%f, %f) movement failed", speed, duty_limit);

        return err;
    }

    // Restore original settings
    if (override_duty_limit) {
        if (xSemaphoreTake(_xMutex, portMAX_DELAY)) {
            err = pbio_control_settings_set_limits(&_servo.control.settings, _speed, _acceleration, _actuation);
            xSemaphoreGive(_xMutex);
        }
        if (err != PBIO_SUCCESS) {
            output_motor_error(err, "Motor::run_until_stalled(%f, %f) restore limits failed", speed, duty_limit);

            return err;
        }
    }

    return PBIO_SUCCESS;
}

/**
Runs the motor at a constant speed by a given angle (relative)

:param speed: Speed of the motor in deg/s
:param rotation_angle: Angle by which the motor should rotate in deg
:param then: What to do after coming to a standstill
:param wait: Wait for the maneuver to complete before continuing with the rest of the program
*/
pbio_error_t Motor::run_angle(float speed, float angle, pbio_actuation_t then, bool wait, CancelToken* cancel_token) {
    pbio_error_t err;
    if (xSemaphoreTake(_xMutex, portMAX_DELAY)) {
        err = pbio_servo_run_angle(&_servo, speed, angle, then);
        xSemaphoreGive(_xMutex);
    }
    if (err != PBIO_SUCCESS) {
        output_motor_error(err, "Motor::run_angle(%f, %f) init failed", speed, angle);
        return err;
    }

    if (wait) {
        err = wait_for_completion(cancel_token);
        if (err != PBIO_SUCCESS) {
            output_motor_error(err, "Motor::run_angle(%f, %f) movement failed", speed, angle);
            return err;
        }
    }

    return PBIO_SUCCESS;
}

/**
Runs the motor at a constant speed towards a given target angle.

The direction of rotation is automatically selected based on the target angle. It does not matter if speed is positive or negative.

:param speed: Speed of the motor in deg/s
:param target_angle: Angle that the motor should rotate to in deg
:param then: What to do after coming to a standstill
:param wait: Wait for the maneuver to complete before continuing with the rest of the program
*/
pbio_error_t Motor::run_target(float speed, float target_angle, pbio_actuation_t then, bool wait, CancelToken* cancel_token) {
    pbio_error_t err;
    if (xSemaphoreTake(_xMutex, portMAX_DELAY)) {
        err = pbio_servo_run_target(&_servo, speed, target_angle, then);
        xSemaphoreGive(_xMutex);
    }
    if (err != PBIO_SUCCESS) {
        output_motor_error(err, "Motor::run_target(%f, %f) init failed", speed, target_angle);
        return err;
    }

    if (wait) {
        err = wait_for_completion(cancel_token);
        if (err != PBIO_SUCCESS) {
            output_motor_error(err, "Motor::run_target(%f, %f) movement failed", speed, target_angle);
            return err;
        }
    }

    return PBIO_SUCCESS;
}

/** 
Tracks a target angle. 

This is similar to run_target(), but the usual smooth acceleration is skipped: it will move 
to the target angle as fast as possible. This method is useful if you want to continuously 
change the target angle.

:param target_angle: Target angle that the motor should rotate to in deg
*/
void Motor::track_target(float target_angle) {
    if (xSemaphoreTake(_xMutex, portMAX_DELAY)) {
        pbio_servo_track_target(&_servo, target_angle);
        xSemaphoreGive(_xMutex);
    }
}

/**
Wait until current movement is completed or fails
*/
pbio_error_t Motor::wait_for_completion(CancelToken* cancel_token) {
    bool wait;
    pbio_error_t status;
    CancelToken localToken;
    if (cancel_token == nullptr) {
        // If no cancel token is provided, create a local cancel token in order to be able 
        // to stop the movement when a global cancel is raised
        cancel_token = &localToken;
    }

    while (true) {
        IF_CANCELLED(*cancel_token, {
            // If the movement is canceled, stop the motor
            stop();
            return PBIO_ERROR_CANCELED;
        });

        if (xSemaphoreTake(_xMutex, portMAX_DELAY)) {
            status = _servo_status;
            wait = _servo_status == PBIO_SUCCESS && !pbio_control_is_done(&_servo.control);
            xSemaphoreGive(_xMutex);
        }
            
        if (wait)
            delay(5);
        else
            break;
    }

    return status;
}

/**
Return max speed control limit in user units

:return: Return max speed (user unit/s)
*/
float Motor::get_speed_limit() {
    float _speed;
    if (xSemaphoreTake(_xMutex, portMAX_DELAY)) {
        _speed = pbio_control_settings_get_speed_limit(&_servo.control.settings);
        xSemaphoreGive(_xMutex);
    }

    return _speed;
}

/**
Return max acceleration control limit in user units

:return: Return max acceleration (user unit/s^2)
*/
float Motor::get_acceleration_limit() {
    float _acceleration;
    if (xSemaphoreTake(_xMutex, portMAX_DELAY)) {
        _acceleration = pbio_control_settings_get_acceleration_limit(&_servo.control.settings);
        xSemaphoreGive(_xMutex);
    }

    return _acceleration;
}

/**
Return actuation control limit in user units

:return: Return max actuation percentage (0-100%)
 */
uint8_t Motor::get_actuation_limit() {
    int32_t _actuation;
    if (xSemaphoreTake(_xMutex, portMAX_DELAY)) {
        _actuation = pbio_control_settings_get_actuation_limit(&_servo.control.settings);
        xSemaphoreGive(_xMutex);
    }

    return (uint8_t)_actuation;
}

/**
Set speed limit in user units

:param speed: Maximum speed (user units/s)
*/
pbio_error_t Motor::set_speed_limit(float speed) {
    pbio_error_t err;

    if (xSemaphoreTake(_xMutex, portMAX_DELAY)) {
        err = pbio_control_settings_set_speed_limit(&_servo.control.settings, speed);
        xSemaphoreGive(_xMutex);
    }
    if (err != PBIO_SUCCESS) {            
        output_motor_error(err, "Motor::set_speed_limits(%f) override limit failed", speed);

        return err;
    }

    return PBIO_SUCCESS;
}

/**
Set acceleration limit in user units

:param acceleration: Maximum acceleration (user units/s^2)
*/
pbio_error_t Motor::set_acceleration_limit(float acceleration) {
    pbio_error_t err;

    if (xSemaphoreTake(_xMutex, portMAX_DELAY)) {
        err = pbio_control_settings_set_acceleration_limit(&_servo.control.settings, acceleration);
        xSemaphoreGive(_xMutex);
    }
    if (err != PBIO_SUCCESS) {            
        output_motor_error(err, "Motor::set_acceleration_limit(%f) override limit failed", acceleration);

        return err;
    }

    return PBIO_SUCCESS;
}

/**
Set actuation limit in percentage

:param actuation: Maximum actuation percentage (0 to 100%)
*/
pbio_error_t Motor::set_actuation_limit(uint8_t actuation) {
    pbio_error_t err;

    if (xSemaphoreTake(_xMutex, portMAX_DELAY)) {
        err = pbio_control_settings_set_actuation_limit(&_servo.control.settings, (int32_t)actuation);
        xSemaphoreGive(_xMutex);
    }
    if (err != PBIO_SUCCESS) {            
        output_motor_error(err, "Motor::set_actuation_limit(%u) override limit failed", actuation);

        return err;
    }

    return PBIO_SUCCESS;
}

/**
Return pid settings

:param pid_kp: Return PID Kp
:param pid_ki: Return PID Ki
:param pid_kd: Return PID Kd
:param integral_range: Return integral range: Region around the target count in which integral errors are accumulated (user units)
:param integral_rate: Return integral rate: Maximum rate at which the integrator is allowed to increase  (user units/s)
 */
void Motor::get_pid(uint16_t *kp, uint16_t *ki, uint16_t *kd, float *integral_deadzone, float *integral_rate) const {
    int16_t _kp, _ki, _kd;
    int32_t _control_offset;
    if (xSemaphoreTake(_xMutex, portMAX_DELAY)) {
        pbio_control_settings_get_pid(&_servo.control.settings, &_kp, &_ki, &_kd, integral_deadzone, integral_rate, &_control_offset);
        xSemaphoreGive(_xMutex);
    }
    *kp = (uint16_t)_kp;
    *ki = (uint16_t)_ki;
    *kd = (uint16_t)_kd;
}

/**
Set PID settings

:param pid_kp: PID Kp
:param pid_ki: PID Ki
:param pid_kd: PID Kd
:param integral_range: Region around the target count in which integral errors are accumulated (user units)
:param integral_rate: Maximum rate at which the integrator is allowed to increase  (user units/s)
*/
pbio_error_t Motor::set_pid(uint16_t kp, uint16_t ki, uint16_t kd, float integral_deadzone, float integral_rate) {
    pbio_error_t err;

    if (integral_deadzone <= 0) {
        err = PBIO_ERROR_INVALID_ARG;
        output_motor_error(err, "Motor::set_pid(%u, %u, %u, %f, %f) integral_deadzone out of range", kp, ki, kd, integral_deadzone, integral_rate);
        return err;
    }

    if (integral_rate <= 0) {
        err = PBIO_ERROR_INVALID_ARG;
        output_motor_error(err, "Motor::set_pid(%u, %u, %u, %f, %f) integral_rate out of range", kp, ki, kd, integral_deadzone, integral_rate);
        return err;
    }

    int16_t _kp, _ki, _kd;
    int32_t _control_offset;
    float _integral_deadzone, _integral_rate;

    if (xSemaphoreTake(_xMutex, portMAX_DELAY)) {
        pbio_control_settings_get_pid(&_servo.control.settings, &_kp, &_ki, &_kd, &_integral_deadzone, &_integral_rate, &_control_offset);
        _kp = (int16_t)kp;
        _ki = (int16_t)ki;
        _kd = (int16_t)kd;
        err = pbio_control_settings_set_pid(&_servo.control.settings, _kp, _ki, kd, integral_deadzone, integral_rate, _control_offset);
        xSemaphoreGive(_xMutex);
    }
    if (err != PBIO_SUCCESS) {
        err = PBIO_ERROR_INVALID_ARG;
        output_motor_error(err, "Motor::set_pid(%u, %u, %u, %f, %f) sert failed", kp, ki, kd, integral_deadzone, integral_rate);
        return err;
    }

    return PBIO_SUCCESS;
}

/**
Return position and speed tolerance in user units to consider the movement done

:param speed: Return speed tolerance
:param position: Return position tolerance 
*/
void Motor::get_target_tolerances(float *speed, float *position) const {
    if (xSemaphoreTake(_xMutex, portMAX_DELAY)) {
        pbio_control_settings_get_target_tolerances(&_servo.control.settings, speed, position);
        xSemaphoreGive(_xMutex);
    }
}

/**
Set position and speed tolerance in user units to consider the movement done

:param speed: Speed tolerance in user units
:param position: Position tolerance in user units
 */
pbio_error_t Motor::set_target_tolerances(float speed, float position) {
    pbio_error_t err;

    if (speed <= 0) {
        err = PBIO_ERROR_INVALID_ARG;
        output_motor_error(err, "Motor::set_target_tolerances(%f, %f) speed out of range", speed, position);
        return err;
    }

    if (position <= 1) {
        err = PBIO_ERROR_INVALID_ARG;
        output_motor_error(err, "Motor::set_target_tolerances(%f, %f) position out of range", speed, position);            
        return err;
    }

    if (xSemaphoreTake(_xMutex, portMAX_DELAY)) {
        err = pbio_control_settings_set_target_tolerances(&_servo.control.settings, speed, position);
        xSemaphoreGive(_xMutex);
    }
    if (err != PBIO_SUCCESS) {
        output_motor_error(err, "Motor::set_target_tolerances(%f, %f) set failed", speed, position);            
        return err;
    }

    return PBIO_SUCCESS;
}

/**
Return stall tolerances in user units

:param speed: Return speed tolerance (user unit)
:param time: Reutrn time tolerance (ms)
*/
void Motor::get_stall_tolerances(float *speed, uint32_t *time_ms) const {
    int32_t _time_ms;
    if (xSemaphoreTake(_xMutex, portMAX_DELAY)) {
        pbio_control_settings_get_stall_tolerances(&_servo.control.settings, speed, &_time_ms);
        xSemaphoreGive(_xMutex);
    }
    *time_ms = _time_ms;
}

/**
 Set stall tolerances in user units

:param speed: Speed tolerance in user units
:param time: Time tolerance in ms
*/
pbio_error_t Motor::set_stall_tolerances(float speed, uint32_t time_ms) {
    pbio_error_t err;

    if (xSemaphoreTake(_xMutex, portMAX_DELAY)) {
        err = pbio_control_settings_set_stall_tolerances(&_servo.control.settings, speed, (int32_t)time_ms);
        xSemaphoreGive(_xMutex);
    }
    if (err != PBIO_SUCCESS) {
        output_motor_error(err, "Motor::set_stall_tolerances(%f, %u) set failed", speed, time_ms);            
        return err;
    }
    
    return PBIO_SUCCESS;
}

/**
 * Prints an error message to the serial output.
 * @param [in]  err     The error code
 * @param [in]  format  The format string for the error message
 * @param [in]  ...     Additional arguments for the format string
 */
void Motor::output_motor_error(pbio_error_t err, const char* format, ...) {
    if (err != PBIO_SUCCESS) {
        va_list args;
        va_start(args, format);

        char buffer[1024];
        vsnprintf(buffer, sizeof(buffer), format, args);
        va_end(args);

        String err_message = String(_name) +" " + buffer;
        
        if (_current_error_output_func) {
            _current_error_output_func(err, pbio_error_str(err), err_message.c_str());
        } else {
            Serial.print("Motor Error: ");
            Serial.print(pbio_error_str(err));
            Serial.print(" -> ");
            Serial.println(err_message.c_str());
        }
    }
}

void Motor::update() {
    if (xSemaphoreTake(_xMutex, portMAX_DELAY)) {
        _servo_status = pbio_servo_control_update(&_servo);
        xSemaphoreGive(_xMutex);
    }
}