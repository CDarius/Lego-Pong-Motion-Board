#pragma once

#include <Arduino.h>
#include "motor.hpp"
#include "motorhoming.hpp"
#include "error.hpp"
#include "utils/logger.hpp"
#include "utils/cancel_token.hpp"

struct switch_homing_config_t : public base_homing_config_t  {
    uint8_t home_switch_pin; // I/O pin number where the home switch is connected
    int switch_pressed_value; // Value indicating the switch is pressed (LOW or HIGH)
    bool start_in_positive_direction; // If true, start homing in the positive direction
    float speed; // Speed to use during homing (motor units/second)
    float minimum_travel; // Minimum required travel distance before hitting the switch (motor units)
};

// MotorWithReferenceSwitch: Inherits from Motor, adds reference switch logic
class MotorWithReferenceSwitch : public IMotorHoming {
    private:
        bool _referenced = false;
        switch_homing_config_t& _config;

    public:
        MotorWithReferenceSwitch(switch_homing_config_t& config) : _config(config)
        {}

        bool referenced() const override { return _referenced; }

        switch_homing_config_t* config() const override { return &_config; }

        // Run axis homing procedure using a reference switch
        pbio_error_t run_axis_homing(CancelToken& cancel_token) override;
};
