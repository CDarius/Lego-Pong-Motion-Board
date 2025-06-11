#ifndef __PROJECT_CONFIG_H__
#define __PROJECT_CONFIG_H__

//#define USE_IO_BOARD_AP

// Wifi configuration
#ifdef USE_IO_BOARD_AP
#define WIFI_SSID     "Lego_Pong"
#define WIFI_PASSWORD "ContinuateASparareStronzi"
#else
#define WIFI_SSID     "Agenoria"
#define WIFI_PASSWORD "carota24carati"
#endif

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

#define SERIAL_PRINT_MOTOR_ERROR

#endif