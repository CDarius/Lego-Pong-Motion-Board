#include "motor_control/motorhoming.hpp"

void home_with_switch(Motor& motor, uint8_t home_switch_pin, int switch_pressed_value, switch_homing_config_t& config) {
    float start_position = motor.angle();

    float forward_speed = config.start_in_positive_direction ? config.speed : -config.speed;
    float backward_speed = -2 * forward_speed;

    while (true)
    {
        // Start running toward the home swtich    
        motor.run(forward_speed);

        // Wait for the home switch to be pressed
        while (digitalRead(home_switch_pin) == switch_pressed_value) {
            delay(10); // Poll every 10 ms
        }

        float hit_position = motor.angle();
        float travel_distance = hit_position - start_position;
        if (travel_distance >= config.minimum_travel) {
            // We hit the switch after moving the minimum distance
            motor.reset_angle(config.switch_axis_position);
            break;
        }
        else {
            // Retract and try again
            motor.run_angle(backward_speed, config.minimum_travel * 1.05, PBIO_ACTUATION_HOLD, true);
            // Test if the switch is released
            if (digitalRead(home_switch_pin) == switch_pressed_value) {
                // The switch should not be pressed after retracting
                while (true) {
                    Serial.println("Warning: Switch still pressed after retracting. Retrying...");
                    delay(1000); 
                }
            }
        }
    }
    
    // Move away from the switch 
    motor.run_angle(backward_speed, config.retract_distance, PBIO_ACTUATION_COAST, true);

    // Test if the switch is released
    if (digitalRead(home_switch_pin) == switch_pressed_value) {
        // The switch should not be pressed after retracting
        while (true) {
            Serial.println("Warning: Switch still pressed after retracting. Retrying...");
            delay(1000); 
        }
    }
}