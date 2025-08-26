#pragma once

#include "game_settings.hpp"
#include "motor_control/motorhoming.hpp"
#include "devices/io_board.hpp"
#include "encodermultijog.hpp"

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
};
