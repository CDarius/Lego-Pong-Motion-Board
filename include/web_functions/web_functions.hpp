#pragma once

#include "web_function_group.hpp"
#include "game/web_function_group_game.hpp"
#include "game/game_x/web_function_group_gamex.hpp"
#include "game/game_y/web_function_group_gamey.hpp"
#include "axes/web_function_group_axes.hpp"
#include "axis/web_function_group_axis.hpp"
#include "utils/task_runner.hpp"
#include "devices/io_board.hpp"
#include "game/game.hpp"
#include "config.h"

class WebFunctions {
    private:
        TaskRunner _taskRunner = TaskRunner("web_functions_task_runner", OTHER_TASK_HIGH_PRIORITY);

        IOBoard& _ioBoard;
        EncoderMultiJog& _lEncoderJog;
        EncoderMultiJog& _rEncoderJog;
        Game& _game;
        IMotorHoming& _XMotor;
        IMotorHoming& _YMotor;
        IMotorHoming& _LMotor;
        IMotorHoming& _RMotor;
        
        WebFunctionGroupGame _gameGroup = WebFunctionGroupGame(_ioBoard, _taskRunner, _game, _XMotor, _YMotor, _LMotor, _RMotor);
        WebFunctionGroupGameX _gameXGroup = WebFunctionGroupGameX(_ioBoard, _taskRunner, _XMotor, (*(_game.getSettings())).xAxis, *(_lEncoderJog.getEncoder()));
        WebFunctionGroupGameY _gameYGroup = WebFunctionGroupGameY(_ioBoard, _taskRunner, _YMotor, (*(_game.getSettings())).yAxis, *(_lEncoderJog.getEncoder()));
        WebFunctionGroupAxes _axesGroup = WebFunctionGroupAxes(_ioBoard, _taskRunner, _XMotor, _YMotor, _LMotor, _RMotor, _lEncoderJog, _rEncoderJog);
        WebFunctionGroupAxis _xAxisGroup = WebFunctionGroupAxis("x_axis", "X-Axis", _ioBoard, _taskRunner, _XMotor);
        WebFunctionGroupAxis _yAxisGroup = WebFunctionGroupAxis("y_axis", "Y-Axis", _ioBoard, _taskRunner, _YMotor);
        WebFunctionGroupAxis _lAxisGroup = WebFunctionGroupAxis("l_axis", "L-Axis", _ioBoard, _taskRunner, _LMotor);
        WebFunctionGroupAxis _rAxisGroup = WebFunctionGroupAxis("r_axis", "R-Axis", _ioBoard, _taskRunner, _RMotor);

        WebFunctionGroup* _groups[8] = {&_gameGroup, &_gameXGroup, &_gameYGroup, &_axesGroup, &_xAxisGroup, &_yAxisGroup, &_lAxisGroup, &_rAxisGroup };
        uint16_t _groupsCount = sizeof(_groups) / sizeof(WebFunctionGroup*);

    public:
        WebFunctions(
            IOBoard& ioBoard, EncoderMultiJog& lEncoderJog, EncoderMultiJog& rEncoderJog, 
            Game& game,
            IMotorHoming& XMotor, IMotorHoming& YMotor, 
            IMotorHoming& LMotor, IMotorHoming& RMotor)
            : _ioBoard(ioBoard), _lEncoderJog(lEncoderJog), _rEncoderJog(rEncoderJog), 
            _game(game), 
            _XMotor(XMotor), _YMotor(YMotor), _LMotor(LMotor), _RMotor(RMotor) {}

        WebFunctionGroup* getGroup(const char* name);

        WebFunctionGroup** getGroups();
        uint16_t getGroupsCount() const;

        // Returns true if any WebFunction in any group is InProgress
        bool checkAnyFunctionInProgress() const;
};
