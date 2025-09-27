#pragma once

#include <Arduino.h>
#include "web_functions/web_function.hpp"
#include "motor_control/motorhoming.hpp"
#include "game/axes_homing.hpp"
#include "game/encodermultijog.hpp"
#include "utils/task_runner.hpp"
#include "utils/cancel_token.hpp"

class WebFunctionAxesHomeAll : public WebFunction {
    private:
        IMotorHoming& _xMotor;
        IMotorHoming& _yMotor;
        IMotorHoming& _lMotor;
        IMotorHoming& _rMotor;
        EncoderMultiJog& _lEncoderJog;
        EncoderMultiJog& _rEncoderJog;
        TaskRunner& _taskRunner;
        CancelToken* _cancelToken = nullptr;
        uint8_t _axisIndex = 0;

    public:
        WebFunctionAxesHomeAll(
            IMotorHoming& xMotor, IMotorHoming& yMotor, 
            IMotorHoming& lMotor, IMotorHoming& rMotor,
            EncoderMultiJog& lEncoderJog, EncoderMultiJog& rEncoderJog,
            TaskRunner& taskRunner, IOBoard& ioBoard)
            : WebFunction(ioBoard), 
            _xMotor(xMotor), _yMotor(yMotor), 
            _lMotor(lMotor), _rMotor(rMotor),
            _lEncoderJog(lEncoderJog), _rEncoderJog(rEncoderJog), 
            _taskRunner(taskRunner) {};

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
