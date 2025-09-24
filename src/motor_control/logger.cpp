// SPDX-License-Identifier: MIT
// Copyright (c) 2018-2020 The Pybricks Authors

#include "motor_control/logger.hpp"
#include "config.h"
#include "motor_control/const.h"
#include "monotonic.h"
#include "esp_heap_caps.h"

static void pbio_logger_delete(pbio_log_t *log) {
    // Free log if any
    if (log->len > 0) {
        free(log->data);
        log->data = nullptr;
    }
    log->sampled = 0;
    log->skipped = 0;
    log->len = 0;
    log->active = false;
}

/*
 * Start the logger for a specific duration and sample division
 * log: Pointer to the logger to start
 * duration: Desired logging duration in ms
 * div: Number of calls to the logger per sample actually logged (1 = log every call, 2 = log every other call, etc.)
 * Returns ::PBIO_SUCCESS on success, or an error code if the logger could not be started
*/
pbio_error_t pbio_logger_start(pbio_log_t *log, uint32_t duration, uint32_t div) {
    // Free any existing log
    pbio_logger_delete(log);

    // Set number of calls to the logger per sample actually logged
    log->sample_div = div > 0 ? div : 1;

    // Minimal log length
    uint32_t len = duration / PBIO_CONFIG_SERVO_PERIOD_MS / log->sample_div;

    // Assert length is allowed
    if (len > MAX_LOG_LEN) {
        return PBIO_ERROR_INVALID_ARG;
    }

    // Allocate memory for the logs
    log->data = (int32_t*)heap_caps_malloc(len * NUM_LOGGER_COLS(log) * sizeof(int32_t), MALLOC_CAP_SPIRAM);
    if (log->data == nullptr) {
        return PBIO_ERROR_FAILED;
    }

    // (re-)initialize logger status for this servo
    log->len = len;
    log->sampled = 0;
    log->skipped = 0;
    log->start = monotonic_us();
    log->active = true;
    return PBIO_SUCCESS;
}

uint32_t pbio_logger_rows(pbio_log_t *log) {
    return log->sampled;
}

uint32_t pbio_logger_cols(pbio_log_t *log) {
    return NUM_LOGGER_COLS(log);
}

void pbio_logger_stop(pbio_log_t *log) {
    // Release the logger for re-use
    log->active = false;
}

pbio_error_t pbio_logger_update(pbio_log_t *log, int32_t *buf) {

    // Log nothing if logger is inactive
    if (!log->active) {
        return PBIO_SUCCESS;
    }

    // Skip logging if we are not yet at a multiple of sample_div
    if (++log->skipped != log->sample_div) {
        return PBIO_SUCCESS;
    }
    log->skipped = 0;

    // Raise error if log is full, which should not happen
    if (log->sampled > log->len) {
        log->active = false;
        return PBIO_ERROR_FAILED;
    }

    // Stop successfully when done
    if (log->sampled == log->len) {
        log->active = false;
        return PBIO_SUCCESS;
    }

    // Write time of logging (ms)
    uint32_t start_index = log->sampled * NUM_LOGGER_COLS(log);
    log->data[start_index] = (int32_t)((monotonic_us() - log->start)/US_PER_MS);

    // Write the data
    for (uint8_t i = 0; i < log->num_values; i++) {
        log->data[start_index + i + NUM_DEFAULT_LOG_VALUES] = buf[i];
    }

    // Increment sample counter
    log->sampled++;

    return PBIO_SUCCESS;
}

pbio_error_t pbio_logger_read(pbio_log_t *log, int32_t sample_index, int32_t *buf) {

    // Validate index value
    if (sample_index < -1) {
        return PBIO_ERROR_INVALID_ARG;
    }

    // Get index or latest sample if requested index is -1
    uint32_t index = sample_index == -1 ? log->sampled - 1 : sample_index;

    // Ensure index is within bounds
    if (index >= log->sampled) {
        return PBIO_ERROR_INVALID_ARG;
    }

    // Read the data
    uint32_t start_index = index * NUM_LOGGER_COLS(log);
    for (uint8_t i = 0; i < NUM_LOGGER_COLS(log); i++) {
        buf[i] = log->data[start_index + i];
    }

    return PBIO_SUCCESS;
}

pbio_error_t pbio_logger_read(pbio_log_t *log, uint32_t start_sample, uint32_t num_samples, int32_t *buf) {
    if (start_sample + num_samples > log->sampled) {
        num_samples = log->sampled - start_sample;
    }

    // Ensure index is within bounds
    if (start_sample >= log->sampled) {
        return PBIO_ERROR_INVALID_ARG;
    }

    // Read the data
    uint32_t start_array_index = start_sample * NUM_LOGGER_COLS(log);
    uint32_t num_array_indexes = num_samples * NUM_LOGGER_COLS(log);
    for (uint32_t i = 0; i < num_array_indexes; i++) {
        buf[i] = log->data[start_array_index + i];
    }

    return PBIO_SUCCESS;
}

const char* pbio_logger_col_name(pbio_log_t *log, uint8_t col) {
    if (col >= 0 && col < NUM_DEFAULT_LOG_VALUES) {
        // Default columns
        switch (col) {
            case 0:
                return "Time";
            default:
                return nullptr;
        }
    }
    if ((col - NUM_DEFAULT_LOG_VALUES) < log->num_values) {
        return log->col_names[col - NUM_DEFAULT_LOG_VALUES];
    }
    return nullptr;
}

const char* pbio_logger_col_unit(pbio_log_t *log, uint8_t col) {
    if (col >= 0 && col < NUM_DEFAULT_LOG_VALUES) {
        // Default columns
        switch (col) {
            case 0:
                return "ms";
            default:
                return nullptr;
        }
    }
    if ((col - NUM_DEFAULT_LOG_VALUES) < log->num_values) {
        return log->col_units[col - NUM_DEFAULT_LOG_VALUES];
    }
    return nullptr;
}