#pragma once

#include "web_function_group.hpp"
#include "axes/web_function_group_axes.hpp"
#include "axis/web_function_group_axis.hpp"
#include "utils/task_runner.hpp"
#include "devices/io_board.hpp"

class WebFunctions {
    private:
        TaskRunner _taskRunner = TaskRunner("web_functions_task_runner");

        IOBoard& _ioBoard;
        EncoderMultiJog& _encoderJog;
        IMotorHoming& _YMotor;
        IMotorHoming& _LMotor;
        IMotorHoming& _RMotor;
        WebFunctionGroupAxes _axesGroup = WebFunctionGroupAxes(_ioBoard, _taskRunner, _encoderJog);
        WebFunctionGroupAxis _yAxisGroup = WebFunctionGroupAxis("y_axis", "Y-Axis", _ioBoard, _taskRunner, _YMotor);
        WebFunctionGroupAxis _lAxisGroup = WebFunctionGroupAxis("l_axis", "L-Axis", _ioBoard, _taskRunner, _LMotor);
        WebFunctionGroupAxis _rAxisGroup = WebFunctionGroupAxis("r_axis", "R-Axis", _ioBoard, _taskRunner, _RMotor);

        WebFunctionGroup* _groups[4] = { &_axesGroup, &_yAxisGroup, &_lAxisGroup, &_rAxisGroup };
        uint16_t _groupsCount = sizeof(_groups) / sizeof(WebFunctionGroup*);

    public:
        WebFunctions(IOBoard& ioBoard, EncoderMultiJog& encoderJog, IMotorHoming& YMotor, IMotorHoming& LMotor, IMotorHoming& RMotor)
            : _ioBoard(ioBoard), _encoderJog(encoderJog), _YMotor(YMotor), _LMotor(LMotor), _RMotor(RMotor) {}

        WebFunctionGroup* getGroup(const char* name);

        WebFunctionGroup** getGroups();
        uint16_t getGroupsCount() const;
};
