#pragma once

#include "web_function_group.hpp"
#include "game/game_x/web_function_group_gamex.hpp"
#include "game/game_y/web_function_group_gamey.hpp"
#include "axes/web_function_group_axes.hpp"
#include "axis/web_function_group_axis.hpp"
#include "utils/task_runner.hpp"
#include "devices/io_board.hpp"
#include "game/game_settings.hpp"
#include "config.h"

class WebFunctions {
    private:
        TaskRunner _taskRunner = TaskRunner("web_functions_task_runner", OTHER_TASK_HIGH_PRIORITY);

        IOBoard& _ioBoard;
        EncoderMultiJog& _encoderJog;
        GameSettings& _gameSettings;
        IMotorHoming& _XMotor;
        IMotorHoming& _YMotor;
        IMotorHoming& _LMotor;
        IMotorHoming& _RMotor;
        WebFunctionGroupGameX _gameXGroup = WebFunctionGroupGameX(_ioBoard, _taskRunner, _XMotor, _gameSettings.xAxis);
        WebFunctionGroupGameY _gameYGroup = WebFunctionGroupGameY(_ioBoard, _taskRunner, _YMotor, _gameSettings.yAxis);
        WebFunctionGroupAxes _axesGroup = WebFunctionGroupAxes(_ioBoard, _taskRunner, _encoderJog);
        WebFunctionGroupAxis _xAxisGroup = WebFunctionGroupAxis("x_axis", "X-Axis", _ioBoard, _taskRunner, _XMotor);
        WebFunctionGroupAxis _yAxisGroup = WebFunctionGroupAxis("y_axis", "Y-Axis", _ioBoard, _taskRunner, _YMotor);
        WebFunctionGroupAxis _lAxisGroup = WebFunctionGroupAxis("l_axis", "L-Axis", _ioBoard, _taskRunner, _LMotor);
        WebFunctionGroupAxis _rAxisGroup = WebFunctionGroupAxis("r_axis", "R-Axis", _ioBoard, _taskRunner, _RMotor);

        WebFunctionGroup* _groups[7] = {&_gameXGroup, &_gameYGroup, &_axesGroup, &_xAxisGroup, &_yAxisGroup, &_lAxisGroup, &_rAxisGroup };
        uint16_t _groupsCount = sizeof(_groups) / sizeof(WebFunctionGroup*);

    public:
        WebFunctions(IOBoard& ioBoard, EncoderMultiJog& encoderJog, GameSettings& gameSettings,
            IMotorHoming& XMotor, IMotorHoming& YMotor, IMotorHoming& LMotor, IMotorHoming& RMotor)
            : _ioBoard(ioBoard), _encoderJog(encoderJog), _gameSettings(gameSettings), _XMotor(XMotor), _YMotor(YMotor), _LMotor(LMotor), _RMotor(RMotor) {}

        WebFunctionGroup* getGroup(const char* name);

        WebFunctionGroup** getGroups();
        uint16_t getGroupsCount() const;

        // Returns true if any WebFunction in any group is InProgress
        bool checkAnyFunctionInProgress() const;
};
