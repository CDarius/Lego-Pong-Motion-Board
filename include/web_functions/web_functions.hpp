#pragma once

#include "web_functions/web_function_group.hpp"
#include "web_functions/axis/web_function_group_axis.hpp"
#include "utils/task_runner.hpp"
#include "devices/io_board.hpp"

class WebFunctions {
    private:
        TaskRunner _taskRunner = TaskRunner("web_functions_task_runner");

        IOBoard& _ioBoard;
        IMotorHoming& _YMotor;
        IMotorHoming& _LMotor;
        IMotorHoming& _RMotor;
        WebFunctionGroupAxis _yAxisGroup = WebFunctionGroupAxis("y_axis", "Y-Axis", _ioBoard, _taskRunner, _YMotor);
        WebFunctionGroupAxis _lAxisGroup = WebFunctionGroupAxis("l_axis", "L-Axis", _ioBoard, _taskRunner, _LMotor);
        WebFunctionGroupAxis _rAxisGroup = WebFunctionGroupAxis("r_axis", "R-Axis", _ioBoard, _taskRunner, _RMotor);

        WebFunctionGroup* _groups[3] = {&_yAxisGroup, &_lAxisGroup, &_rAxisGroup};
        uint16_t _groupsCount = sizeof(_groups) / sizeof(WebFunctionGroup*);

    public:
        WebFunctions(IOBoard& ioBoard, IMotorHoming& YMotor, IMotorHoming& LMotor, IMotorHoming& RMotor)
            : _ioBoard(ioBoard), _YMotor(YMotor), _LMotor(LMotor), _RMotor(RMotor) {}

        WebFunctionGroup* getGroup(const char* name);

        WebFunctionGroup** getGroups();
        uint16_t getGroupsCount() const;
};
