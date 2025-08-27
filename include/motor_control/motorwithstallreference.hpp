#pragma once

#include <Arduino.h>
#include "motor.hpp"
#include "motorhoming.hpp"
#include "error.hpp"
#include "utils/logger.hpp"
#include "utils/cancel_token.hpp"

struct stall_homing_config_t : public base_homing_config_t {
    bool start_in_positive_direction; // If true, start homing in the positive direction
    float speed; // Speed to use during homing (motor units/second)
    float duty_limit; // Duty cycle limit to detect stall on the reference obstacle (0.0% to 100.0%)
    float minimum_travel; // Minimum required travel distance before hitting the switch (motor units)
    float retract_distance; // Distance to retract after hitting the home switch (motor units)
};

class MotorWithStallReference : public IMotorHoming {
    private:
        bool _referenced = false;
        stall_homing_config_t& _config;

    public:
        MotorWithStallReference(stall_homing_config_t& config): _config(config) {}

        bool referenced() const override { return _referenced; }

        stall_homing_config_t* config() const { return &_config; }
        base_homing_config_t* base_config() const override { return &_config; }

        // Run axis homing procedure using a reference switch
        pbio_error_t run_axis_homing(CancelToken& cancel_token) override;
};
