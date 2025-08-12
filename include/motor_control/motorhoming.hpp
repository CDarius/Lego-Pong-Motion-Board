#pragma once

#include "motor_control/error.hpp"
#include "utils/cancel_token.hpp"

class IMotorHoming : public Motor {
public:
    virtual ~IMotorHoming() = default;
    virtual bool referenced() const = 0;
    virtual pbio_error_t run_axis_homing(CancelToken& cancel_token) = 0;
};
