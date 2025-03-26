#include <Arduino.h>
#include <Wire.h>
#include "monotonic.h"
#include "unit_encoder.hpp"
#include "motor_control/config.h"
#include "motor_control/motor.hpp"
#include "motor_control/controlsettings.h"

Motor x_motor = Motor();
Motor y_motor = Motor();
Motor l_motor = Motor();
Motor r_motor = Motor();
UnitEncoder encoder = UnitEncoder();

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
            if (led)
                encoder.setLEDColor(1, 0x00FF00);
            else
                encoder.setLEDColor(1, 0x000000);
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
    Serial.begin(115200);
    Wire.begin(2, 1, 200000);
    encoder.begin(&Wire);
    x_motor.begin(5, 6, 7, 8, PBIO_DIRECTION_CLOCKWISE, 1.0, &settings_servo_ev3_large);
    y_motor.begin(9, 10, 11, 12, PBIO_DIRECTION_CLOCKWISE, 1.0, &settings_servo_ev3_large);
    l_motor.begin(15, 16, 17, 18, PBIO_DIRECTION_CLOCKWISE, 1.0, &settings_servo_ev3_large);
    r_motor.begin(35, 36, 37, 38, PBIO_DIRECTION_CLOCKWISE, 1.0, &settings_servo_ev3_large);

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
}


float angle = 0;

void loop() {
    /*
    //x_motor.dc(100);
    //x_motor.run(120.0 * 6);
    Serial.println("Start....");
    delay(5000);
    //x_motor.dc(0);
    x_motor.stop();
    Serial.println("Stop");    
    delay(5000);
    */
    
    /*
    Serial.println("Start....");
    x_motor.run_until_stalled(60*6, 30);
    Serial.println("Stop");    
    delay(5000);
    */

    /*
    Serial.println("Start....");
    x_motor.reset_angle(0);
    x_motor.run_target(30*6, 360*3);
    Serial.println("Stop");    
    delay(5000);
    */

    int16_t jog = encoder.getValue();
    encoder.clearValue();
    angle += jog * 2;
    x_motor.run_target(170 * 6, angle, PBIO_ACTUATION_HOLD, false);
    delay(100);
}