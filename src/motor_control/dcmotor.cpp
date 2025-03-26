#include "motor_control/dcmotor.hpp"

void DCMotor::begin(uint8_t pwmPin1, uint8_t pwmPin2, pbio_direction_t direction, uint16_t userPwmMax) {
    _pwmPin1 = pwmPin1;
    _pwmPin2 = pwmPin2;
    _direction = direction;
    _userPwmMax = userPwmMax;

    bool success = ledcAttach(pwmPin1, MOTOR_PWM_FREQUENCY, MOTOR_PWM_RESOLUTION);
    success = ledcAttach(pwmPin2, MOTOR_PWM_FREQUENCY, MOTOR_PWM_RESOLUTION);
    if (!success)
        throw -1;

    coast();
}

void DCMotor::getState(pbio_passivity_t *state, int32_t *duty_now) {
    *state = _state;
    *duty_now = _duty_now;
}

uint16_t DCMotor::getUserPwmMax() {
    return _userPwmMax;
}

void DCMotor::coast() {
    _state = PBIO_DCMOTOR_COAST;
    set_duty_cycle(0);
}

void DCMotor::brake() {
    _state = PBIO_DCMOTOR_BRAKE;
    set_duty_cycle(MOTOR_PWM_MAX_VALUE);
}

void DCMotor::set_duty_cycle(int32_t duty_steps) {
    _state = PBIO_DCMOTOR_BRAKE;
    _duty_now = duty_steps;

    duty_steps = duty_steps * MOTOR_PWM_MAX_VALUE / _userPwmMax;

    if (duty_steps > MOTOR_PWM_MAX_VALUE)
        duty_steps = MOTOR_PWM_MAX_VALUE;
    if (duty_steps < -MOTOR_PWM_MAX_VALUE)
        duty_steps = -MOTOR_PWM_MAX_VALUE;

    if (_direction == PBIO_DIRECTION_COUNTERCLOCKWISE)
        duty_steps = -duty_steps;

    if (duty_steps >= 0) {
        ledcWrite(_pwmPin1, duty_steps);
        ledcWrite(_pwmPin2, 0);
    } else {
        ledcWrite(_pwmPin1, 0);
        ledcWrite(_pwmPin2, -duty_steps);
    }
}
