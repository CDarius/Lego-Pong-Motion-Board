#include <Arduino.h>
#include <Wire.h>
#include <WiFi.h>
#include "monotonic.h"
#include "config.h"
#include "api_server/api_server.hpp"
#include "devices/unit_encoder.hpp"
#include "devices/rgb_led.hpp"
#include "devices/button.hpp"
#include "devices/io_board.hpp"
#include "motor_control/motor.hpp"
#include "motor_control/controlsettings.h"
#include "motor_control/motorwithreferenceswitch.hpp"
#include "motor_control/motorwithstallreference.hpp"
#include "motor_control/encoderjog.hpp"
#include "motor_control/error.hpp"
#include "game/game.hpp"
#include "game/encodermultijog.hpp"
#include "game/axes_homing.hpp"
#include "game/game_buttons.hpp"
#include "utils/i2c_utils.hpp"
#include "utils/logger.hpp"
#include "utils/cancel_token.hpp"

#include "settings/settings.hpp"
#include "web_functions/web_functions.hpp"

// ****************************************************
// *** I/O Pins
// ****************************************************

#define X_AXIS_PWM_PIN_1        13
#define X_AXIS_PWM_PIN_2        12
#define X_AXIS_ENC_PIN_1        11
#define X_AXIS_ENC_PIN_2        10

#define Y_AXIS_PWM_PIN_1        9
#define Y_AXIS_PWM_PIN_2        46
#define Y_AXIS_ENC_PIN_1        3
#define Y_AXIS_ENC_PIN_2        8

#define L_AXIS_PWM_PIN_1        18
#define L_AXIS_PWM_PIN_2        17
#define L_AXIS_ENC_PIN_1        16
#define L_AXIS_ENC_PIN_2        15

#define R_AXIS_PWM_PIN_1        4
#define R_AXIS_PWM_PIN_2        5
#define R_AXIS_ENC_PIN_1        6
#define R_AXIS_ENC_PIN_2        7

#define IO_BOARD_UART_RX        39
#define IO_BOARD_UART_TX        40

#define I2C_BUS_1_SDA           2
#define I2C_BUS_1_SCL           1

#define I2C_BUS_2_SDA           41
#define I2C_BUS_2_SCL           42

#define Y_AXIS_HOME_SWITCH_PIN   45 // Grove port 2

#define LED_OUTPUT              38

// ****************************************************

ApiRestServer server;

stall_homing_config_t x_motor_homing_config = {
    .start_in_positive_direction = false,
    .speed = 10.0, // Speed in motor stud/second
    .duty_limit = 50.0, // Duty cycle limit to detect stall on the reference obstacle (0.0% to 100.0%)
    .minimum_travel = 12.0, // Minimum travel distance before hitting the switch in stud
};

switch_homing_config_t y_motor_homing_config = {
    .home_switch_pin = Y_AXIS_HOME_SWITCH_PIN,
    .switch_pressed_value = HIGH,
    .start_in_positive_direction = true,
    .speed = 10.0, // Speed in motor stud/second
    .minimum_travel = 12.0, // Minimum travel distance before hitting the switch in stud
};
stall_homing_config_t l_motor_homing_config = {
    .start_in_positive_direction = true,
    .speed = 10.0, // Speed in motor stud/second
    .duty_limit = 30.0, // Duty cycle limit to detect stall on the reference obstacle (0.0% to 100.0%)
    .minimum_travel = 12.0, // Minimum travel distance before hitting the switch in stud
};
stall_homing_config_t r_motor_homing_config = {
    .start_in_positive_direction = true,
    .speed = 10.0, // Speed in motor stud/second
    .duty_limit = 30.0, // Duty cycle limit to detect stall on the reference obstacle (0.0% to 100.0%)
    .minimum_travel = 12.0, // Minimum travel distance before hitting the switch in stud
};


MotorWithStallReference x_motor(x_motor_homing_config);
MotorWithReferenceSwitch y_motor(y_motor_homing_config);
MotorWithStallReference l_motor(l_motor_homing_config);
MotorWithStallReference r_motor(r_motor_homing_config);

UnitEncoder l_encoder;
UnitEncoder r_encoder;
RGBLed rgb_led;
Button stop_button;
IOBoard io_board(Serial1);

encoder_multi_jog_config_t encoder_jog_config  {    
    .update_interval_ms = 50, // Axis position update interval in milliseconds
    .x_encoder_multiplier = 0.2, // X-Axis multiplier for encoder value to convert from encoder units to motor position units
    .y_encoder_multiplier = 0.2, // Y-Axis multiplier for encoder value to convert from encoder units to motor position units
    .l_r_encoder_multiplier = 0.2, // L-Axis and R-Axis multiplier for encoder value to convert from encoder units to motor position units
};

EncoderMultiJog l_encoder_jog(l_encoder, encoder_jog_config, x_motor, y_motor, l_motor, r_motor);
EncoderMultiJog r_encoder_jog(r_encoder, encoder_jog_config, x_motor, y_motor, l_motor, r_motor);

Game game(x_motor, y_motor, l_motor, r_motor, io_board, l_encoder_jog, r_encoder_jog);

Settings game_settings(game, encoder_jog_config, x_motor, y_motor, l_motor, r_motor);
WebFunctions web_functions(io_board, l_encoder_jog, r_encoder_jog, game,  x_motor, y_motor, l_motor, r_motor);

bool service_mode;

void log_motor_errors(pbio_error_t err, const char* err_string, const char* message) {
    String Message = String("[") + (err_string ? err_string : "Unknown") + "] " + (message ? message : "");
    Logger::instance().logE(Message.c_str());
}

void motor_loop_task(void *parameter) {
    int32_t counter = 0;
    bool led = false;

    while (true) {
        uint64_t start_time = monotonic_us();
        uint32_t millis = (uint32_t)(start_time / US_PER_MS);

        // Update stop buttons status
        stop_button.setRawState(millis, STOP_BUTTON_PRESSED);

        // Stop game and all motors if the stop button was clicked
        if (stop_button.wasClicked()) {
            CancelToken::cancelAll();
            x_motor.stop();
            y_motor.stop();
            l_motor.stop();
            r_motor.stop();
        }

        // Update motors motion
        x_motor.update();
        y_motor.update();
        l_motor.update();
        r_motor.update();
        
        // Blink the watchdog LED every 150 iterations (450 ms)
        counter++;
        if (counter >= 150) {
            counter = 0;
            led = !led;
#ifndef DEVEL_NO_HARDWARE            
            digitalWrite(LED_OUTPUT, led ? HIGH : LOW);
#else   
            rgb_led.setColor(led ? RGB_COLOR_GREEN : RGB_COLOR_BLACK);
#endif
        }

        // Run the task every 3ms
        delay(PBIO_CONFIG_SERVO_PERIOD_MS);
    }
}

void send_axes_position_task(void *parameter) {
    while (true) {
        io_board.sendAxesPosition(
            x_motor.angle(),
            y_motor.angle(),
            l_motor.angle(),
            r_motor.angle()
        );
        delay(200);
    }
}

void setup() {    
    // Wait for other devices to initialize
    delay(500);

    // Configure USB & and IO board uarts
    Serial.begin(115200);
    Serial1.begin (115200, SERIAL_8N1, IO_BOARD_UART_RX, IO_BOARD_UART_TX, false);
    Serial1.setTimeout(0);

    // Configure log output to ioboard
    Logger::instance().setOutput([](LogLevel level, const char* message) {
        io_board.sendLog(level, message);
    });

    // Configure inputs
    pinMode(START_BUTTON_PIN, INPUT_PULLUP);
    pinMode(STOP_BUTTON_PIN, INPUT_PULLUP);
    pinMode(Y_AXIS_HOME_SWITCH_PIN, INPUT_PULLUP);

    // Configure outputs
    pinMode(LED_OUTPUT, OUTPUT);
    rgb_led.begin();
    rgb_led.setColor(RGB_COLOR_WHITE);

    // Configure two I2C busses
    Wire.begin(I2C_BUS_1_SDA, I2C_BUS_1_SCL, 400000);
    Wire1.begin(I2C_BUS_2_SDA, I2C_BUS_2_SCL, 400000);
    l_encoder.begin(&Wire);
    r_encoder.begin(&Wire1);

    // Configure motors
    x_motor.begin("X", X_AXIS_ENC_PIN_1, X_AXIS_ENC_PIN_2, X_AXIS_PWM_PIN_1, X_AXIS_PWM_PIN_2, PBIO_DIRECTION_CLOCKWISE, 30.0, &settings_servo_ev3_large, log_motor_errors);
    y_motor.begin("Y", Y_AXIS_ENC_PIN_1, Y_AXIS_ENC_PIN_2, Y_AXIS_PWM_PIN_1, Y_AXIS_PWM_PIN_2, PBIO_DIRECTION_CLOCKWISE, 1138.0/31.0, &settings_servo_ev3_medium, log_motor_errors);
    l_motor.begin("L", L_AXIS_ENC_PIN_1, L_AXIS_ENC_PIN_2, L_AXIS_PWM_PIN_1, L_AXIS_PWM_PIN_2, PBIO_DIRECTION_CLOCKWISE, 30.0, &settings_servo_ev3_large, log_motor_errors);
    r_motor.begin("R", R_AXIS_ENC_PIN_1, R_AXIS_ENC_PIN_2, R_AXIS_PWM_PIN_1, R_AXIS_PWM_PIN_2, PBIO_DIRECTION_COUNTERCLOCKWISE, 30.0, &settings_servo_ev3_large, log_motor_errors);

    // Restore game and axes settings from NVS
    game_settings.restoreFromNVS();
    
    service_mode = START_BUTTON_PRESSED && STOP_BUTTON_PRESSED;
    bool start_web_server = service_mode;
#if defined(DEVEL_NO_HARDWARE) || defined(FORCE_SERVICE_MODE)
    service_mode = true;
    start_web_server = true;
#endif
#if FORCE_START_WEB_SERVER
    start_web_server = true;
#endif

#ifndef DEVEL_NO_HARDWARE
    // Test connection with the I/O board (also waits for the board to be ready)
    rgb_led.setColor(RGB_COLOR_YELLOW);
    if (!io_board.testConnection(10000)) {
        Serial.println("Failed to connect to the I/O board. Please check the connection.");
        rgb_led.unrecoverableError();
    }

    // Restart the I/O board
    rgb_led.setColor(RGB_COLOR_MAGENTA);
    io_board.sendInit(start_web_server);
    delay(200);

    // Wait for the I/O board to be ready
    if (!io_board.testConnection(10000)) {
        Serial.println("Failed to restore I/O board connection after INIT command");
        rgb_led.unrecoverableError();
    }
    rgb_led.setColor(RGB_COLOR_BLUE);

    // Start I/O baord comm task on core 1
    xTaskCreatePinnedToCore (
        send_axes_position_task,   // Function to implement the task
        "send_axes_pos",           // Name of the task
        2000,                   // Stack size in words
        NULL,                   // Task input parameter
        OTHER_TASK_PRIORITY,    // Priority of the task
        NULL,                   // Task handle.
        OTHER_TASK_CORE         // Core where the task should run
    );

    // Test that all I2C devices are connected
    bool all_i2c_devices_found = true;
    if (!testI2CDeviceExist(&Wire, UNIT_ENC_ENCODER_ADDR, "Left paddle encoder")) {
        all_i2c_devices_found = false;
        Logger::instance().logE("Left paddle encoder not found!!");
    }
    if (!testI2CDeviceExist(&Wire1, UNIT_ENC_ENCODER_ADDR, "Right paddle encoder")) {
        all_i2c_devices_found = false;
        Logger::instance().logE("Right paddle encoder not found!!");
    }
    if (!all_i2c_devices_found) {
        rgb_led.unrecoverableError();
    }
#endif

    // Start motor loop task on core 0. Core 0 is used only for motor control
    xTaskCreatePinnedToCore (
        motor_loop_task,        // Function to implement the task
        "motor_loop",           // Name of the task
        8000,                   // Stack size in words
        NULL,                   // Task input parameter
        MOTION_TASK_PRIORITY,   // Priority of the task
        NULL,                   // Task handle.
        MOTION_TASK_CORE        // Core where the task should run
    );

    if (start_web_server) {
        // Connect to WiFi
        IPAddress staticIP(IP_ADDRESS);
        IPAddress gateway(IP_GATEWAY);
        IPAddress subnet(IP_SUBNET);
        IPAddress dns(IP_DNS);
    
        if (WiFi.config(staticIP, gateway, subnet, dns, dns)) {
            WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
            while (WiFi.status() != WL_CONNECTED) {
                delay(1000);
                Logger::instance().logI("Connecting to WiFi...");
            }
            Logger::instance().logI("Connected to WiFi!");
            Logger::instance().logI("IP Address: " + WiFi.localIP().toString());
            
            server.begin(
                &game_settings, &web_functions, 
                &x_motor, &y_motor, &l_motor, &r_motor,
                game.getLogger());
        } 
        else {
            Logger::instance().logE("WiFi configuration failed. Please check the static IP settings.");
            rgb_led.unrecoverableError();
        }
    }

    rgb_led.setColor(RGB_COLOR_GREEN);
    io_board.playSound(IO_BOARD_SOUND_START);

    if (!service_mode) {
        // Home all axes
        CancelToken cancelToken;
        pbio_error_t err = homeAllAxes(x_motor, y_motor, l_motor, r_motor, io_board, l_encoder_jog, r_encoder_jog, cancelToken);
        if (err != PBIO_SUCCESS) {
            Logger::instance().logE("Failed to home all axes.");
            rgb_led.unrecoverableError();
        }
        Logger::instance().logI("Axes homed successfully");

        // Reset game display to score 0-0 and beep three times
        game.resetDisplay();
        for (int i = 0; i < 3; i++) {
            io_board.playSound(IO_BOARD_SOUND_BEEP);
            delay(300);
        }
    }
    else {
        io_board.showScrollingText("Service mode", SCROLLING_TEXT_ANIM_DELAY, true);
    }
}

void loop() {
    if (!service_mode) {
        Button startButton;

        // Wait for the start button press and no STOP button pressed
        while (!startButton.wasClicked() || STOP_BUTTON_PRESSED)
        {
            startButton.setRawState(millis(), START_BUTTON_PRESSED);
            delay(100);
        }

        // Run one match with a random player
        GamePlayer player = (esp_random() & 1) ? GamePlayer::L : GamePlayer::R;
        CancelToken cancelToken;
        game.run(player, GameMode::PLAYER_VS_PLAYER, cancelToken);
    }
    else {
        // Service mode, nothing to do here
        delay(100);
    }
}