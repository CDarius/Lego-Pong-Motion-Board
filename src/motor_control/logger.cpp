// SPDX-License-Identifier: MIT
// Copyright (c) 2018-2020 The Pybricks Authors

#include "motor_control/logger.hpp"
#include "config.h"
#include "motor_control/const.h"
#include "monotonic.h"
#include "esp_heap_caps.h"

PBIOLogger::PBIOLogger() {
    // Configure the logs for a servo
    static char *servo_col_names[] {
        (char *)"Time since start of maneuver",
        (char *)"Current position",
        (char *)"Current speed",
        (char *)"Current actuation type",
        (char *)"Current actuation value",
        (char *)"Position setpoint",
        (char *)"Speed setpoint",
        (char *)"Error: position for angle maneuver or else speed",
        (char *)"Accumulated position error"
    };
    _col_names = servo_col_names;
    static char *servo_col_units[] {
        (char *)"ms",
        (char *)"count",
        (char *)"count/s",
        (char *)"pbio_actuation_t",
        (char *)"duty steps",
        (char *)"count",
        (char *)"count/s",
        (char *)"count or count/s",
        (char *)"count"
    };
    _col_units = servo_col_units;
}

void PBIOLogger::delete_log() {
    // Free log if any
    if (_len > 0) {
        free(_data);
        _data = nullptr;
    }
    _sampled = 0;
    _skipped = 0;
    _len = 0;
    _active = false;
}

/*
 * Start the logger for a specific duration and sample division
 * log: Pointer to the logger to start
 * duration: Desired logging duration in ms
 * div: Number of calls to the logger per sample actually logged (1 = log every call, 2 = log every other call, etc.)
 * Returns ::PBIO_SUCCESS on success, or an error code if the logger could not be started
*/
pbio_error_t PBIOLogger::start(uint32_t duration, uint32_t div) {
    if (xSemaphoreTake(_xMutex, portMAX_DELAY)) {
        // Free any existing log
        delete_log();

        // Set number of calls to the logger per sample actually logged
        _sample_div = div > 0 ? div : 1;

        // Minimal log length
        uint32_t len = duration / PBIO_CONFIG_SERVO_PERIOD_MS / _sample_div;

        // Assert length is allowed
        if (len > PBIO_MAX_LOG_LEN) {
            xSemaphoreGive(_xMutex);
            return PBIO_ERROR_INVALID_ARG;
        }

        // Allocate memory for the logs
        _data = (int32_t*)heap_caps_malloc(len * PBIO_NUM_LOGGER_COLS * sizeof(int32_t), MALLOC_CAP_SPIRAM);
        if (_data == nullptr) {
            xSemaphoreGive(_xMutex);
            return PBIO_ERROR_FAILED;
        }

        // (re-)initialize logger status for this servo
        _len = len;
        _sampled = 0;
        _skipped = 0;
        _start = monotonic_us();
        _active = true;
        xSemaphoreGive(_xMutex);
    }

    return PBIO_SUCCESS;
}

uint32_t PBIOLogger::rows() {
    uint32_t sampled;
    if (xSemaphoreTake(_xMutex, portMAX_DELAY)) {
        sampled = _sampled;
        if (_data == nullptr)
            sampled = 0;
        xSemaphoreGive(_xMutex);
    }
    return sampled;
}

uint32_t PBIOLogger::cols() {
    uint32_t num_cols;
    if (xSemaphoreTake(_xMutex, portMAX_DELAY)) {
        num_cols = PBIO_NUM_LOGGER_COLS;
        xSemaphoreGive(_xMutex);
    }
    return num_cols;
}

void PBIOLogger::stop() {
    if (xSemaphoreTake(_xMutex, portMAX_DELAY)) {
        // Release the logger for re-use
        _active = false;
        xSemaphoreGive(_xMutex);
    }
}

pbio_error_t PBIOLogger::update(int32_t *buf) {
    if (xSemaphoreTake(_xMutex, portMAX_DELAY)) {
        // Log nothing if logger is inactive
        if (!_active) {
            xSemaphoreGive(_xMutex);
            return PBIO_SUCCESS;
        }

        // Skip logging if we are not yet at a multiple of sample_div
        if (++_skipped != _sample_div) {
            xSemaphoreGive(_xMutex);
            return PBIO_SUCCESS;
        }
        _skipped = 0;

        // Raise error if log is full, which should not happen
        if (_sampled > _len) {
            _active = false;
            xSemaphoreGive(_xMutex);
            return PBIO_ERROR_FAILED;
        }

        // Stop successfully when done
        if (_sampled == _len) {
            _active = false;
            xSemaphoreGive(_xMutex);
            return PBIO_SUCCESS;
        }

        // Write time of logging (ms)
        uint32_t start_index = _sampled * PBIO_NUM_LOGGER_COLS;
        _data[start_index] = (int32_t)((monotonic_us() - _start)/US_PER_MS);

        // Write the data
        for (uint8_t i = 0; i < _num_values; i++) {
            _data[start_index + i + PBIO_NUM_DEFAULT_LOG_VALUES] = buf[i];
        }

        // Increment sample counter
        _sampled++;
        xSemaphoreGive(_xMutex);
    }

    return PBIO_SUCCESS;
}

pbio_error_t PBIOLogger::read(int32_t sample_index, int32_t *buf) {
    if (xSemaphoreTake(_xMutex, portMAX_DELAY)) {
        // Validate index value
        if (sample_index < -1) {
            xSemaphoreGive(_xMutex);
            return PBIO_ERROR_INVALID_ARG;
        }

        // Get index or latest sample if requested index is -1
        uint32_t index = sample_index == -1 ? _sampled - 1 : sample_index;

        // Ensure index is within bounds
        if (index >= _sampled) {
            xSemaphoreGive(_xMutex);
            return PBIO_ERROR_INVALID_ARG;
        }

        // Read the data
        uint32_t start_index = index * PBIO_NUM_LOGGER_COLS;
        for (uint8_t i = 0; i < PBIO_NUM_LOGGER_COLS; i++) {
            buf[i] = _data[start_index + i];
        }

        xSemaphoreGive(_xMutex);
    }

    return PBIO_SUCCESS;
}

pbio_error_t PBIOLogger::read(uint32_t start_sample, uint32_t num_samples, int32_t *buf) {
    if (xSemaphoreTake(_xMutex, portMAX_DELAY)) {
        if (start_sample + num_samples > _sampled) {
            num_samples = _sampled - start_sample;
        }

        // Ensure index is within bounds
        if (start_sample >= _sampled) {
            xSemaphoreGive(_xMutex);
            return PBIO_ERROR_INVALID_ARG;
        }

        // Read the data
        uint32_t start_array_index = start_sample * PBIO_NUM_LOGGER_COLS;
        uint32_t num_array_indexes = num_samples * PBIO_NUM_LOGGER_COLS;
        for (uint32_t i = 0; i < num_array_indexes; i++) {
            buf[i] = _data[start_array_index + i];
        }

        xSemaphoreGive(_xMutex);
    }

    return PBIO_SUCCESS;
}

const char* PBIOLogger::col_name(uint8_t col) {
    if (xSemaphoreTake(_xMutex, portMAX_DELAY)) {
        if (col >= 0 && col < PBIO_NUM_DEFAULT_LOG_VALUES) {
            // Default columns
            switch (col) {
                case 0:
                    xSemaphoreGive(_xMutex);
                    return "Time";
                default:
                    xSemaphoreGive(_xMutex);
                    return nullptr;
            }
        }
        if ((col - PBIO_NUM_DEFAULT_LOG_VALUES) < _num_values) {
            char* col_name = _col_names[col - PBIO_NUM_DEFAULT_LOG_VALUES];
            xSemaphoreGive(_xMutex);
            return col_name;
        }
        xSemaphoreGive(_xMutex);
    }

    return nullptr;
}

const char* PBIOLogger::col_unit(uint8_t col) {
    if (xSemaphoreTake(_xMutex, portMAX_DELAY)) {
        if (col >= 0 && col < PBIO_NUM_DEFAULT_LOG_VALUES) {
            // Default columns
            switch (col) {
                case 0:
                    xSemaphoreGive(_xMutex);
                    return "ms";
                default:
                    xSemaphoreGive(_xMutex);
                    return nullptr;
            }
        }
        if ((col - PBIO_NUM_DEFAULT_LOG_VALUES) < _num_values) {
            char* col_unit = _col_units[col - PBIO_NUM_DEFAULT_LOG_VALUES];
            xSemaphoreGive(_xMutex);
            return col_unit;
        }
        xSemaphoreGive(_xMutex);
    }

    return nullptr;
}

bool PBIOLogger::is_active() {
    bool active = false;
    if (xSemaphoreTake(_xMutex, portMAX_DELAY)) {
        active = _active;
        xSemaphoreGive(_xMutex);
    }
    return active;
}