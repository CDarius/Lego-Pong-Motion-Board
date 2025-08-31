#include "motor_control/tacho.hpp"

Tacho::Tacho() {
}

void Tacho::begin(uint8_t pin1, uint8_t pin2, float gear_ratio, pbio_direction_t direction) {
    _pin1 = pin1;
    _pin2 = pin2;
    _gear_ratio = gear_ratio;
    _direction = direction;

    pinMode(_pin1, INPUT_PULLUP);
    pinMode(_pin2, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(_pin1), std::bind( &Tacho::ISR_read_encoder, this ), CHANGE);
    attachInterrupt(digitalPinToInterrupt(_pin2), std::bind( &Tacho::ISR_read_encoder, this ), CHANGE);

    _status = digitalRead(_pin1) << 1 | digitalRead(_pin2);
}

void IRAM_ATTR Tacho::ISR_read_encoder() {
    uint8_t new_status = digitalRead(_pin1) << 1 | digitalRead(_pin2);

    if (_status == new_status)
        return;

    portENTER_CRITICAL( &mux );

    bool ok = false;
    switch (_status)
    {
    case 1:
        if (new_status == 3) {
            _last_count++;
            ok = true;
        }
        if (new_status == 0) {
            _last_count--;
            ok = true;
        }
        break;
    
    case 3:
        if (new_status == 2) {
            _last_count++;
            ok = true;
        }
        if (new_status == 1) {
            _last_count--;
            ok = true;
        }
        break;

    case 2:
        if (new_status == 0) {
            _last_count++;
            ok = true;
        }
        if (new_status == 3) {
            _last_count--;
            ok = true;
        }
        break;

    case 0:
        if (new_status == 1) {
            _last_count++;
            ok = true;
        }
        if (new_status == 2) {
            _last_count--;
            ok = true;
        }
        break;
    }

    _status = new_status;
    if (ok) {
        uint8_t new_head = (_ring_head + 1) & TACHO_RING_BUF_MASK;

        _ring_counts[new_head] = _last_count;
        _ring_timestamps[new_head] = monotonic_us();
        _ring_head = new_head;
    }
    else
        _sequence_error = false;

    portEXIT_CRITICAL( &mux );
}

int32_t Tacho::getCount() const {
    portENTER_CRITICAL( &mux );
    int32_t value = _last_count + _offset;
    portEXIT_CRITICAL( &mux );    
    return _direction == PBIO_DIRECTION_CLOCKWISE ? value : -value;
}

float Tacho::getAngle() const {
    return getCount() / _gear_ratio;
}

int32_t Tacho::getRate() const {
    portENTER_CRITICAL( &mux );

    // head can be updated in interrupt, so only read it once
    uint8_t head = _ring_head;
    int32_t head_count = _ring_counts[head];
    uint64_t head_time = _ring_timestamps[head];
    
    uint64_t now = monotonic_us();

    // if it has been more than 50ms since last timestamp, we are not moving.
    if ((now - head_time) > 50 * US_PER_MS) {
        portEXIT_CRITICAL( &mux );
        return 0;
    }

    uint8_t x = 0;
    uint8_t tail;

    int32_t tail_count;
    uint64_t tail_time;
    while (x++ < TACHO_RING_BUF_SIZE) {
        tail = (head - x) & TACHO_RING_BUF_MASK;

        tail_count = _ring_counts[tail];
        tail_time = _ring_timestamps[tail];

        // if count hasn't changed, then we are not moving
        if (head_count == tail_count) {
            portEXIT_CRITICAL( &mux );
            return 0;
        }

        /*
        * we need delta_t to be >= 20ms to be reasonably accurate.
        * timer is 10us, thus * 100 to get ms.
        */
        if ((head_time - tail_time) >= 20 * US_PER_MS) {
            break;
        }

        // If the ring buffer entry is newer than head it means that there are no older
        // record in the buffer to search for
        if (tail_time > head_time)
        {
            portEXIT_CRITICAL( &mux );
            return 0;
        }
    }

    portEXIT_CRITICAL( &mux );

    /* avoid divide by 0 - motor probably hasn't moved yet */
    if (head_time == tail_time)
        return 0;

    int32_t rate = (head_count - tail_count) * US_PER_SECOND / (int32_t)(head_time - tail_time);
    return _direction == PBIO_DIRECTION_CLOCKWISE ? rate : -rate;
}

float Tacho::getAngularRate() const {
    return getRate() / _gear_ratio;
}

/**
 * Reset the angle of the tacho
 *
 * @param angle: The angle to reset to (in user units)
 */
void Tacho::resetAngle(float angle) {
    if (_direction == PBIO_DIRECTION_COUNTERCLOCKWISE)
        angle =-angle;

    int32_t counts = (int32_t)(_gear_ratio * angle);
    _offset = counts - _last_count;
}

bool Tacho::isSequenceError() {
    portENTER_CRITICAL( &mux );
    bool value = _sequence_error;
    portEXIT_CRITICAL( &mux );
    return value;
}

void Tacho::clearSequenceError() {
    portENTER_CRITICAL( &mux );
    _sequence_error = false;
    portEXIT_CRITICAL( &mux );
}

