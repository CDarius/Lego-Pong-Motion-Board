#pragma once

#include <Arduino.h>
#include "web_function.hpp"

class WebFunctionGroup {
    public:
        virtual const char* getName() const = 0;
        virtual const char* getTitle() const = 0;

        virtual WebFunction** getFunctions() = 0;
        virtual uint16_t getFunctionsCount() const = 0;

        WebFunction* getFunction(const char* name);
};
