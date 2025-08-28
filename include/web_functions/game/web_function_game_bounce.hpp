#pragma once

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "web_functions/web_function.hpp"
#include "motor_control/motorhoming.hpp"
#include "utils/task_runner.hpp"
#include "utils/cancel_token.hpp"
#include "utils/logger.hpp"
#include "game/game_settings.hpp"

#define BOUNCE_SAFE_OVERSHOOT_DISTANCE (6)  // Safe overshoot distance in studs

class WebFunctionGameBounce : public WebFunction{
private:
    IMotorHoming& _axis;
    TaskRunner& _taskRunner;
    TaskHandle_t _taskHandle = nullptr;
    CancelToken* _cancelToken = nullptr;
    float* _bounceInversionOvershootAtSpeed;

    pbio_error_t _runBounceTest(uint8_t speedCmdPercentage, float& overshoot, CancelToken& cancel_token);
    
public:
    // Constructor that takes and stores an IMotorHoming reference
    WebFunctionGameBounce(IMotorHoming& axis, float* bounceInversionOvershootAtSpeed, TaskRunner& taskRunner, IOBoard& ioBoard) : 
        _axis(axis), _bounceInversionOvershootAtSpeed(bounceInversionOvershootAtSpeed), 
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
