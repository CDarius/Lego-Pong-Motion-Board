
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

IMotorHoming* EncoderJog::getMotor() const {
    return motor;
}

void EncoderJog::start(IMotorHoming& motor) {
    this->motor = &motor;
    pos_setpoint = this->motor->angle();
    last_encoder_value = 0;
    encoder.clearValue();
    last_update_us = monotonic_us();
}

void EncoderJog::stop() {
    if (motor) {
        motor->stop();
    }
    motor = nullptr;
}

void EncoderJog::update() {
    if (!motor) {
        return;
    }

    // Run only every update_interval_us
    uint64_t now_us = monotonic_us();
    if (now_us - last_update_us < update_interval_us) {
        return;
    }
    last_update_us = now_us;

    // Detect a change in jog encoder value
    int16_t encoder_value = encoder.getValue();
    int16_t new_step = encoder_value - last_encoder_value;
        
    if (new_step == 0) {
        // No movement requested
        return;
    }

    float new_position = pos_setpoint + ((float)new_step) * encoder_multiplier;;

    // Clamp the new position to the motor's software limits if referenced
    if (motor->referenced()) {
        if (new_position < motor->getSwLimitMinus())
            new_position = motor->getSwLimitMinus();
        else if (new_position > motor->getSwLimitPlus())
            new_position = motor->getSwLimitPlus();
    }

    // Track the target position
    motor->track_target(new_position);
    pos_setpoint = new_position;
    last_encoder_value = encoder_value;
}
