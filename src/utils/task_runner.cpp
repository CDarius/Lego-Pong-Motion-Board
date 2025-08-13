#include "utils/task_runner.hpp"

void TaskRunner::runAsync(TaskMethod method, void* context) {
    // Lazy create the task at the first run request
    if (_taskHandle == nullptr) {
        xTaskCreatePinnedToCore(
            task_func,              // Function to run
            _taskName,              // Task name
            4096,                   // Stack size
            this,                   // Task parameter
            OTHER_TASK_PRIORITY,    // Task priority
            &_taskHandle,           // Task handle
            OTHER_TASK_CORE         // Core where the task should run
        );
    }
    _pendingMethod = method;
    _pendingContext = context;
    xTaskNotifyGive(_taskHandle);
}


void TaskRunner::task_func(void* pv) {
    TaskRunner* self = static_cast<TaskRunner*>(pv);
    for (;;) {
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
        if (self->_pendingMethod) {
            self->_pendingMethod(self->_pendingContext);
            self->_pendingMethod = nullptr;
            self->_pendingContext = nullptr;
        }
    }
}