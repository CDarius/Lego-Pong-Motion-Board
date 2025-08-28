#pragma once

#include "web_functions/web_function_group.hpp"
#include "web_functions/game/web_function_game_bounce.hpp"
#include "motor_control/motorhoming.hpp"
#include "utils/task_runner.hpp"
#include "devices/io_board.hpp"
#include "game/game_settings.hpp"

class WebFunctionGroupGameY : public WebFunctionGroup {
    private:
        IMotorHoming& _motor;
        GameYAxisSettings& _gameSettings;
        WebFunctionGameBounce _bounce = WebFunctionGameBounce(_motor, _gameSettings.bounceInversionOvershootAtSpeed, _taskRunner, _ioboard);

        WebFunction* _functions[1] = {&_bounce};

    public:
        WebFunctionGroupGameY(IOBoard& ioboard, TaskRunner& taskRunner, IMotorHoming& motor, GameYAxisSettings& gameSettings) :
            _motor(motor), _gameSettings(gameSettings), WebFunctionGroup("game_y", "Game Y", ioboard, taskRunner) {}

        WebFunction** getFunctions() override {
            return _functions;
        }

        uint16_t getFunctionsCount() const override {
            return sizeof(_functions) / sizeof(WebFunction*);
        }
};
