#pragma once

#include <Arduino.h>

#define X_AXIS_BOUNCE_INVERSIONS_COUNT 5
#define X_AXIS_BOUNCE_INVERSIONS_SPEEDS {60.0f, 70.0f, 80.0f, 90.0f, 100.0f}
#define Y_AXIS_BOUNCE_INVERSIONS_COUNT 5
#define Y_AXIS_BOUNCE_INVERSIONS_SPEEDS {60.0f, 70.0f, 80.0f, 90.0f, 100.0f}

struct GameXAxisSettings {
    uint8_t startBallGameSpeed = 50;            // Game start ball speed on X axis (%)
    uint8_t ballBounceSpeedIncrement = 10;      // Ball x-axis increment speed at each bounce (%)
    float minCloseLoopSpeed = 10.0f;            // Minimum speed for close loop movements (stud/second)
    float ballPaddleDistance = 0.2f;            // Game ball distance from the paddle before serving (stud)
    float paddleCollisionTolerance = 0.5f;      // Ball-paddle collision tolerance (stud)
    float bounceInversionOvershootAtSpeed[X_AXIS_BOUNCE_INVERSIONS_COUNT]    // Ball x-axis overshoot distance when a bounce inversion occurs (stud)
        = {0.0f};        
};

struct GameYAxisSettings {
    float minCloseLoopSpeed = 10.0f;            // Minimum speed for close loop movements (stud/second)
    uint8_t startSpeedRange = 50;               // Speed range for the random serving speed (%) 
    float paddleCollisionTolerance = 1.0f;      // Ball-paddle collision tolerance (stud)
    float bounceInversionOvershootAtSpeed[Y_AXIS_BOUNCE_INVERSIONS_COUNT]    // Ball y-axis overshoot distance when a bounce inversion occurs (stud)
        = {0.0f};        
};

struct GameSettings {
    GameXAxisSettings xAxis;
    GameYAxisSettings yAxis;

};
