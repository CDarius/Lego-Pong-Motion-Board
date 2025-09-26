#pragma once

#include "web_functions/web_function_group.hpp"
#include "web_functions/game/web_function_game_log.hpp"
#include "utils/task_runner.hpp"
#include "devices/io_board.hpp"
#include "devices/unit_encoder.hpp"
#include "game/game.hpp"
#include "motor_control/motorhoming.hpp"

class WebFunctionGroupGame : public WebFunctionGroup {
    private:
        Game& _game;
        IMotorHoming& _xAxis;
        IMotorHoming& _yAxis;
        IMotorHoming& _lAxis;
        IMotorHoming& _rAxis;

        WebFunctionGameLog _log_pl_pl = WebFunctionGameLog(
            GameMode::PLAYER_VS_PLAYER, _game, 
            _xAxis, _yAxis, _lAxis, _rAxis, 
            _taskRunner, _ioboard);
        WebFunctionGameLog _log_pl_ai = WebFunctionGameLog(
            GameMode::PLAYER_VS_AI, _game, 
            _xAxis, _yAxis, _lAxis, _rAxis, 
            _taskRunner, _ioboard);
        WebFunctionGameLog _log_ai_ai = WebFunctionGameLog(
            GameMode::AI_VS_AI, _game, 
            _xAxis, _yAxis, _lAxis, _rAxis, 
            _taskRunner, _ioboard);

        WebFunction* _functions[3] = {&_log_pl_pl, &_log_pl_ai, &_log_ai_ai};

    public:
        WebFunctionGroupGame(
            IOBoard& ioboard, TaskRunner& taskRunner, 
            Game& game, IMotorHoming& xAxis, IMotorHoming& yAxis, 
            IMotorHoming& lAxis, IMotorHoming& rAxis) :
            _game(game), _xAxis(xAxis), _yAxis(yAxis), 
            _lAxis(lAxis), _rAxis(rAxis), 
            WebFunctionGroup("game", "Game", ioboard, taskRunner) {}

        WebFunction** getFunctions() override {
            return _functions;
        }

        uint16_t getFunctionsCount() const override {
            return sizeof(_functions) / sizeof(WebFunction*);
        }
};
