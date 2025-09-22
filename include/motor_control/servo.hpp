// SPDX-License-Identifier: MIT
// Copyright (c) 2018-2020 The Pybricks Authors

#pragma once

#include <Arduino.h>
#include "monotonic.h"
#include "motor_control/const.h"
#include "motor_control/enums.h"
#include "motor_control/error.hpp"
#include "motor_control/controlsettings.h"
#include "motor_control/tacho.hpp"
#include "motor_control/dcmotor.hpp"
#include "motor_control/control.hpp"
#include "motor_control/logger.hpp"

typedef struct _pbio_servo_t {
    DCMotor *dcmotor;
    Tacho *tacho;
    pbio_control_t control;
    pbio_log_t log;
} pbio_servo_t;

void pbio_servo_setup(pbio_servo_t *srv, DCMotor *dcmotor, Tacho *tacho, float counts_per_unit, pbio_control_settings_t *settings);

void pbio_servo_reset_angle(pbio_servo_t *srv, float reset_angle);
pbio_error_t pbio_servo_is_stalled(pbio_servo_t *srv, bool *stalled);

void pbio_servo_stop(pbio_servo_t *srv, pbio_actuation_t after_stop);

void pbio_servo_set_duty_cycle(pbio_servo_t *srv, float duty_steps);

pbio_error_t pbio_servo_run(pbio_servo_t *srv, float speed);
pbio_error_t pbio_servo_run_time(pbio_servo_t *srv, float speed, int32_t duration, pbio_actuation_t after_stop);
pbio_error_t pbio_servo_run_until_stalled(pbio_servo_t *srv, float speed, pbio_actuation_t after_stop);
pbio_error_t pbio_servo_run_angle(pbio_servo_t *srv, float speed, float angle, pbio_actuation_t after_stop);
pbio_error_t pbio_servo_run_target(pbio_servo_t *srv, float speed, float target, pbio_actuation_t after_stop);
void pbio_servo_track_target(pbio_servo_t *srv, float target);

pbio_error_t pbio_servo_control_update(pbio_servo_t *srv);
