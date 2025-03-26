#ifndef __ENUMS_HPP_
#define __ENUMS_HPP_

typedef enum {
    PBIO_DIRECTION_CLOCKWISE,         /**< Positive means clockwise */
    PBIO_DIRECTION_COUNTERCLOCKWISE,  /**< Positive means counterclockwise */
} pbio_direction_t;

typedef enum {
    PBIO_ACTUATION_COAST,      /**< Coast the motor */
    PBIO_ACTUATION_BRAKE,      /**< Brake the motor */
    PBIO_ACTUATION_HOLD,       /**< Actively hold the motor in place */
    PBIO_ACTUATION_DUTY,
} pbio_actuation_t;

#endif