#include <Arduino.h>
#include <Wire.h>
#include <WiFi.h>
#include "monotonic.h"
#include "config.h"
#include "api_server.hpp"
#include "devices/unit_encoder.hpp"
#include "devices/rgb_led.hpp"
#include "motor_control/motor.hpp"
#include "motor_control/controlsettings.h"
#include "motor_control/motorhoming.hpp"
#include "motor_control/error.hpp"
#include "game/game.hpp"
#include "utils/i2c_utils.hpp"
#include "utils/logger.hpp"

#include "settings/settings.hpp"

// ****************************************************
// *** I/O Pins
// ****************************************************

#define X_AXIS_PWM_PIN_1        13
#define X_AXIS_PWM_PIN_2        12
#define X_AXIS_ENC_PIN_1        10
#define X_AXIS_ENC_PIN_2        11

#define Y_AXIS_PWM_PIN_1        9
#define Y_AXIS_PWM_PIN_2        46
#define Y_AXIS_ENC_PIN_1        8
#define Y_AXIS_ENC_PIN_2        3

#define L_AXIS_PWM_PIN_1        18
#define L_AXIS_PWM_PIN_2        17
#define L_AXIS_ENC_PIN_1        15
#define L_AXIS_ENC_PIN_2        16

#define R_AXIS_PWM_PIN_1        4
#define R_AXIS_PWM_PIN_2        5
#define R_AXIS_ENC_PIN_1        7
#define R_AXIS_ENC_PIN_2        6

#define IO_BOARD_UART_RX        44
#define IO_BOARD_UART_TX        43

#define I2C_BUS_1_SDA           2
#define I2C_BUS_1_SCL           1

#define I2C_BUS_2_SDA           41
#define I2C_BUS_2_SCL           42

#define START_BUTTON_PIN        40 // Grover port 1
#define STOP_BUTTON_PIN         39 // Grover port 1
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
    .start_in_positive_direction = false,
    .speed = 10.0, // Speed in motor stud/second
    .minimum_travel = 12.0, // Minimum travel distance before hitting the switch in stud
    .retract_distance = 8.0, // Distance to retract after hitting the switch in stud
    .switch_axis_position = 0.0 // Position of the switch in stud
};
switch_homing_config_t l_motor_homing_config = {
    .start_in_positive_direction = true,
    .speed = 10.0, // Speed in motor stud/second
    .minimum_travel = 12.0, // Minimum travel distance before hitting the switch in stud
    .retract_distance = 8.0, // Distance to retract after hitting the switch in stud
    .switch_axis_position = 0.0 // Position of the switch in stud
};
switch_homing_config_t r_motor_homing_config = {
    .start_in_positive_direction = true,
    .speed = 10.0, // Speed in motor stud/second
    .minimum_travel = 12.0, // Minimum travel distance before hitting the switch in stud
    .retract_distance = 8.0, // Distance to retract after hitting the switch in stud
    .switch_axis_position = 0.0 // Position of the switch in stud
};

UnitEncoder l_encoder;
UnitEncoder r_encoder;
RGBLed rgb_led;

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

        x_motor.update();
        y_motor.update();
        l_motor.update();
        r_motor.update();
        
        counter++;
        if (counter >= 150) {
            counter = 0;
            led = !led;
            digitalWrite(LED_OUTPUT, led ? HIGH : LOW);
        }

        // Run the task every 6ms
        uint64_t delta_time = monotonic_us() - start_time;
        delay_ms = (uint32_t)((PBIO_CONFIG_SERVO_PERIOD_MS*US_PER_MS - delta_time) / 1000);
        if (delay_ms == 0 || delay_ms > PBIO_CONFIG_SERVO_PERIOD_MS)
            delay(0);
        else
            delay(delay_ms);
    }
}

void measure_task(void *parameter) {
    while (true) {
        Serial.print("Counter = ");
        Serial.print(x_motor.angle());
        Serial.print(" speed = ");
        Serial.print(x_motor.speed());
        Serial.print(" count/sec speed = ");
        Serial.println(x_motor.speed() / 6);
        delay(500);    
    }
}

void setup() {
    // Configure USB & and IO board uarts
    Serial.begin(115200);
    Serial1.begin (115200, SERIAL_8N1, IO_BOARD_UART_RX, IO_BOARD_UART_TX, false, 0);
    Serial1.setTimeout(0);

    // Configure inputs
    pinMode(START_BUTTON_PIN, OUTPUT);
    pinMode(STOP_BUTTON_PIN, OUTPUT);
    pinMode(Y_AXIS_HOME_SWITH_PIN, OUTPUT);
    pinMode(L_AXIS_HOME_SWITH_PIN, OUTPUT);
    pinMode(R_AXIS_HOME_SWITH_PIN, OUTPUT);

    //pinMode(35, OUTPUT); // Can not be used
    //pinMode(36, OUTPUT); // Can not be used
    //pinMode(37, OUTPUT); // Can not be used
    pinMode(45, INPUT_PULLUP);
    pinMode(14, INPUT_PULLUP);

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
    x_motor.begin("X", X_AXIS_ENC_PIN_1, X_AXIS_ENC_PIN_2, X_AXIS_PWM_PIN_1, X_AXIS_PWM_PIN_2, PBIO_DIRECTION_CLOCKWISE, 1.0, &settings_servo_ev3_large, log_motor_errors);
    y_motor.begin("Y", Y_AXIS_ENC_PIN_1, Y_AXIS_ENC_PIN_2, Y_AXIS_PWM_PIN_1, Y_AXIS_PWM_PIN_2, PBIO_DIRECTION_CLOCKWISE, 1.0, &settings_servo_ev3_large, log_motor_errors);
    l_motor.begin("L", L_AXIS_ENC_PIN_1, L_AXIS_ENC_PIN_2, L_AXIS_PWM_PIN_1, L_AXIS_PWM_PIN_2, PBIO_DIRECTION_CLOCKWISE, 1.0, &settings_servo_ev3_large, log_motor_errors);
    r_motor.begin("R", R_AXIS_ENC_PIN_1, R_AXIS_ENC_PIN_2, R_AXIS_PWM_PIN_1, R_AXIS_PWM_PIN_2, PBIO_DIRECTION_CLOCKWISE, 1.0, &settings_servo_ev3_large, log_motor_errors);
    
    // Restore game and axes settings from NVS
    game_settings.restoreFromNVS();
    
    // TODO:
    // Start communication with I/O board
    // Send a PING command and wait for a response (implemnt a retry)
    // If the response is not received, log the error and do the red blinking of the LED
    // If the communication is established, send an INIT command with the start web server status

    // TODO: 
    // Add a semaphore to protect to the I/O board communication. Different tasks on different cores can access the I/O board at the same time, which can lead to data corruption.

    // Test that all I2C devices are connected
    bool all_i2c_devices_found = true;
    if (!testI2CDeviceExist(&Wire, UNIT_ENC_ENCODER_ADDR, "Left paddle encoder")) {
        all_i2c_devices_found = false;
    }
    if (!testI2CDeviceExist(&Wire1, UNIT_ENC_ENCODER_ADDR, "Right paddle encoder")) {
        all_i2c_devices_found = false;
    }
    if (!all_i2c_devices_found) {
        rgb_led.unrecoverableError();
    }

    // Start motor loop task on core 0
    xTaskCreatePinnedToCore (
        motor_loop_task,    // Function to implement the task
        "motor_loop",       // Name of the task
        8000,      // Stack size in words
        NULL,      // Task input parameter
        0,         // Priority of the task
        NULL,      // Task handle.
        0          // Core where the task should run
    );

    // Start meauser task on core 1
    xTaskCreatePinnedToCore (
        measure_task,   // Function to implement the task
        "measue",       // Name of the task
        4000,      // Stack size in words
        NULL,      // Task input parameter
        0,         // Priority of the task
        NULL,      // Task handle.
        1          // Core where the task should run
    );

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

    rgb_led.setColor(RGB_COLOR_GREEN);
}

void loop() {
    unsigned long time = micros();

    // Serial test
    /*
    Serial1.println("Serial 1!!");
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
    */

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

    //scanI2CDevices(&Wire1);
}