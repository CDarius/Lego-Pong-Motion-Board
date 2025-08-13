#pragma once

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "web_functions/web_function.hpp"
#include "motor_control/motorhoming.hpp"
#include "utils/task_runner.hpp"
#include "utils/cancel_token.hpp"
#include "utils/logger.hpp"

class WebFunctionAxisHoming : public WebFunction{
private:
    IMotorHoming& _axis;
    TaskRunner& _taskRunner;
    TaskHandle_t _taskHandle = nullptr;
    CancelToken* _cancelToken = nullptr;

public:
    // Constructor that takes and stores an IMotorHoming reference
    WebFunctionAxisHoming(IMotorHoming& axis, TaskRunner& taskRunner) : _axis(axis), _taskRunner(taskRunner) {};

    // Override methods as needed
    const char* getName() const override;
    const char* getTitle() const override;
    const char* getDescription() const override;
    const char* getPrerequisitesDescription() const override;

    bool shouldSaveResult() const override {
        return false; // No result to save for homing
    };
    void saveResult() override {}

    bool arePrerequisitesMet() const override;
    WebFunctionExecutionStatus start() override;
    void stop() override;
};
