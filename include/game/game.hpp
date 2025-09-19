#pragma once

#include "config.h"
#include "axes.hpp"
#include "game_settings.hpp"
#include "motor_control/motorhoming.hpp"
#include "motor_control/error.hpp"
#include "devices/io_board.hpp"
#include "devices/button.hpp"
#include "encodermultijog.hpp"
#include "utils/cancel_token.hpp"

#define GAME_PADDLE_H   3.0
#define GAME_PADDLE_W   2.0
#define GAME_BALL_L     2.0

#define GAME_PADDLE_BALL_X_DIST_COLUMN   2.0

enum class GamePlayer {
    L,
    R
};

enum class GameMode {
    PLAYER_VS_PLAYER,
    PLAYER_VS_AI,
    AI_VS_AI
};

#define OTHER_GAME_PLAYER(player) ((player) == GamePlayer::L ? GamePlayer::R : GamePlayer::L)
#define GAME_WIN_SCORE (3)

class Game {
    private:
        GameSettings _settings;
        IMotorHoming& _xMotor;
        IMotorHoming& _yMotor;
        IMotorHoming& _lMotor;
        IMotorHoming& _rMotor;
        IOBoard& _ioBoard;
        EncoderMultiJog _lEncoderJog;
        EncoderMultiJog _rEncoderJog;

        uint8_t _scoreL = 0;
        uint8_t _scoreR = 0;

        // Game motion state
        float _ballX = 0.0f;
        float _ballY = 0.0f;
        float _paddleL = 0.0f;
        float _paddleR = 0.0f;
        float _speedX = 0.0f;
        float _speedY = 0.0f;
        float _overshootX = 0.0f;
        float _overshootY = 0.0f;

        // AI player state
        unsigned long _lastAIUpdateTime = 0;
        bool _lPlayerIsAI;
        bool _rPlayerIsAI;
        
        // Move the ball in front of the player paddle
        pbio_error_t moveBallToPaddle(GamePlayer player, CancelToken& cancelToken);
        // Make ball track the paddle vertical position
        void ballTrackPaddle(IMotorHoming& axis);
        // Return true if the ball is on the specified player's paddle column (ball X is near the paddle)
        bool isBallOnThePaddleColumn(GamePlayer player) const;
        // Make a ball close loop interpolated movement
        pbio_error_t moveBallCloseLoop(float x, float y, CancelToken& cancelToken, float speedX = NAN, float speedY = NAN);
        // Human player serve the ball from the paddle
        pbio_error_t humanPlayerServeBall(GamePlayer player, CancelToken& cancelToken);
        // AI player serve the ball from the paddle
        pbio_error_t aiPlayerServeBall(GamePlayer player, CancelToken& cancelToken);
        // Throw the ball from player's paddle
        void throwBall(GamePlayer player);
        // Bounce the ball on top or bottom border
        void bounceBallTopBottom();
        // Test if the ball has reached the paddle on X axis. When return true we must then check the Y axis to understand if the ball is also at the paddle height
        bool isBallAtPaddleBounceLimit() const;
        // Test if the ball is within the paddle's Y-axis range and if it is bounce
        bool bounceOnPaddle();
        // Calculate the travel overshoot when the ball speed X is inverted
        float getXInversionOvershoot(float speed) const;
        // Calculate the travel overshoot when the ball speed Y is inverted
        float getYInversionOvershoot(float speed) const;
        // AI player logic
        void aiPlayer(GamePlayer player);

    public:
        Game(
            IMotorHoming& xMotor,
            IMotorHoming& yMotor,
            IMotorHoming& lMotor,
            IMotorHoming& rMotor,
            IOBoard& ioBoard,
            EncoderMultiJog& lEncoderJog,
            EncoderMultiJog& rEncoderJog
        ) : _xMotor(xMotor), _yMotor(yMotor), _lMotor(lMotor), _rMotor(rMotor), _ioBoard(ioBoard), _lEncoderJog(lEncoderJog), _rEncoderJog(rEncoderJog) {}

        GameSettings *getSettings() {
            return &_settings;
        }

        pbio_error_t run(GamePlayer startPlayer, GameMode mode);
};
