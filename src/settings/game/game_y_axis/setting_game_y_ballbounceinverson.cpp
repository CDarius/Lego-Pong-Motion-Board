#include "game\game_settings.hpp"
#include "settings\game\game_y_axis\setting_game_y_ballbounceinverson.hpp"

const char* GameYBallBounceInversionOvershootSetting::getName() const {
    float speeds[Y_AXIS_BOUNCE_INVERSIONS_COUNT] = Y_AXIS_BOUNCE_INVERSIONS_SPEEDS;
    switch ((int)speeds[_index]) {
    case 5:
        return "bounce_inv_5";
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
    case 40:
        return "bounce_inv_40";
    case 45:
        return "bounce_inv_45";
    default:
        return "bounce_inv_unknown";
    }
}

const char* GameYBallBounceInversionOvershootSetting::getTitle() const {
    float speeds[Y_AXIS_BOUNCE_INVERSIONS_COUNT] = Y_AXIS_BOUNCE_INVERSIONS_SPEEDS;
    switch ((int)speeds[_index])
    {
    case 5:
        return "Bounce overshoot 5 stud/s";
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
    case 40:
        return "Bounce overshoot 40 stud/s";
    case 45:
        return "Bounce overshoot 45 stud/s";
    default:
        return "Bounce overshoot unknown";
    }
}

const char* GameYBallBounceInversionOvershootSetting::getDescription() const {
    float speeds[Y_AXIS_BOUNCE_INVERSIONS_COUNT] = Y_AXIS_BOUNCE_INVERSIONS_SPEEDS;
    switch((int)speeds[_index]) 
    {
        case 5:
            return "Ball Y-Axis overshoot distance when a bounce inversion occurs at 5 stud/s";
        case 10:
            return "Ball Y-Axis overshoot distance when a bounce inversion occurs at 10 stud/s";
        case 15:
            return "Ball Y-Axis overshoot distance when a bounce inversion occurs at 15 stud/s";
        case 20:
            return "Ball Y-Axis overshoot distance when a bounce inversion occurs at 20 stud/s";
        case 25:
            return "Ball Y-Axis overshoot distance when a bounce inversion occurs at 25 stud/s";
        case 30:
            return "Ball Y-Axis overshoot distance when a bounce inversion occurs at 30 stud/s";
        case 35:
            return "Ball Y-Axis overshoot distance when a bounce inversion occurs at 35 stud/s";
        case 40:
            return "Ball Y-Axis overshoot distance when a bounce inversion occurs at 40 stud/s";
        case 45:
            return "Ball Y-Axis overshoot distance when a bounce inversion occurs at 45 stud/s";
        default:
            return "Ball Y-Axis overshoot distance when a bounce inversion occurs at unknown speed";
    }
}

