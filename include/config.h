#pragma once

#include <freertos/FreeRTOSConfig.h>

#define IP_ADDRESS    192, 168, 0, 205  
#define IP_GATEWAY    192, 168, 0, 1
#define IP_SUBNET     255, 255, 255, 0
#define IP_DNS        192, 168, 0, 1

#define FORCE_START_WEB_SERVER (0)

// Motor configuration
#define PBIO_CONFIG_SERVO_PERIOD_MS (3)

#define MOTOR_PWM_FREQUENCY (9000)
#define MOTOR_PWM_RESOLUTION (12)
#define MOTOR_PWM_MAX_VALUE ((1 << MOTOR_PWM_RESOLUTION) - 2)

#define ENCODER_COUNTS_PER_DEGREE (2)

// Task configuration
#define MOTION_TASK_CORE        (0)
#define OTHER_TASK_CORE         (1)
#define MOTION_TASK_PRIORITY    (configMAX_PRIORITIES - 1)
#define OTHER_TASK_PRIORITY     (0)

// Scrolling text animation speed
#define SCROLLING_TEXT_ANIM_DELAY (50)

// Leave this file at the end to be able to override configurations
#include "secrets.h"