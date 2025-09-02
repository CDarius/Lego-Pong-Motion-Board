#pragma once

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "web_functions/web_function.hpp"
#include "motor_control/motorhoming.hpp"
#include "utils/task_runner.hpp"
#include "utils/cancel_token.hpp"
#include "utils/logger.hpp"
#include "config.h"

#define MAX_SPEED_TEST_NUM_ITERATIONS (3)

class WebFunctionAxisMaxSpeed : public WebFunction{
private:
    IMotorHoming& _axis;
    TaskRunner& _taskRunner;
    TaskHandle_t _taskHandle = nullptr;
    CancelToken* _cancelToken = nullptr;
    
public:
    // Constructor that takes and stores an IMotorHoming reference
    WebFunctionAxisMaxSpeed(IMotorHoming& axis, TaskRunner& taskRunner, IOBoard& ioBoard) : _axis(axis), _taskRunner(taskRunner), WebFunction(ioBoard) {};

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
