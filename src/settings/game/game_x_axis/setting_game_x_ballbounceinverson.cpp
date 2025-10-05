#include "game\game_settings.hpp"
#include "settings\game\game_x_axis\setting_game_x_ballbounceinverson.hpp"

const char* GameXBallBounceInversionOvershootSetting::getName() const {
    float speeds[X_AXIS_BOUNCE_INVERSIONS_COUNT] = X_AXIS_BOUNCE_INVERSIONS_SPEEDS;
    switch ((int)speeds[_index]) {
    case 10:
        return "bounce_inv_10";
    case 15:
        return "bounce_inv_15";
    case 20:
        return "bounce_inv_20";
    case 25:
        return "bounce_inv_25";
    case 30:
        return "bounce_inv_30";
    case 35:
        return "bounce_inv_35";
    default:
        return "bounce_inv_unknown";
    }
}

const char* GameXBallBounceInversionOvershootSetting::getTitle() const {
    float speeds[X_AXIS_BOUNCE_INVERSIONS_COUNT] = X_AXIS_BOUNCE_INVERSIONS_SPEEDS;
    switch ((int)speeds[_index])
    {
    case 10:
        return "Bounce overshoot 10 stud/s";
    case 15:
        return "Bounce overshoot 15 stud/s";
    case 20:
        return "Bounce overshoot 20 stud/s";
    case 25:
        return "Bounce overshoot 25 stud/s";
    case 30:
        return "Bounce overshoot 30 stud/s";
    case 35:
        return "Bounce overshoot 35 stud/s";
    default:
        return "Bounce overshoot unknown";
    }
}

const char* GameXBallBounceInversionOvershootSetting::getDescription() const {
    float speeds[X_AXIS_BOUNCE_INVERSIONS_COUNT] = X_AXIS_BOUNCE_INVERSIONS_SPEEDS;
    switch((int)speeds[_index]) 
    {
        case 10:
            return "Ball X-Axis overshoot distance when a bounce inversion occurs at 10 stud/s";
        case 15:
            return "Ball X-Axis overshoot distance when a bounce inversion occurs at 15 stud/s";
        case 20:
            return "Ball X-Axis overshoot distance when a bounce inversion occurs at 20 stud/s";
        case 25:
            return "Ball X-Axis overshoot distance when a bounce inversion occurs at 25 stud/s";
        case 30:
            return "Ball X-Axis overshoot distance when a bounce inversion occurs at 30 stud/s";
        case 35:
            return "Ball X-Axis overshoot distance when a bounce inversion occurs at 35 stud/s";
        default:
            return "Ball X-Axis overshoot distance when a bounce inversion occurs at unknown speed";
    }
}

