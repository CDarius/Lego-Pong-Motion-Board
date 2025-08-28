#pragma once

#include "web_functions/web_function_group.hpp"
#include "web_functions/game/web_function_game_bounce.hpp"
#include "motor_control/motorhoming.hpp"
#include "utils/task_runner.hpp"
#include "devices/io_board.hpp"
#include "game/game_settings.hpp"

class WebFunctionGroupGameX : public WebFunctionGroup {
    private:
        IMotorHoming& _motor;
        GameXAxisSettings& _gameSettings;
        WebFunctionGameBounce _bounce = WebFunctionGameBounce(_motor, _gameSettings.bounceInversionOvershootAtSpeed, _taskRunner, _ioboard);

        WebFunction* _functions[1] = {&_bounce};

    public:
        WebFunctionGroupGameX(IOBoard& ioboard, TaskRunner& taskRunner, IMotorHoming& motor, GameXAxisSettings& gameSettings) :
            _motor(motor), _gameSettings(gameSettings), WebFunctionGroup("game_x", "Game X", ioboard, taskRunner) {}

        WebFunction** getFunctions() override {
            return _functions;
        }

        uint16_t getFunctionsCount() const override {
            return sizeof(_functions) / sizeof(WebFunction*);
        }
};
