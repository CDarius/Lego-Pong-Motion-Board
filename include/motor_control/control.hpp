// SPDX-License-Identifier: MIT
// Copyright (c) 2018-2020 The Pybricks Authors

#ifndef _PBIO_CONTROL_H_
#define _PBIO_CONTROL_H_

#include <Arduino.h>
#include "motor_control/const.h"
#include "motor_control/config.h"
#include "motor_control/enums.h"
#include "motor_control/error.hpp"
#include "motor_control/controlsettings.h"
#include "motor_control/trajectory.hpp"
#include "motor_control/integrator.hpp"

// Maneuver-specific function that returns true if maneuver is done, based on current state
typedef bool (*pbio_control_on_target_t)(pbio_trajectory_t *trajectory,
                                         pbio_control_settings_t *settings,
                                         int32_t time,
                                         int32_t count,
                                         int32_t rate,
                                         bool stalled);

// Functions to check whether motion is done
extern pbio_control_on_target_t pbio_control_on_target_always;
extern pbio_control_on_target_t pbio_control_on_target_never;
extern pbio_control_on_target_t pbio_control_on_target_angle;
extern pbio_control_on_target_t pbio_control_on_target_time;
extern pbio_control_on_target_t pbio_control_on_target_stalled;

typedef enum {
    PBIO_CONTROL_NONE,   /**< No control */
    PBIO_CONTROL_TIMED,  /**< Run for a given amount of time */
    PBIO_CONTROL_ANGLE,  /**< Run to an angle */
} pbio_control_type_t;

typedef struct _pbio_control_t {
    pbio_control_type_t type;
    pbio_control_settings_t settings;
    pbio_actuation_t after_stop;
    pbio_trajectory_t trajectory;
    pbio_rate_integrator_t rate_integrator;
    pbio_count_integrator_t count_integrator;
    pbio_control_on_target_t on_target_func;
    bool stalled;
    bool on_target;
} pbio_control_t;

// Convert control units (counts, rate) and physical user units (deg or mm, deg/s or mm/s)
float pbio_control_counts_to_user(pbio_control_settings_t *s, int32_t counts);
int32_t pbio_control_user_to_counts(pbio_control_settings_t *s, float user);

void pbio_control_settings_get_limits(pbio_control_settings_t *s, float *speed, float *acceleration, int32_t *actuation);
pbio_error_t pbio_control_settings_set_limits(pbio_control_settings_t *ctl, float speed, float acceleration, int32_t actuation);

void pbio_control_settings_get_pid(pbio_control_settings_t *s, int16_t *pid_kp, int16_t *pid_ki, int16_t *pid_kd, float *integral_range, float *integral_rate, int32_t *control_offset);
pbio_error_t pbio_control_settings_set_pid(pbio_control_settings_t *s, int16_t pid_kp, int16_t pid_ki, int16_t pid_kd, float integral_range, float integral_rate, int32_t control_offset);

void pbio_control_settings_get_target_tolerances(pbio_control_settings_t *s, float *speed, float *position);
pbio_error_t pbio_control_settings_set_target_tolerances(pbio_control_settings_t *s, float speed, float position);

void pbio_control_settings_get_stall_tolerances(pbio_control_settings_t *s,  float *speed, int32_t *time);
pbio_error_t pbio_control_settings_set_stall_tolerances(pbio_control_settings_t *s, float speed, int32_t time);

int32_t pbio_control_settings_get_max_integrator(pbio_control_settings_t *s);
int32_t pbio_control_get_ref_time(pbio_control_t *ctl, int32_t time_now);

void pbio_control_stop(pbio_control_t *ctl);
pbio_error_t pbio_control_start_angle_control(pbio_control_t *ctl, int32_t time_now, int32_t count_now, int32_t target_count, int32_t rate_now, int32_t target_rate, int32_t acceleration, pbio_actuation_t after_stop);
pbio_error_t pbio_control_start_relative_angle_control(pbio_control_t *ctl, int32_t time_now, int32_t count_now, int32_t relative_target_count, int32_t rate_now, int32_t target_rate, int32_t acceleration, pbio_actuation_t after_stop);
pbio_error_t pbio_control_start_timed_control(pbio_control_t *ctl, int32_t time_now, int32_t duration, int32_t count_now, int32_t rate_now, int32_t target_rate, int32_t acceleration, pbio_control_on_target_t stop_func, pbio_actuation_t after_stop);
void pbio_control_start_hold_control(pbio_control_t *ctl, int32_t time_now, int32_t target_count);

bool pbio_control_is_stalled(pbio_control_t *ctl);
bool pbio_control_is_done(pbio_control_t *ctl);

void control_update(pbio_control_t *ctl, int32_t time_now, int32_t count_now, int32_t rate_now, pbio_actuation_t *actuation_type, int32_t *control);

#endif // _PBIO_CONTROL_H_
