#pragma once

enum class WebFunctionExecutionStatus {
    Done,
    InProgress,
    Failed
};

class WebFunction {
protected:
    WebFunction() {}
    WebFunctionExecutionStatus _status = WebFunctionExecutionStatus::Done;
    const char* _failureDescription = nullptr;    

public:
    virtual const char* getName() const = 0;
    virtual const char* getTitle() const = 0;
    virtual const char* getDescription() const = 0;
    virtual const char* getPrerequisitesDescription() const = 0;

    virtual bool shouldSaveResult() const = 0;
    virtual void saveResult()= 0;
    
    virtual bool arePrerequisitesMet() const = 0;
    virtual WebFunctionExecutionStatus start() = 0;
    virtual void stop() = 0;
    
    WebFunctionExecutionStatus getRunningStatus() const {
        return _status;
    }

    const char* getFailuerDescription() const {
        return _failureDescription;
    }
    
};
