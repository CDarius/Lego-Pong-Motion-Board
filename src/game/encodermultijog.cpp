#include "game/encodermultijog.hpp"

void EncoderMultiJog::applyConfigForAxis(Axes axis) {
    encoder_jog_config_t* config = _encoderJog.getConfig();
    switch (axis) {
        case Axes::X:
            config->encoder_multiplier = _config.x_encoder_multiplier;
            config->use_track = _config.x_use_track;
            config->encoder_invert = false;
            break;
        case Axes::Y:
            config->encoder_multiplier = _config.y_encoder_multiplier;
            config->use_track = _config.y_use_track;
            config->encoder_invert = false;
            break;
        case Axes::L:
            config->encoder_multiplier = _config.l_r_encoder_multiplier;
            config->use_track = _config.l_r_use_track;
            config->encoder_invert = _config.l_encoder_invert;
            break;
        case Axes::R:
            config->encoder_multiplier = _config.l_r_encoder_multiplier;
            config->use_track = _config.l_r_use_track;
            config->encoder_invert = _config.r_encoder_invert;
            break;
    }
    config->update_interval_ms = _config.update_interval_ms;
    config->medium_enc_speed_count = _config.medium_enc_speed_count;
    config->high_enc_speed_count = _config.high_enc_speed_count;
    config->medium_speed_multiplier_factor = _config.medium_speed_multiplier_factor;
    config->high_speed_multiplier_factor = _config.high_speed_multiplier_factor;
}

void EncoderMultiJog::start(Axes axis) {
    IMotorHoming* targetAxis = nullptr;

    // Get the new axis and its multiplier
    switch (axis) {
        case Axes::X:
            targetAxis = &_xAxis;
            break;
        case Axes::Y:
            targetAxis = &_yAxis;
            break;
        case Axes::L:
            targetAxis = &_lAxis;
            break;
        case Axes::R:
            targetAxis = &_rAxis;
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
        applyConfigForAxis(axis);
        _encoderJog.start(*targetAxis);
        _last_update_counter = _config.update_counter;
    }
}

void EncoderMultiJog::stop() {
    _encoderJog.stop();
}

void EncoderMultiJog::update() {
    if (_config.update_counter != _last_update_counter) {
        // The config has changed, re-apply it to the current axis
        IMotorHoming* currentMotor = _encoderJog.getMotor();
        if (currentMotor) {
            if (currentMotor == &_xAxis) {
                applyConfigForAxis(Axes::X);
            } else if (currentMotor == &_yAxis) {
                applyConfigForAxis(Axes::Y);
            } else if (currentMotor == &_lAxis) {
                applyConfigForAxis(Axes::L);
            } else if (currentMotor == &_rAxis) {
                applyConfigForAxis(Axes::R);
            }
        }
        _last_update_counter = _config.update_counter;
    }

    _encoderJog.update();
}