#pragma once

#include <Arduino.h>
#include "axes.hpp"
#include "devices/unit_encoder.hpp"
#include "motor_control/encoderjog.hpp"
#include "motor_control/motorhoming.hpp"

struct encoder_multi_jog_config_t  {    
    uint16_t update_interval_ms; // Axis position update interval in milliseconds
    float x_encoder_multiplier; // X-Axis multiplier for encoder value to convert from encoder units to motor position units (stud/count)
    float y_encoder_multiplier; // Y-Axis multiplier for encoder value to convert from encoder units to motor position units (stud/count)
    float l_r_encoder_multiplier; // L-Axis and R-Axis multiplier for encoder value to convert from encoder units to motor position units (stud/count)
    bool l_encoder_invert; // L-Axis encoder direction invert
    bool r_encoder_invert; // R-Axis encoder direction invert
};

class EncoderMultiJog {
    private:
        UnitEncoder& _unitEncoder;
        IMotorHoming& _xAxis;
        IMotorHoming& _yAxis;
        IMotorHoming& _lAxis;
        IMotorHoming& _rAxis;
        encoder_multi_jog_config_t& _config;
        EncoderJog _encoderJog = EncoderJog(_unitEncoder);

    public:
        EncoderMultiJog(UnitEncoder& unitEncoder, encoder_multi_jog_config_t& config, IMotorHoming& xAxis, IMotorHoming& yAxis, IMotorHoming& lAxis, IMotorHoming& rAxis)
            : _unitEncoder(unitEncoder), _config(config), _xAxis(xAxis), _yAxis(yAxis), _lAxis(lAxis), _rAxis(rAxis) {}

        // Get the encoder reference for jog control
        UnitEncoder* getEncoder() const;

        // Get the current jogged motor
        IMotorHoming* getMotor() const;

        // Get the current encoder multiplier to convert encoder units to motor position units
        float getEncoderMultiplier() const;
        // Set the current encoder multiplier to convert encoder units to motor position units
        void setEncoderMultiplier(float multiplier);

        // Start jog control for the specified axis
        void start(Axes axis);

        // Stop jog control and clear motor pointers
        void stop();

        // Update jog control. Runs only if enough time has passed since last update.
        // Moves the motor by the encoder value scaled by encoder_multiplier at axis_speed.
        void update();
};
