#ifndef __GAME_SETTINGS_HPP__
#define __GAME_SETTINGS_HPP__

#include <Arduino.h>

struct GameXAxisSettings {
    uint8_t startBallGameSpeed;         // Game start ball speed on X axis (%)
    uint8_t ballBounceSpeedIncrement;   // Ball x-axis increment speed at each bounce (%)
    float bounceThresholdAtLowSpeed;    // Ball x-axis bounce threshold at startBallGameSpeed speed (stud)
    float bounceThresholdAtMaxSpeed;    // Ball x-axis bounce threshold at maximum speed (stud)
};

struct GameYAxisSettings {

};

struct GameSettings {
    GameXAxisSettings xAxis;
    GameYAxisSettings yAxis;
};

#endif