#pragma once

#include "web_functions/web_function_group.hpp"
#include "web_function_axes_jog.hpp"
#include "utils/task_runner.hpp"
#include "game/encodemultijog.hpp"
#include "devices/io_board.hpp"

class WebFunctionGroupAxes : public WebFunctionGroup {
    private:
        EncoderMultiJog& _encoderJog;        
        WebFunctionAxesJog _axesJog = WebFunctionAxesJog(_encoderJog, _taskRunner, _ioboard);

        WebFunction* _functions[1] = {&_axesJog};

    public:
        WebFunctionGroupAxes(IOBoard& ioboard, TaskRunner& taskRunner, EncoderMultiJog& encoderJog)
            : _encoderJog(encoderJog), WebFunctionGroup("axes", "Axes", ioboard, taskRunner) {}

        WebFunction** getFunctions() override {
            return _functions;
        }

        uint16_t getFunctionsCount() const override {
            return sizeof(_functions) / sizeof(WebFunction*);
        }
};
