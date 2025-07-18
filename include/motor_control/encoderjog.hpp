
#pragma once
#include "motor_control/motor.hpp"
#include "devices/unit_encoder.hpp"

// EncoderJog provides jog control for a motor using an external encoder as input.
class EncoderJog {
private:
    // Axis position update interval in microseconds
    int update_interval_us;
    // Multiplier for encoder value to convert from encoder units to motor position units
    float encoder_multiplier;
    // Axis speed to use for jog movement
    float axis_speed = 0.0f;
    Motor* motor = nullptr;
    UnitEncoder* encoder = nullptr;
    uint64_t last_update_us = 0;

public:
    EncoderJog(int interval_ms = 200, float multiplier = 1.0f)
        : update_interval_us(interval_ms * 1000), encoder_multiplier(multiplier) {}

    // Get the axis position update interval in milliseconds
    int getUpdateIntervalMs() const;
    // Set the axis position update interval in milliseconds
    void setUpdateIntervalMs(int interval_ms);

    // Get the encoder multiplier to convert encoder units to motor position units
    float getEncoderMultiplier() const;
    // Set the encoder multiplier to convert encoder units to motor position units
    void setEncoderMultiplier(float multiplier);

    // Set the encoder reference for jog control
    void begin(UnitEncoder& encoder);
    // Start jog control with the given motor
    void start(Motor& motor);

    // Stop jog control and clear motor/encoder pointers
    void stop();

    // Update jog control. Runs only if enough time has passed since last update.
    // Moves the motor by the encoder value scaled by encoder_multiplier at axis_speed.
    void update();
};

