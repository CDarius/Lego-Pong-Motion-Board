// SPDX-License-Identifier: MIT
// Copyright (c) 2018-2020 The Pybricks Authors

#pragma once

#include <Arduino.h>
#include "motor_control/extra_math.h"
#include "motor_control/const.h"
#include "motor_control/macros.h"
#include "motor_control/error.hpp"

#define DURATION_FOREVER (-1)
#define DURATION_MAX_S (30*60)

// Macro to evaluate b*t/US_PER_SECOND in two steps to avoid excessive round-off errors and overflows.
#define timest(b, t) ((b * ((t)/US_PER_MS))/MS_PER_SECOND)

/**
Macro to evaluate division of speed by acceleration (w/a), yielding time, in the appropriate units
Return the time to reach the speed w at the acceleation a

:param w: Speed in (enc count/sec)
:param a: Acceleration in (enc count/sec^2)
:return: Time to reach the speed in us
*/
#define wdiva(w, a) ((((w)*US_PER_MS)/a)*MS_PER_SECOND)

/**
 * Motor trajectory parameters for an ideal maneuver without disturbances
 */
typedef struct _pbio_trajectory_t {
    bool forever;                       /**<  Whether maneuver has end-point */
    int32_t t0;                         /**<  Time at start of maneuver (us) */
    int32_t t1;                         /**<  Time after the acceleration in-phase (us) */
    int32_t t2;                         /**<  Time at start of acceleration out-phase (us) */
    int32_t t3;                         /**<  Time at end of maneuver (us) */
    int32_t th0;                        /**<  Encoder count at start of maneuver (integer part) */
    int32_t th1;                        /**<  Encoder count after the acceleration in-phase (integer part) */
    int32_t th2;                        /**<  Encoder count at start of acceleration out-phase (integer part) */
    int32_t th3;                        /**<  Encoder count at end of maneuver (integer part) */
    int32_t th0_ext;                    /**<  As above, but additional millicounts (decimal part only in millicouns) */
    int32_t th1_ext;                    /**<  As above, but additional  millicounts (decimal part only in millicouns) */
    int32_t th2_ext;                    /**<  As above, but additional  millicounts (decimal part only in millicouns) */
    int32_t th3_ext;                    /**<  As above, but additional  millicounts (decimal part only in millicouns) */
    int32_t w0;                         /**<  Encoder rate at start of maneuver (count/sec) */
    int32_t w1;                         /**<  Encoder rate target when not accelerating (count/sec) */
    int32_t a0;                         /**<  Encoder acceleration during in-phase (count/sec^2) */
    int32_t a2;                         /**<  Encoder acceleration during out-phase (count/sec^2) */
} pbio_trajectory_t;

// Core trajectory generators

void pbio_trajectory_make_stationary(pbio_trajectory_t *ref, int32_t t0, int32_t th0);

pbio_error_t pbio_trajectory_make_time_based(pbio_trajectory_t *ref, int32_t t0, int32_t duration, int32_t th0, int32_t th0_ext, int32_t w0, int32_t wt, int32_t wmax, int32_t a, int32_t amax);

pbio_error_t pbio_trajectory_make_angle_based(pbio_trajectory_t *ref, int32_t t0, int32_t th0, int32_t th3, int32_t w0, int32_t wt, int32_t wmax, int32_t a, int32_t amax);

void pbio_trajectory_get_reference(pbio_trajectory_t *traject, int32_t time_ref, int32_t *count_ref, int32_t *count_ref_ext, int32_t *rate_ref, int32_t *acceleration_ref);

// Extended and patched trajectories

pbio_error_t pbio_trajectory_make_time_based_patched(pbio_trajectory_t *ref, int32_t t0, int32_t t3, int32_t wt, int32_t wmax, int32_t a, int32_t amax);

pbio_error_t pbio_trajectory_make_angle_based_patched(pbio_trajectory_t *ref, int32_t t0, int32_t th3, int32_t wt, int32_t wmax, int32_t a, int32_t amax);
