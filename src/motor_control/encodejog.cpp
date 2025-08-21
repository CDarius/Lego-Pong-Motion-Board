
#include "motor_control/encoderjog.hpp"
#include "monotonic.h"

int EncoderJog::getUpdateIntervalMs() const {
    return update_interval_us / 1000;
}

void EncoderJog::setUpdateIntervalMs(int interval_ms) {
    update_interval_us = interval_ms * 1000;
}

float EncoderJog::getEncoderMultiplier() const {
    return encoder_multiplier;
}

void EncoderJog::setEncoderMultiplier(float multiplier) {
    encoder_multiplier = multiplier;
}

UnitEncoder* EncoderJog::getEncoder() const {
    return &encoder;
}

void EncoderJog::start(Motor& motor) {
    this->motor = &motor;
    axis_speed = this->motor->get_speed_limit();
    last_update_us = monotonic_us();
    encoder.clearValue();
}

void EncoderJog::stop() {
    motor = nullptr;
}

void EncoderJog::update() {
    if (!motor) {
        return;
    }
    uint64_t now_us = monotonic_us();
    if (now_us - last_update_us < update_interval_us) {
        return;
    }
    last_update_us = now_us;
    float new_position = motor->angle() + encoder.getValue() * encoder_multiplier;
    encoder.clearValue();
    motor->run_target(axis_speed, new_position, PBIO_ACTUATION_HOLD, false);
}
