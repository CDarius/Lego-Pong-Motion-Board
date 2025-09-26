#pragma once

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "web_functions/web_function.hpp"
#include "utils/task_runner.hpp"
#include "utils/cancel_token.hpp"
#include "utils/logger.hpp"
#include "game/game.hpp"
#include "motor_control/motorhoming.hpp"

class WebFunctionGameLog : public WebFunction{
private:
    GameMode _mode;
    Game& _game;
    IMotorHoming& _xAxis;
    IMotorHoming& _yAxis;
    IMotorHoming& _lAxis;
    IMotorHoming& _rAxis;
    TaskRunner& _taskRunner;
    TaskHandle_t _taskHandle = nullptr;
    CancelToken* _cancelToken = nullptr;
    float* _bounceInversionOvershootAtSpeed;

public:
    // Constructor that takes and stores a Game reference
    WebFunctionGameLog(
        GameMode mode, Game& game, 
        IMotorHoming& xAxis, IMotorHoming& yAxis, 
        IMotorHoming& lAxis, IMotorHoming& rAxis, 
        TaskRunner& taskRunner, IOBoard& ioBoard) : 
        _mode(mode), _game(game), _xAxis(xAxis), _yAxis(yAxis), 
        _lAxis(lAxis), _rAxis(rAxis), 
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
