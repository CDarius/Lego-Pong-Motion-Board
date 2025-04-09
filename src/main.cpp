#include <Arduino.h>
#include <Wire.h>
#include <WiFi.h>
#include "monotonic.h"
#include "unit_encoder.hpp"
#include "config.h"
#include "api_server.hpp"
#include "motor_control/motor.hpp"
#include "motor_control/controlsettings.h"
#include "game/game.hpp"

#include "settings/settings.hpp"

// ****************************************************
// *** I/O Pins
// ****************************************************

#define X_AXIS_PWM_PIN_1        4
#define X_AXIS_PWM_PIN_2        5
#define X_AXIS_ENC_PIN_1        6
#define X_AXIS_ENC_PIN_2        7

#define Y_AXIS_PWM_PIN_1        15
#define Y_AXIS_PWM_PIN_2        16
#define Y_AXIS_ENC_PIN_1        17
#define Y_AXIS_ENC_PIN_2        18

#define L_AXIS_PWM_PIN_1        8
#define L_AXIS_PWM_PIN_2        3
#define L_AXIS_ENC_PIN_1        46
#define L_AXIS_ENC_PIN_2        9

#define R_AXIS_PWM_PIN_1        10
#define R_AXIS_PWM_PIN_2        11
#define R_AXIS_ENC_PIN_1        12
#define R_AXIS_ENC_PIN_2        13

#define IO_BOARD_UART_RX        44
#define IO_BOARD_UART_TX        43

#define I2C_BUS_1_SDA           2
#define I2C_BUS_1_SCL           1

#define I2C_BUS_2_SDA           41
#define I2C_BUS_2_SCL           42

#define START_BUTTON_PIN        40
#define STOP_BUTTON_PIN         39
#define Y_AXIS_HOME_SWITH_PIN   38
#define L_AXIS_HOME_SWITH_PIN   47
#define R_AXIS_HOME_SWITH_PIN   21

#define LED_OUTPUT              48

// ****************************************************

ApiRestServer server;

Motor x_motor;
Motor y_motor;
Motor l_motor;
Motor r_motor;
UnitEncoder encoder;

Game game;

Settings game_settings(game, x_motor, y_motor, l_motor, r_motor);

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

void scanI2CDevices(TwoWire* wire) {
    byte error, address;
    int nDevices;
  
    Serial.println("Scanning...");
  
    nDevices = 0;
    for(address = 1; address < 127; address++ )
    {
      // The i2c_scanner uses the return value of
      // the Write.endTransmisstion to see if
      // a device did acknowledge to the address.
      wire->beginTransmission(address);
      error = wire->endTransmission();
  
      if (error == 0)
      {
        Serial.print("I2C device found at address 0x");
        if (address<16)
          Serial.print("0");
        Serial.print(address,HEX);
        Serial.println("  !");
  
        nDevices++;
      }
      else if (error==4)
      {
        Serial.print("Unknown error at address 0x");
        if (address<16)
          Serial.print("0");
        Serial.println(address,HEX);
      }
    }
    if (nDevices == 0)
      Serial.println("No I2C devices found\n");
    else
      Serial.println("done\n");
  
    delay(1000);           // wait 5 seconds for next scan
}

void setup() {
    // Configure USB & and IO board uarts
    Serial.begin(115200);
    Serial1.begin (115200, SERIAL_8N1, IO_BOARD_UART_RX, IO_BOARD_UART_TX, false, 0);
    Serial1.setTimeout(0);

    // Configure inputs
    pinMode(START_BUTTON_PIN, INPUT_PULLUP);
    pinMode(STOP_BUTTON_PIN, INPUT_PULLUP);
    pinMode(Y_AXIS_HOME_SWITH_PIN, INPUT_PULLUP);
    pinMode(L_AXIS_HOME_SWITH_PIN, INPUT_PULLUP);
    pinMode(R_AXIS_HOME_SWITH_PIN, INPUT_PULLUP);

    // Configure outputs
    pinMode(LED_OUTPUT, OUTPUT);

    // Configure two I2C busses
    Wire.begin(I2C_BUS_1_SDA, I2C_BUS_1_SCL, 400000);
    Wire1.begin(I2C_BUS_2_SDA, I2C_BUS_2_SCL, 400000);
    encoder.begin(&Wire);

    // Configure motors
    x_motor.begin(X_AXIS_PWM_PIN_1, X_AXIS_PWM_PIN_2, X_AXIS_ENC_PIN_1, X_AXIS_ENC_PIN_2, PBIO_DIRECTION_CLOCKWISE, 1.0, &settings_servo_ev3_large);
    y_motor.begin(Y_AXIS_PWM_PIN_1, Y_AXIS_PWM_PIN_2, Y_AXIS_ENC_PIN_1, Y_AXIS_ENC_PIN_2, PBIO_DIRECTION_CLOCKWISE, 1.0, &settings_servo_ev3_large);
    l_motor.begin(L_AXIS_PWM_PIN_1, L_AXIS_PWM_PIN_2, L_AXIS_ENC_PIN_1, L_AXIS_ENC_PIN_2, PBIO_DIRECTION_CLOCKWISE, 1.0, &settings_servo_ev3_large);
    r_motor.begin(R_AXIS_PWM_PIN_1, R_AXIS_PWM_PIN_2, R_AXIS_ENC_PIN_1, R_AXIS_ENC_PIN_2, PBIO_DIRECTION_CLOCKWISE, 1.0, &settings_servo_ev3_large);
    
    // Restore game and axes settings from NVS
    game_settings.restoreFromNVS();
    
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
            Serial.println("Connecting to WiFi...");
        }
        Serial.println("Connected to WiFi!");
        Serial.print("IP Address: ");
        Serial.println(WiFi.localIP());

        server.begin(&game_settings);
    } 
    else {
        Serial.println("Configuration failed!!!");
    }
}

void loop() {
    unsigned long time = micros();

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

    delay(1000);

    scanI2CDevices(&Wire1);
}