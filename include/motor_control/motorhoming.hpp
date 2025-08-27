#pragma once

#include "motor.hpp"
#include "motor_control/error.hpp"
#include "utils/cancel_token.hpp"

struct base_homing_config_t {
    float axis_position_at_home_marker = 0.0;   // Axis position at home marker: switch, stop, etc.. (studs)
};

class IMotorHoming : public Motor {
    private:

    public:
        virtual ~IMotorHoming() = default;
        virtual bool referenced() const = 0;
        virtual pbio_error_t run_axis_homing(CancelToken& cancel_token) = 0;

        virtual base_homing_config_t* base_config() const = 0;
};
