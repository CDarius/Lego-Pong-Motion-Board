#pragma once

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "web_functions/web_function.hpp"
#include "motor_control/motorhoming.hpp"
#include "utils/task_runner.hpp"
#include "utils/cancel_token.hpp"
#include "utils/logger.hpp"
#include "devices/unit_encoder.hpp"

#define MIN_SPEED_SAFE_OVERSHOOT_DISTANCE (4.0f)  // Safe overshoot distance in studs

class WebFunctionGameMinCloseLoopSpeed : public WebFunction{
private:
    IMotorHoming& _axis;
    UnitEncoder& _encoder;
    float& _closeLoopMinSpeed;
    TaskRunner& _taskRunner;
    TaskHandle_t _taskHandle = nullptr;
    CancelToken* _cancelToken = nullptr;    
    
public:
    // Constructor that takes and stores an IMotorHoming reference
    WebFunctionGameMinCloseLoopSpeed(IMotorHoming& axis, float& closeLoopMinSpeed, UnitEncoder& encoder, TaskRunner& taskRunner, IOBoard& ioBoard) : 
        _axis(axis), 
        _closeLoopMinSpeed(closeLoopMinSpeed),
        _encoder(encoder),
        _taskRunner(taskRunner), WebFunction(ioBoard) {};

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
