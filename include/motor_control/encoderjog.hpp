
#pragma once
#include "motorhoming.hpp"
#include "devices/unit_encoder.hpp"

// EncoderJog provides jog control for a motor using an external encoder as input.
class EncoderJog {
private:
    // Axis position update interval in milliseconds
    int update_interval_ms;
    // Multiplier for encoder value to convert from encoder units to motor position units
    float encoder_multiplier;
    // Position setpoint for the axis
    float pos_setpoint = 0.0f;
    // Direction
    bool invert_encoder;
    // Last jog wheel value
    int16_t last_encoder_value = 0;
    IMotorHoming* motor = nullptr;
    UnitEncoder& encoder;
    uint32_t last_update_ms = 0;

public:
    EncoderJog(UnitEncoder& encoder, int interval_ms = 200, float multiplier = 1.0f)
        : encoder(encoder), update_interval_ms(interval_ms), encoder_multiplier(multiplier) {}

    // Get the axis position update interval in milliseconds
    int getUpdateIntervalMs() const;
    // Set the axis position update interval in milliseconds
    void setUpdateIntervalMs(int interval_ms);

    // Get the encoder multiplier to convert encoder units to motor position units
    float getEncoderMultiplier() const;
    // Set the encoder multiplier to convert encoder units to motor position units
    void setEncoderMultiplier(float multiplier);

    // Get the encoder invert status
    bool getEncoderInvert() const;
    // Set the encoder invert status
    void setEncoderInvert(bool invert);

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

