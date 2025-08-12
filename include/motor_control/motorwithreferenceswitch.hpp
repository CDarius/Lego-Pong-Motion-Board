#pragma once

#include <Arduino.h>
#include "motor.hpp"
#include "motorhoming.hpp"
#include "error.hpp"
#include "utils/logger.hpp"
#include "utils/cancel_token.hpp"

typedef struct {
    bool start_in_positive_direction; // If true, start homing in the positive direction
    float speed; // Speed to use during homing (motor units/second)
    float minimum_travel; // Minimum required travel distance before hitting the switch (motor units)
    float retract_distance; // Distance to retract after hitting the home switch (motor units)
    float switch_axis_position; // Axis position to set when hit the switch (motor units)
} switch_homing_config_t;


// MotorWithReferenceSwitch: Inherits from Motor, adds reference switch logic
class MotorWithReferenceSwitch : public IMotorHoming {
    private:
        bool _referenced = false;
        const uint8_t _home_switch_pin;
        const int _switch_pressed_value;
        switch_homing_config_t& _config;

    public:
        MotorWithReferenceSwitch(uint8_t home_switch_pin, int switch_pressed_value, switch_homing_config_t& config)
            : _home_switch_pin(home_switch_pin),
            _switch_pressed_value(switch_pressed_value),
            _config(config)
        {}

        // Read-only property: referenced
        bool referenced() const { return _referenced; }

        // Read-only property: config
        switch_homing_config_t* config() const { return &_config; }

        // Run axis homing procedure using a reference switch
        pbio_error_t run_axis_homing(CancelToken& cancel_token);
};
