#pragma once

#include "web_functions/web_function_group.hpp"
#include "web_function_axis_homing.hpp"
#include "web_function_axis_set_zero.hpp"
#include "web_function_axis_maxspeed.hpp"
#include "web_functions/axis/web_function_axis_stepresponse.hpp"
#include "motor_control/motorhoming.hpp"
#include "utils/task_runner.hpp"
#include "devices/io_board.hpp"

class WebFunctionGroupAxis : public WebFunctionGroup {
    private:
        IMotorHoming& _motor;        
        WebFunctionAxisHoming _homing = WebFunctionAxisHoming(_motor, _taskRunner, _ioboard);
        WebFunctionAxisSetZero _setZero = WebFunctionAxisSetZero(_motor, _ioboard);
        WebFunctionAxisMaxSpeed _maxSpeed = WebFunctionAxisMaxSpeed(_motor, _taskRunner, _ioboard);
        WebFunctionAxisStepResponse _stepResponse = WebFunctionAxisStepResponse(_motor, _taskRunner, _ioboard);

        WebFunction* _functions[4] = {&_homing, &_setZero, &_maxSpeed, &_stepResponse};

    public:
        WebFunctionGroupAxis(const char* name, const char* title, IOBoard& ioboard, TaskRunner& taskRunner, IMotorHoming& motor)
            : _motor(motor),  WebFunctionGroup(name, title, ioboard, taskRunner) {}

        WebFunction** getFunctions() override {
            return _functions;
        }

        uint16_t getFunctionsCount() const override {
            return sizeof(_functions) / sizeof(WebFunction*);
        }
};
