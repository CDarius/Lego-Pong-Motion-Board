#pragma once

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "web_functions/web_function.hpp"
#include "motor_control/motorhoming.hpp"
#include "utils/task_runner.hpp"
#include "utils/cancel_token.hpp"
#include "utils/logger.hpp"

class WebFunctionAxisSetZero : public WebFunction{
private:
    IMotorHoming& _axis;

public:
    // Constructor that takes and stores an IMotorHoming reference
    WebFunctionAxisSetZero(IMotorHoming& axis) : _axis(axis) {};

    // Override methods as needed
    const char* getName() const override;
    const char* getTitle() const override;
    const char* getDescription() const override;
    uint16_t getPrerequisitesCount() const override;
    const char* getPrerequisiteDescription(uint16_t index) const override;

    void arePrerequisitesMet(bool* results) const override;
    WebFunctionExecutionStatus start() override;
    void stop() override;
};
