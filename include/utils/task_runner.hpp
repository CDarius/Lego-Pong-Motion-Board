#pragma once

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "config.h"

// Define a callback type for instance methods
typedef void (*TaskMethod)(void*);

class TaskRunner {
public:
    TaskRunner(const char* taskName, UBaseType_t priority = OTHER_TASK_PRIORITY) : 
        _taskName(taskName), _priority(priority), _pendingMethod(nullptr), _pendingContext(nullptr) {}

    // Schedule an instance method to run in the task context
    void runAsync(TaskMethod method, void* context);

private:
    static void task_func(void* pv);

    const char* _taskName;
    UBaseType_t _priority;
    TaskHandle_t _taskHandle = nullptr;
    TaskMethod _pendingMethod;
    void* _pendingContext;
};