#pragma once

#include <Arduino.h>
#include <FunctionalInterrupt.h>
#include "monotonic.h"
#include "enums.h"
#include "const.h"

#define TACHO_RING_BUF_SIZE 64
#define TACHO_RING_BUF_MASK TACHO_RING_BUF_SIZE - 1

class Tacho {
    public:
        Tacho();
        void begin(uint8_t pin1, uint8_t pin2, float gear_ration, pbio_direction_t direction);

        int32_t getCount() const;
        float getAngle() const;
        void resetAngle(float angle);

        int32_t getRate() const;
        float getAngularRate() const;

        bool isSequenceError();
        void clearSequenceError();

    private:
        uint8_t _pin1;
        uint8_t _pin2;
        float _gear_ratio;
        pbio_direction_t _direction;
        int32_t _offset = 0;
        int32_t _last_count = 0;
        uint8_t _status;
        bool _sequence_error = false;
        mutable portMUX_TYPE mux = portMUX_INITIALIZER_UNLOCKED;

        int32_t _ring_counts[TACHO_RING_BUF_SIZE];
        uint64_t _ring_timestamps[TACHO_RING_BUF_SIZE];
        volatile uint8_t _ring_head = 0;
    
        void ISR_read_encoder();
};
