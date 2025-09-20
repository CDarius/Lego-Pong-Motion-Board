#pragma once

#include <Arduino.h>
#include "const.h"

/**
 * Control settings
 */
typedef struct _pbio_control_settings_t {
    float counts_per_unit;          /**< Conversion between user units (degree, mm, etc) and integer counts used internally by controller */
    int32_t stall_rate_limit;       /**< If this speed cannnot be reached even with the maximum duty value (equal to stall_torque_limit), the motor is considered to be stalled */
    int32_t stall_time;             /**< Minimum stall time before the run_stalled action completes */
    int32_t max_rate;               /**< Soft limit on the reference encoder rate in all run commands */
    int32_t rate_tolerance;         /**< Allowed deviation (counts/s) from target speed. Hence, if speed target is zero, any speed below this tolerance is considered to be standstill. */
    int32_t count_tolerance;        /**< Allowed deviation (counts) from target before motion is considered complete */
    int32_t abs_acceleration;       /**< Encoder acceleration/deceleration rate when beginning to move or stopping. Positive value in counts per second per second */
    int16_t pid_kp;                 /**< Proportional position control constant (and integral speed control constant) */
    int16_t pid_ki;                 /**< Integral position control constant */
    int16_t pid_kd;                 /**< Derivative position control constant (and proportional speed control constant) */
    int32_t max_control;            /**< Upper limit on control output */
    int32_t control_offset;         /**< Constant feedforward signal added in the reference direction */
    int32_t actuation_scale;        /**< Number of "duty steps" per "%" user-specified raw actuation value */
    int32_t integral_range;         /**< Region around the target count in which integral errors are accumulated */
    int32_t integral_rate;          /**< Maximum rate at which the integrator is allowed to increase */
} pbio_control_settings_t;

static pbio_control_settings_t settings_servo_ev3_medium = {
    .stall_rate_limit = 30,
    .stall_time = 200*US_PER_MS,
    .max_rate = 2000,
    .rate_tolerance = 100,
    .count_tolerance = 10,
    .abs_acceleration = 8000,
    .pid_kp = 300,
    .pid_ki = 400,
    .pid_kd = 3,
    .max_control = 10000,
    .control_offset = 2000,
    .actuation_scale = 100,
    .integral_range = 45,
    .integral_rate = 10,
};

static pbio_control_settings_t settings_servo_ev3_large = {
    .stall_rate_limit = 30,
    .stall_time = 200*US_PER_MS,
    .max_rate = 1600,
    .rate_tolerance = 100,
    .count_tolerance = 10,
    .abs_acceleration = 3200,
    .pid_kp = 400,
    .pid_ki = 1200,
    .pid_kd = 5,
    .max_control = 10000,
    .control_offset = 0,
    .actuation_scale = 100,
    .integral_range = 45,
    .integral_rate = 10,
};
