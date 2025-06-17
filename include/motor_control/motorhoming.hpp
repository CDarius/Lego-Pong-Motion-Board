#ifndef __MOTOR_HOMING_HPP__
#define __MOTOR_HOMING_HPP__

#include "motor.hpp"

typedef struct {
    bool start_in_positive_direction; // If true, start homing in the positive direction
    float speed; // Speed to use during homing (motor units/second)
    float minimum_travel; // Minimum required travel distance before hitting the switch (motor units)
    float retract_distance; // Distance to retract after hitting the home switch (motor units)
    float switch_axis_position; // Axis position to set when hit the switch (motor units)
} switch_homing_config_t;

void home_with_switch(Motor& motor, switch_homing_config_t& config);

#endif