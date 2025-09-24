#pragma once

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "web_functions/web_function.hpp"
#include "motor_control/motorhoming.hpp"
#include "motor_control/logger.hpp"
#include "utils/task_runner.hpp"
#include "utils/cancel_token.hpp"
#include "utils/logger.hpp"
#include "config.h"
#include "monotonic.h"

#define RUN_STEP_RESPONSE_LOG_DURATION_MS (20000)
#define RUN_STEP_RESPONSE_SETTLE_TIME_MS (500)

class WebFunctionAxisStepResponse : public WebFunction{
private:
    IMotorHoming& _axis;
    TaskRunner& _taskRunner;
    TaskHandle_t _taskHandle = nullptr;
    CancelToken* _cancelToken = nullptr;
    
public:
    // Constructor that takes and stores an IMotorHoming reference
    WebFunctionAxisStepResponse(IMotorHoming& axis, TaskRunner& taskRunner, IOBoard& ioBoard) : _axis(axis), _taskRunner(taskRunner), WebFunction(ioBoard) {};

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
