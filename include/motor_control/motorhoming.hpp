#pragma once

#include "motor.hpp"
#include "motor_control/error.hpp"
#include "utils/cancel_token.hpp"

class IMotorHoming : public Motor {
    public:
        virtual ~IMotorHoming() = default;
        float axis_position_at_home_marker; // Axis position at home marker: switch, stop, etc.. (motor units)
        virtual bool referenced() const = 0;
        virtual pbio_error_t run_axis_homing(CancelToken& cancel_token) = 0;
};
