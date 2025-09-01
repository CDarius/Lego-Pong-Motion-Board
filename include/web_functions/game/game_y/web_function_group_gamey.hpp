#pragma once

#include "web_functions/web_function_group.hpp"
#include "web_functions/game/web_function_game_bounce.hpp"
#include "web_functions/game/web_function_game_mincloseloopspeed.hpp"
#include "web_functions/game/web_function_game_openloopspeedtest.hpp"
#include "motor_control/motorhoming.hpp"
#include "utils/task_runner.hpp"
#include "devices/io_board.hpp"
#include "devices/unit_encoder.hpp"
#include "game/game_settings.hpp"

class WebFunctionGroupGameY : public WebFunctionGroup {
    private:
        IMotorHoming& _motor;
        UnitEncoder& _encoder;
        GameYAxisSettings& _gameSettings;
        WebFunctionGameBounce _bounce = WebFunctionGameBounce(_motor, _gameSettings.bounceInversionOvershootAtSpeed, _taskRunner, _ioboard);
        WebFunctionGameMinCloseLoopSpeed _minCloseLoopSpeed = WebFunctionGameMinCloseLoopSpeed(_motor, _gameSettings.minCloseLoopSpeed, _encoder, _taskRunner, _ioboard);
        WebFunctionGameOpenLoopSpeedTest _openLoopSpeedTest = WebFunctionGameOpenLoopSpeedTest(_motor, _encoder, _taskRunner, _ioboard);

        WebFunction* _functions[3] = {&_bounce, &_minCloseLoopSpeed, &_openLoopSpeedTest};

    public:
        WebFunctionGroupGameY(IOBoard& ioboard, TaskRunner& taskRunner, IMotorHoming& motor, GameYAxisSettings& gameSettings, UnitEncoder& encoder) :
            _motor(motor), _gameSettings(gameSettings), _encoder(encoder), WebFunctionGroup("game_y", "Game Y", ioboard, taskRunner) {}

        WebFunction** getFunctions() override {
            return _functions;
        }

        uint16_t getFunctionsCount() const override {
            return sizeof(_functions) / sizeof(WebFunction*);
        }
};
