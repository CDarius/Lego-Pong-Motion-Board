// SPDX-License-Identifier: MIT
// Copyright (c) 2018-2020 The Pybricks Authors
// Copyright (c) 2025 CDarius

#pragma once

#include <Arduino.h>
#include "const.h"
#include "error.hpp"

// Number of values logged by the logger itself, such as time of call to logger
#define PBIO_NUM_DEFAULT_LOG_VALUES (1)

// Maximum number of values to be logged per sample
#define PBIO_MAX_LOG_VALUES (20)

// Maximum length (index) of a log
#define PBIO_MAX_LOG_LEN ((MAX_LOG_MEM_KB*1024) / PBIO_MAX_LOG_VALUES)

#define PBIO_NUM_LOGGER_COLS (_num_values + PBIO_NUM_DEFAULT_LOG_VALUES)

class PBIOLogger {
    private:
        bool _active = false;
        uint32_t _skipped = 0;
        uint32_t _sampled = 0;
        uint32_t _len = 0;
        uint64_t _start = 0;
        uint8_t _num_values = SERVO_LOG_NUM_VALUES;
        int32_t *_data = nullptr;
        uint32_t _sample_div = 1;
        char **_col_names;
        char **_col_units;
        SemaphoreHandle_t _xMutex = xSemaphoreCreateMutex();

        void delete_log();
    public:
        PBIOLogger();

        pbio_error_t start(uint32_t duration, uint32_t div);
        pbio_error_t update(int32_t *buf);
        void stop();
        bool is_active();
        
        pbio_error_t read(int32_t sample_index, int32_t *buf);
        pbio_error_t read(uint32_t start_sample, uint32_t num_samples, int32_t *buf);

        uint32_t rows();
        uint32_t cols();
        const char* col_name(uint8_t col);
        const char* col_unit(uint8_t col);
};