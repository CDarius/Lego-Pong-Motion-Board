#ifndef __MOTOR_HOMING_HPP__
#define __MOTOR_HOMING_HPP__

#include <Arduino.h>
#include "motor.hpp"
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

pbio_error_t home_with_switch(Motor& motor, uint8_t home_switch_pin, int switch_pressed_value, switch_homing_config_t& config);

#endif