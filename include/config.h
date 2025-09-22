#pragma once

#include <freertos/FreeRTOSConfig.h>

#define IP_ADDRESS    192, 168, 0, 205  
#define IP_GATEWAY    192, 168, 0, 1
#define IP_SUBNET     255, 255, 255, 0
#define IP_DNS        192, 168, 0, 1

// Motor configuration
#define PBIO_CONFIG_SERVO_PERIOD_MS (3)

#define MOTOR_PWM_FREQUENCY (9000)
#define MOTOR_PWM_RESOLUTION (12)
#define MOTOR_PWM_MAX_VALUE ((1 << MOTOR_PWM_RESOLUTION) - 2)

#define ENCODER_COUNTS_PER_DEGREE (2)

// Log
#define MAX_LOG_MEM_KB 8*1024 // 8 MB on ESP32-S3-WROOM-1-N16R8

// Task configuration
#define MOTION_TASK_CORE            (0)
#define OTHER_TASK_CORE             (1)
#define MOTION_TASK_PRIORITY        (configMAX_PRIORITIES - 1)
#define OTHER_TASK_PRIORITY         (0)
#define OTHER_TASK_HIGH_PRIORITY    (10)

// Scrolling text animation speed
#define SCROLLING_TEXT_ANIM_DELAY   (50)
#define SCROLLING_SCORE_ANIM_DELAY  (50)
#define SCRORE_BLINK_DELAY          (300)

// Leave this file at the end to be able to override configurations
#include "secrets.h"