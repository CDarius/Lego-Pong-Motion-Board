#pragma once

#include <Arduino.h>
#include "devices/io_board.hpp"

enum class WebFunctionExecutionStatus {
    Done,
    InProgress,
    Failed
};

class WebFunction {
protected:
    IOBoard& _ioBoard;
    WebFunctionExecutionStatus _status = WebFunctionExecutionStatus::Done;
    const char* _failureDescription = nullptr;    

    WebFunction(IOBoard& ioBoard) : _ioBoard(ioBoard) {}

public:
    virtual const char* getName() const = 0;
    virtual const char* getTitle() const = 0;
    virtual const char* getDescription() const = 0;
    virtual uint16_t getPrerequisitesCount() const = 0;
    virtual const char* getPrerequisiteDescription(uint16_t index) const = 0;

    virtual void arePrerequisitesMet(bool* results) const = 0;
    bool areAllPrerequisitesMet() const;
    virtual WebFunctionExecutionStatus start();
    virtual void stop() = 0;
    
    WebFunctionExecutionStatus getRunningStatus() const {
        return _status;
    }

    const char* getFailuerDescription() const {
        return _failureDescription;
    }
    
};
