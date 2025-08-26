#pragma once

#include <Arduino.h>
#include "web_functions/web_function.hpp"
#include "game/axes.hpp"
#include "game/encodermultijog.hpp"
#include "devices/button.hpp"
#include "utils/task_runner.hpp"
#include "utils/cancel_token.hpp"

class WebFunctionAxesJog : public WebFunction {
    private:
        EncoderMultiJog& _encoderJog;
        TaskRunner& _taskRunner;
        CancelToken* _cancelToken = nullptr;
        uint8_t _axisIndex = 0;

        void startJog(uint8_t axisIndex);

    public:
        WebFunctionAxesJog(EncoderMultiJog& encoderJog, TaskRunner& taskRunner, IOBoard& ioBoard)
            : WebFunction(ioBoard), _encoderJog(encoderJog), _taskRunner(taskRunner) {};

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
