#include <Arduino.h>
#include <Wire.h>
#include <WiFi.h>
#include "monotonic.h"
#include "config.h"
#include "api_server.hpp"
#include "devices/unit_encoder.hpp"
#include "devices/rgb_led.hpp"
#include "devices/button.hpp"
#include "devices/io_board.hpp"
#include "motor_control/motor.hpp"
#include "motor_control/controlsettings.h"
#include "motor_control/motorhoming.hpp"
#include "motor_control/encoderjog.hpp"
#include "motor_control/error.hpp"
#include "game/game.hpp"
#include "utils/i2c_utils.hpp"
#include "utils/logger.hpp"
#include "utils/cancel_token.hpp"

#include "settings/settings.hpp"

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

//#define IO_BOARD_UART_RX        44
//#define IO_BOARD_UART_TX        43
#define IO_BOARD_UART_RX        39
#define IO_BOARD_UART_TX        40

#define I2C_BUS_1_SDA           2
#define I2C_BUS_1_SCL           1

#define I2C_BUS_2_SDA           41
#define I2C_BUS_2_SCL           42

//#define START_BUTTON_PIN        40 // Grover port 1
//#define STOP_BUTTON_PIN         39 // Grover port 1
#define START_BUTTON_PIN        44 // Grover port 1
#define STOP_BUTTON_PIN         43 // Grover port 1
#define Y_AXIS_HOME_SWITH_PIN   21 // Groove port 3
#define L_AXIS_HOME_SWITH_PIN   45 // Groove port 2
#define R_AXIS_HOME_SWITH_PIN   47 // Groove port 2

#define LED_OUTPUT              38

// ****************************************************

ApiRestServer server;

Motor x_motor;
Motor y_motor;
Motor l_motor;
Motor r_motor;
switch_homing_config_t y_motor_homing_config = {
    .start_in_positive_direction = true,
    .speed = 10.0, // Speed in motor stud/second
    .minimum_travel = 12.0, // Minimum travel distance before hitting the switch in stud
    .retract_distance = 8.0, // Distance to retract after hitting the switch in stud
    .switch_axis_position = 0.0 // Position of the switch in stud
};
switch_homing_config_t l_motor_homing_config = {
    .start_in_positive_direction = false,
    .speed = 10.0, // Speed in motor stud/second
    .minimum_travel = 12.0, // Minimum travel distance before hitting the switch in stud
    .retract_distance = 8.0, // Distance to retract after hitting the switch in stud
    .switch_axis_position = 0.0 // Position of the switch in stud
};
switch_homing_config_t r_motor_homing_config = {
    .start_in_positive_direction = false,
    .speed = 10.0, // Speed in motor stud/second
    .minimum_travel = 12.0, // Minimum travel distance before hitting the switch in stud
    .retract_distance = 8.0, // Distance to retract after hitting the switch in stud
    .switch_axis_position = 0.0 // Position of the switch in stud
};

UnitEncoder l_encoder;
UnitEncoder r_encoder;
RGBLed rgb_led;
Button start_button;
Button stop_button;
IOBoard io_board(Serial1);

EncoderJog l_encoder_jog;
EncoderJog r_encoder_jog;

Game game;

Settings game_settings(game, x_motor, y_motor, l_motor, r_motor,
    y_motor_homing_config, l_motor_homing_config, r_motor_homing_config);

void log_motor_errors(pbio_error_t err, const char* err_string, const char* message) {
    String Message = String("[") + (err_string ? err_string : "Unknown") + "] " + (message ? message : "");
    Logger::instance().logE(Message.c_str());
}

void motor_loop_task(void *parameter) {
    int32_t counter = 0;
    bool led = false;

    uint32_t delay_ms;
    while (true) {
        uint64_t start_time = monotonic_us();
        uint32_t millis = (uint32_t)(start_time / US_PER_MS);

        // Update start and stop buttons status
        start_button.setRawState(millis, digitalRead(START_BUTTON_PIN) == LOW);
        stop_button.setRawState(millis, digitalRead(STOP_BUTTON_PIN) == LOW);

        // Stop game and all motors if the stop button was clicked
        if (stop_button.wasClicked()) {
            CancelToken::cancelAll();
        }

        // Update motors motion
        x_motor.update();
        y_motor.update();
        l_motor.update();
        r_motor.update();
        
        // Blink the watchdog LED every
        counter++;
        if (counter >= 150) {
            counter = 0;
            led = !led;
            digitalWrite(LED_OUTPUT, led ? HIGH : LOW);
        }

        // Run the task every 3ms
        uint64_t delta_time = monotonic_us() - start_time;
        delay_ms = (uint32_t)((PBIO_CONFIG_SERVO_PERIOD_MS*US_PER_MS - delta_time) / 1000);
        if (delay_ms == 0 || delay_ms > PBIO_CONFIG_SERVO_PERIOD_MS)
            delay(0);
        else
            delay(delay_ms);
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

void measure_task(void *parameter) {    
    while (true) {
        Serial.print("Counter = ");
        Serial.print(x_motor.angle());
        Serial.print(", speed = ");
        Serial.print(x_motor.speed());
        Serial.print(" unit/sec, motor speed = ");
        Serial.print(x_motor.motor_speed());
        Serial.print(" deg/sec, motor RPM = ");
        Serial.println(x_motor.motor_speed() / 6);
        delay(500);    
    }
}

void setup() {
    // Wait for other devices to initialize
    delay(500);

    // Configure USB & and IO board uarts
    Serial.begin(115200);
    Serial1.begin (115200, SERIAL_8N1, IO_BOARD_UART_RX, IO_BOARD_UART_TX, false);
    Serial1.setTimeout(0);

    // Configure inputs
    pinMode(START_BUTTON_PIN, INPUT_PULLUP);
    pinMode(STOP_BUTTON_PIN, INPUT_PULLUP);
    pinMode(Y_AXIS_HOME_SWITH_PIN, INPUT_PULLUP);
    pinMode(L_AXIS_HOME_SWITH_PIN, INPUT_PULLUP);
    pinMode(R_AXIS_HOME_SWITH_PIN, INPUT_PULLUP);

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
    x_motor.begin("X", X_AXIS_ENC_PIN_1, X_AXIS_ENC_PIN_2, X_AXIS_PWM_PIN_1, X_AXIS_PWM_PIN_2, PBIO_DIRECTION_CLOCKWISE, 2.0, &settings_servo_ev3_large, log_motor_errors);
    y_motor.begin("Y", Y_AXIS_ENC_PIN_1, Y_AXIS_ENC_PIN_2, Y_AXIS_PWM_PIN_1, Y_AXIS_PWM_PIN_2, PBIO_DIRECTION_CLOCKWISE, 1.0, &settings_servo_ev3_large, log_motor_errors);
    l_motor.begin("L", L_AXIS_ENC_PIN_1, L_AXIS_ENC_PIN_2, L_AXIS_PWM_PIN_1, L_AXIS_PWM_PIN_2, PBIO_DIRECTION_CLOCKWISE, 1.0, &settings_servo_ev3_large, log_motor_errors);
    r_motor.begin("R", R_AXIS_ENC_PIN_1, R_AXIS_ENC_PIN_2, R_AXIS_PWM_PIN_1, R_AXIS_PWM_PIN_2, PBIO_DIRECTION_CLOCKWISE, 1.0, &settings_servo_ev3_large, log_motor_errors);

    // Configure jog control for left and right paddles
    l_encoder_jog.begin(l_encoder);
    r_encoder_jog.begin(r_encoder);
    
    // Restore game and axes settings from NVS
    game_settings.restoreFromNVS();
    
    bool start_web_server = digitalRead(START_BUTTON_PIN) == LOW && digitalRead(STOP_BUTTON_PIN) == LOW;

    // Test connection with the I/O board (also waits for the board to be ready)
    rgb_led.setColor(RGB_COLOR_YELLOW);
    if (!io_board.testConnection(5000)) {
        Serial.println("Failed to connect to the I/O board. Please check the connection.");
        rgb_led.unrecoverableError();
    }

    // Restart the I/O board
    rgb_led.setColor(RGB_COLOR_MAGENTA);
    io_board.sendInit(start_web_server);

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
        2000,       // Stack size in words
        NULL,      // Task input parameter
        0,         // Priority of the task
        NULL,      // Task handle.
        1          // Core where the task should run
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

    // Start motor loop task on core 0. Core 0 is used only for motor control
    xTaskCreatePinnedToCore (
        motor_loop_task,    // Function to implement the task
        "motor_loop",       // Name of the task
        8000,      // Stack size in words
        NULL,      // Task input parameter
        0,         // Priority of the task
        NULL,      // Task handle.
        0          // Core where the task should run
    );

    /*
    xTaskCreatePinnedToCore (
        measure_task,   // Function to implement the task
        "measue",       // Name of the task
        4000,      // Stack size in words
        NULL,      // Task input parameter
        0,         // Priority of the task
        NULL,      // Task handle.
        1          // Core where the task should run
    );
    */

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
            Logger::instance().logI("IP Address: " + String(WiFi.localIP()));
    
            server.begin(&game_settings);
        } 
        else {
            Logger::instance().logE("WiFi configuration failed. Please check the static IP settings.");
            rgb_led.unrecoverableError();
        }
    }

    rgb_led.setColor(RGB_COLOR_GREEN);
}


/*
int counter = 0;
int score1 = 0;
int score2 = 0;
*/

void loop() {
    unsigned long time = micros();

    /*
    // Serial test
    //Serial1.println("Serial 1!!");
    io_board.sendLog(LogLevel::Info, "Test log message from main loop");
    Serial.print("Try read serial 1...");
    String read1 = Serial1.readStringUntil('\n');
    if (!read1.isEmpty())
        Serial.println(read1);
    else
        Serial.println("NO DATA");
    unsigned long delta_time = micros() - time;
    Serial.print("Serial 1 time: ");
    Serial.print(delta_time);
    Serial.println("us");
    delay(300);
    */

    /*
    delay(4000);
    pbio_error_t err = home_with_switch(x_motor, START_BUTTON_PIN, LOW, l_motor_homing_config);
    if (err != PBIO_SUCCESS) {
        Logger::instance().logE("Failed to home Y-axis motor: ");
        rgb_led.unrecoverableError();
    }
    else {
        Logger::instance().logI("Y-axis motor homed successfully");
    }
    */

    /*
    String msg = String("Test log message from main loop") + String(counter++);
    io_board.sendLog(LogLevel::Info, msg.c_str());
    delay(200);
    */

    /*
    if (io_board.testConnection(1000)) {
        Serial.println("IO board connection is OK!!");
    } else {
        Serial.println("IO board connection failed :(");
    }

    if (score1 <= score2) {
        score1++;
        if (score1 >= 10) {
            score1 = 0;
            score2 = 0;
        }
    }
    else {
        score2++;
        if (score2 >= 10) {
            score2 = 0;
        }
    }
    io_board.playSound(IO_BOARD_SOUND_BEEP2, 1);
    io_board.showScore(score1, score2, 50);
    delay(1500);
    io_board.playSound(IO_BOARD_SOUND_BEEP2, 1);
    io_board.showText("No scroll", 0);
    delay(1500);
    io_board.playSound(IO_BOARD_SOUND_BEEP2, 1);
    io_board.showText("Blink", 300);
    delay(1500);
    io_board.playSound(IO_BOARD_SOUND_BEEP2, 3);
    io_board.showScrollingText("Long scrolling text !!", 50, true, 0);
    delay(10000);
    */

    /*
    x_motor.dc(100);
    y_motor.dc(100);
    l_motor.dc(100);
    r_motor.dc(100);
    delay(4000);
    x_motor.dc(-100);
    y_motor.dc(-100);
    l_motor.dc(-100);
    r_motor.dc(-100);
    delay(4000);
    x_motor.stop();
    y_motor.stop();
    l_motor.stop();
    r_motor.stop();
    delay(2000);
     */

    //scanI2CDevices(&Wire1);

    /*
    io_board.showScrollingText("X-Axis jog", 50, true, 0);
    l_encoder_jog.setUpdateIntervalMs(50);
    l_encoder_jog.setEncoderMultiplier(4.0f);
    l_encoder_jog.start(x_motor);

    while (true) {
        l_encoder_jog.update();
        r_encoder_jog.update();

        // Update jog control every 200ms
        delay(50);
    }
    */

    /*
    io_board.showScrollingText("X-Axis stall test", 50, true, 0);
    Serial.println("Press the left paddle button to start ...");
    while (!l_encoder.getButtonStatus()) {
        Serial.print("Button stauts: ");
        Serial.println(l_encoder.getButtonStatus() ? "Pressed" : "Released");
        delay(100);
    }
    
    float speed = x_motor.get_speed_limit();
    x_motor.run_until_stalled(speed / 3, 70.0, PBIO_ACTUATION_HOLD);
    Serial.println("Stalled !!");
    */
    
    io_board.showScrollingText("X-Axis run test", 50, true, 0);
    float speed = x_motor.get_speed_limit();
    x_motor.run_angle(speed / 4, 360.0 * 4, PBIO_ACTUATION_HOLD);
    delay(2000);
}