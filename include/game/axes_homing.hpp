#pragma once

#include <Arduino.h>
#include "config.h"
#include "axes.hpp"
#include "utils/cancel_token.hpp"
#include "motor_control/motorhoming.hpp"
#include "devices/io_board.hpp"
#include "encodermultijog.hpp"
#include "motor_control/error.hpp"

pbio_error_t homeAllAxes(
    IMotorHoming& xMotor,
    IMotorHoming& yMotor,
    IMotorHoming& lMotor,
    IMotorHoming& rMotor,
    IOBoard& ioBoard,
    EncoderMultiJog& lEncoderJog,
    EncoderMultiJog& rEncoderJog,
    uint8_t startButtonPin);