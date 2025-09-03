#include "game/encodermultijog.hpp"

UnitEncoder* EncoderMultiJog::getEncoder() const {
    return &_unitEncoder;
}

IMotorHoming* EncoderMultiJog::getMotor() const {
    return _encoderJog.getMotor();
}

float EncoderMultiJog::getEncoderMultiplier() const {
    return _encoderJog.getEncoderMultiplier();
}

void EncoderMultiJog::setEncoderMultiplier(float multiplier) {
    _encoderJog.setEncoderMultiplier(multiplier);
}

void EncoderMultiJog::start(Axes axis) {
    IMotorHoming* targetAxis = nullptr;
    float multiplier = 1.0f;

    // Get the new axis and its multiplier
    switch (axis) {
        case Axes::X:
            targetAxis = &_xAxis;
            multiplier = _config.x_encoder_multiplier;
            break;
        case Axes::Y:
            targetAxis = &_yAxis;
            multiplier = _config.y_encoder_multiplier;
            break;
        case Axes::L:
            targetAxis = &_lAxis;
            multiplier = _config.l_r_encoder_multiplier;
            break;
        case Axes::R:
            targetAxis = &_rAxis;
            multiplier = _config.l_r_encoder_multiplier;
            break;
    }

    IMotorHoming* currentMotor = _encoderJog.getMotor();
    if (currentMotor && currentMotor == targetAxis) {
        // If we are already jogging the target axis, do nothing
        return;
    }
    if (currentMotor && currentMotor != targetAxis) {
        // If we are switching motors, stop the current axis first
        _encoderJog.stop();
    }

    if (targetAxis) {
        _encoderJog.setUpdateIntervalMs(_config.update_interval_ms);
        _encoderJog.setEncoderMultiplier(multiplier);
        _encoderJog.start(*targetAxis);
    }
}

void EncoderMultiJog::stop() {
    _encoderJog.stop();
}

void EncoderMultiJog::update() {
    _encoderJog.update();
}