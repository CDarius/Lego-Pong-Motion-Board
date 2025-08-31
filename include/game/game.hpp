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

#define OTHER_GAME_PLAYER(player) ((player) == GamePlayer::L ? GamePlayer::R : GamePlayer::L)

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

        // Move the ball in front of the player paddle
        pbio_error_t moveBallToPaddle(GamePlayer player, CancelToken& cancelToken);
        // Make ball track the paddle vertical position
        void ballTrackPaddle(IMotorHoming& axis);
        // Return true if the ball is on the specified player's paddle column (ball X is near the paddle)
        bool isBallOnThePaddleColumn(GamePlayer player) const;
        // Make a ball interpolated movement
        pbio_error_t moveBall(float x, float y, CancelToken& cancelToken);


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

        pbio_error_t run(GamePlayer startPlayer);
};
