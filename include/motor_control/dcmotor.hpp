#ifndef __DCMOTOR_HPP_
#define __DCMOTOR_HPP_

#include <Arduino.h>
#include <driver/ledc.h>
#include "const.h"
#include "config.h"
#include "enums.h"

typedef enum {
    PBIO_DCMOTOR_COAST,               /**< dcmotor set to coast */
    PBIO_DCMOTOR_BRAKE,               /**< dcmotor set to brake */
    PBIO_DCMOTOR_DUTY_PASSIVE,        /**< dcmotor set to constant duty. */
} pbio_passivity_t;

class DCMotor {
    public:
        void begin(uint8_t pwmPin1, uint8_t pwmPin2, pbio_direction_t direction, uint16_t userPwmMax);
        void getState(pbio_passivity_t *state, int32_t *duty_now);
        uint16_t getUserPwmMax();
        void coast();
        void brake();
        void set_duty_cycle(int32_t duty_steps);
        
    private:
        uint8_t _pwmPin1;
        uint8_t _pwmPin2;
        uint16_t _userPwmMax;
        int32_t _duty_now;
        pbio_direction_t _direction;
        pbio_passivity_t _state;
};

#endif