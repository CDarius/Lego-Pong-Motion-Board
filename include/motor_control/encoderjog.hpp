
#pragma once
#include "motorhoming.hpp"
#include "devices/unit_encoder.hpp"

struct encoder_jog_config_t  {    
    uint16_t update_interval_ms; // Axis position update interval in milliseconds
    float encoder_multiplier; // Axis multiplier for encoder value to convert from encoder units to motor position units (stud/count)
    bool use_track; // Use track (true) or run_target (false) to move the axis
    bool encoder_invert; // Encoder direction invert
    uint8_t medium_enc_speed_count; // Number of encoder counts to consider it medium speed
    uint8_t high_enc_speed_count; // Number of encoder counts to consider it high speed
    float medium_speed_multiplier_factor; // Multiplier to apply to encoder_multiplier when reach medium speed
    float high_speed_multiplier_factor; // Multiplier to apply to encoder_multiplier when reach high speed
};

// EncoderJog provides jog control for a motor using an external encoder as input.
class EncoderJog {
private:
    // Configuration for the jog control
    encoder_jog_config_t _config;

    // Position setpoint for the axis
    float pos_setpoint = 0.0f;
    // Last jog wheel value
    int16_t last_encoder_value = 0;
    // Last position update time in milliseconds
    uint32_t last_update_ms = 0;

    SemaphoreHandle_t _xMutex = xSemaphoreCreateMutex();
    
    IMotorHoming* motor = nullptr;
    UnitEncoder& encoder;

public:
    EncoderJog(UnitEncoder& encoder, int interval_ms = 200, float multiplier = 1.0f) : encoder(encoder) {
        _config.update_interval_ms = interval_ms;
        _config.encoder_multiplier = multiplier;

        _config.use_track = true;
        _config.medium_enc_speed_count = 1;
        _config.high_enc_speed_count = 1;
        _config.medium_speed_multiplier_factor = 1.0f;
        _config.high_speed_multiplier_factor = 1.0f;
    }

    // Configuration access
    encoder_jog_config_t* getConfig() {
        return &_config;
    }

    // Get the encoder reference for jog control
    UnitEncoder* getEncoder() const {
        return &encoder;
    }

    // Get the current jogged motor
    IMotorHoming* getMotor() const {
        return motor;
    }

    // Get the current axis position setpoint
    float getPosSetpoint() const {
        return pos_setpoint;
    }

    // Override current axis jog position setpoint
    void overridePosSetpoint(float pos);

    // Start jog control with the given motor
    void start(IMotorHoming& motor);

    // Stop jog control and clear motor pointers
    void stop();

    // Update jog control. Runs only if enough time has passed since last update.
    // Moves the motor by the encoder value scaled by encoder_multiplier at axis_speed.
    void update();
};

