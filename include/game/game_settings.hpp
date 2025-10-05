#pragma once

#include <Arduino.h>

#define X_AXIS_BOUNCE_INVERSIONS_COUNT 5
#define X_AXIS_BOUNCE_INVERSIONS_SPEEDS {10.0f, 15.0f, 20.0f, 25.0f, 30.0f}
#define Y_AXIS_BOUNCE_INVERSIONS_COUNT 8
#define Y_AXIS_BOUNCE_INVERSIONS_SPEEDS {5.0f, 10.0f, 15.0f, 20.0f, 25.0f, 30.0f, 35.0f, 40.0f}

struct GameXAxisSettings {
    float startBallGameSpeed = 6.0f;            // Game start ball speed on X axis (stud/second)
    float ballBounceSpeedIncrement = 5.0f;      // Ball x-axis increment speed at each bounce (stud/second)
    float ballBounceMaxSpeed = 35.0f;           // Ball x-axis maximum bounce speed (stud/second)
    float minCloseLoopSpeed = 10.0f;            // Minimum speed for close loop movements (stud/second)
    float ballPaddleDistance = 0.2f;            // Game ball distance from the paddle before serving (stud)
    float paddleCollisionTolerance = 0.5f;      // Ball-paddle collision tolerance (stud)
    float bounceInversionOvershootAtSpeed[X_AXIS_BOUNCE_INVERSIONS_COUNT]    // Ball x-axis overshoot distance when a bounce inversion occurs (stud)
        = {0.0f};        
};

struct GameYAxisSettings {
    float minCloseLoopSpeed = 10.0f;            // Minimum speed for close loop movements (stud/second)
    float ballBounceSpeedMax = 30.0f;           // Ball y-axis maximum bounce speed (stud/second)
    float ballServeSpeedMax = 4.0f;             // Ball y-axis maximum serve speed (stud/second)
    float paddleCollisionTolerance = 1.0f;      // Ball-paddle collision tolerance (stud)
    float bounceInversionOvershootAtSpeed[Y_AXIS_BOUNCE_INVERSIONS_COUNT]    // Ball y-axis overshoot distance when a bounce inversion occurs (stud)
        = {0.0f};        
};

struct GameLRAxisSettings {
    float minJogEncoderMultiplier = 0.2f;       // Minimum jog encoder multiplier for left/right paddles (stud/count)
    float jogMultiplierDecrement = 0.2f;        // Jog encoder multiplier decrement step for left/right paddles (stud/count)
};

struct GameAIPlayerSettings {
    float paddleMaxSpeed = 32.0f;               // Maximum paddle speed for AI player (stud/second)
    float paddleMaxError = 5.0f;                // Maximum paddle position error for AI player (stud)
    uint16_t playerUpdateTimeMs = 300;          // AI player update time (ms)
};

struct GameSettings {
    GameXAxisSettings xAxis;
    GameYAxisSettings yAxis;
    GameLRAxisSettings lrAxis;
    GameAIPlayerSettings aiPlayer;
};
