#include "monotonic.h"

unsigned long __last_micros_value = 0;
uint64_t __accumulated_micros_value = 0;

uint64_t monotonic_us() {
    unsigned long new_value = micros();
    uint32_t delta;
    if (new_value > __last_micros_value)
        delta = (uint32_t)(new_value - __last_micros_value);
    else
        delta = (uint32_t)(((unsigned long)(0xFFFFFFFF) - __last_micros_value) + new_value);

    __accumulated_micros_value += delta;
    __last_micros_value = new_value;

    return __accumulated_micros_value;
}