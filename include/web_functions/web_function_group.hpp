#pragma once

#include <Arduino.h>
#include "utils/task_runner.hpp"
#include "web_function.hpp"

class WebFunctionGroup {
    protected:
        const char* _name;
        const char* _title;
        TaskRunner& _taskRunner;
        IOBoard& _ioboard;

        WebFunctionGroup(const char* name, const char* title, IOBoard& ioboard, TaskRunner& taskRunner)
            : _name(name), _title(title), _ioboard(ioboard), _taskRunner(taskRunner) {}

    public:
        const char* getName() const {
            return _name;
        }

        const char* getTitle() const {
            return _title;
        }

        virtual WebFunction** getFunctions() = 0;
        virtual uint16_t getFunctionsCount() const = 0;

        WebFunction* getFunction(const char* name);
};
