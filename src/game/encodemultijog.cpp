#include "game/encodemultijog.hpp"

void EncoderMultiJog::start(Axes axis) {
    IMotorHoming* targetAxis = nullptr;
    float multiplier = 1.0f;

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

    if (targetAxis) {
        _encoderJog.setUpdateIntervalMs(_config.update_interval_ms);
        _encoderJog.setEncoderMultiplier(multiplier);
        _encoderJog.start(*targetAxis);
    }
}

UnitEncoder* EncoderMultiJog::getEncoder() const {
    return &_unitEncoder;
}

void EncoderMultiJog::stop() {
    _encoderJog.stop();
}

void EncoderMultiJog::update() {
    _encoderJog.update();
}