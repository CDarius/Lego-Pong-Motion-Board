#pragma once

#include "web_functions/web_function_group.hpp"
#include "web_function_axis_homing.hpp"
#include "web_function_axis_set_zero.hpp"
#include "utils/task_runner.hpp"
#include "motor_control\motorhoming.hpp"

class WebFunctionGroupAxis : public WebFunctionGroup {
    private:
        const char* _name;
        const char* _title;
        TaskRunner& _taskRunner;

        IMotorHoming& _motor;
        WebFunctionAxisHoming _homing = WebFunctionAxisHoming(_motor, _taskRunner);
        WebFunctionAxisSetZero _setZero = WebFunctionAxisSetZero(_motor);

        WebFunction* _functions[2] = {&_homing, &_setZero};

    public:
        WebFunctionGroupAxis(const char* name, const char* title, IMotorHoming& motor, TaskRunner& taskRunner)
            : _name(name), _title(title), _motor(motor), _taskRunner(taskRunner) {}

        const char* getName() const {
            return _name;
        }

        const char* getTitle() const {
            return _title;
        }

        WebFunction** getFunctions() override {
            return _functions;
        }

        uint16_t getFunctionsCount() const override {
            return sizeof(_functions) / sizeof(WebFunction*);
        }
};
