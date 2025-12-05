
#include "motor_control/encoderjog.hpp"
#include "monotonic.h"
#include "macros.h"

void EncoderJog::start(IMotorHoming& motor) {
    if (xSemaphoreTake(_xMutex, portMAX_DELAY)) {
        this->motor = nullptr;
        pos_setpoint = motor.angle();
        last_encoder_value = 0;
        while (!encoder.clearValue()) {
            delay(PBIO_CONFIG_SERVO_PERIOD_MS);
        }
        last_update_ms = millis();
        this->motor = &motor;
        xSemaphoreGive(_xMutex);
    }
}

void EncoderJog::stop() {
    if (xSemaphoreTake(_xMutex, portMAX_DELAY)) {
        if (motor) {
            motor->stop();
        }
        motor = nullptr;
        xSemaphoreGive(_xMutex);
    }
}

void EncoderJog::update() {
    if (xSemaphoreTake(_xMutex, portMAX_DELAY)) {
        if (!motor) {
            xSemaphoreGive(_xMutex);
            return;
        }

        // Run only every update_interval_ms
        uint32_t now_ms = millis();
        if (now_ms - last_update_ms < _config.update_interval_ms) {
            xSemaphoreGive(_xMutex);
            return;
        }
        last_update_ms = now_ms;

        // Read actual jog encoder value
        int16_t encoder_value;
        bool readSuccess = encoder.getValue(encoder_value);
        if (!readSuccess) {
            xSemaphoreGive(_xMutex);

            // Failed to read the encoder
            return;
        }
        if (_config.encoder_invert)
            encoder_value = -encoder_value;        

        // Detect a change in jog encoder value
        int16_t new_step = encoder_value - last_encoder_value;            
        if (new_step == 0) {
            xSemaphoreGive(_xMutex);

            // No movement requested
            return;
        }

        // Calculate the encoder wheel speed multiplier factor
        float enc_speed_multiplier = 1.0f;
        if (abs(new_step) >= _config.high_enc_speed_count) {
            enc_speed_multiplier = _config.high_speed_multiplier_factor;
        } else if (abs(new_step) >= _config.medium_enc_speed_count) {
            enc_speed_multiplier = _config.medium_speed_multiplier_factor;
        }

        // Calculate the new target position
        float new_position = pos_setpoint + ((float)new_step) * _config.encoder_multiplier * enc_speed_multiplier;

        // Clamp the new position to the motor's software limits if referenced
        if (motor->referenced()) {
            float swLimitM = motor->getSwLimitMinus();
            float swLimitP = motor->getSwLimitPlus();
            new_position = MIN(MAX(new_position, swLimitM), swLimitP);
        }

        // Move the axis
        if (_config.use_track) {
            motor->track_target(new_position);
        } else {
            motor->run_target(motor->get_speed_limit(), new_position, PBIO_ACTUATION_HOLD, false);
        }

        // Save the new position and encoder value
        pos_setpoint = new_position;
        last_encoder_value = encoder_value;

        xSemaphoreGive(_xMutex);
    }
}

void EncoderJog::overridePosSetpoint(float pos) {
     if (xSemaphoreTake(_xMutex, portMAX_DELAY)) {
        pos_setpoint = pos;
        motor->track_target(pos);
        xSemaphoreGive(_xMutex);
    }
}