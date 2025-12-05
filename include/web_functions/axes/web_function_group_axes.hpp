#pragma once

#include "web_functions/web_function_group.hpp"
#include "web_function_axes_jog.hpp"
#include "web_function_axes_homeall.hpp"
#include "utils/task_runner.hpp"
#include "game/axes_homing.hpp"
#include "game/encodermultijog.hpp"
#include "devices/io_board.hpp"

class WebFunctionGroupAxes : public WebFunctionGroup {
    private:
        IMotorHoming& _xMotor;
        IMotorHoming& _yMotor;
        IMotorHoming& _lMotor;
        IMotorHoming& _rMotor;
        EncoderMultiJog& _lEncoderJog;
        EncoderMultiJog& _rEncoderJog;

        WebFunctionAxesJog _axesJog = WebFunctionAxesJog(_lEncoderJog, _taskRunner, _ioboard);
        WebFunctionAxesHomeAll _axesHomeAll = WebFunctionAxesHomeAll(_xMotor, _yMotor, _lMotor, _rMotor, _lEncoderJog, _rEncoderJog, _taskRunner, _ioboard);

        WebFunction* _functions[2] = {&_axesJog, &_axesHomeAll};

    public:
        WebFunctionGroupAxes(
            IOBoard& ioboard, TaskRunner& taskRunner, 
            IMotorHoming& xMotor, IMotorHoming& yMotor, 
            IMotorHoming& lMotor, IMotorHoming& rMotor,
            EncoderMultiJog& lEncoderJog, EncoderMultiJog& rEncoderJog)
            : _xMotor(xMotor), _yMotor(yMotor), _lMotor(lMotor), _rMotor(rMotor),
              _lEncoderJog(lEncoderJog), _rEncoderJog(rEncoderJog),
              WebFunctionGroup("axes", "Axes", ioboard, taskRunner) {}

        WebFunction** getFunctions() override {
            return _functions;
        }

        uint16_t getFunctionsCount() const override {
            return sizeof(_functions) / sizeof(WebFunction*);
        }
};
