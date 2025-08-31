#include "game\game_settings.hpp"
#include "settings\game\game_x_axis\setting_game_x_ballbounceinverson.hpp"

const char* GameXBallBounceInversionOvershootSetting::getName() const {
    float speeds[X_AXIS_BOUNCE_INVERSIONS_COUNT] = X_AXIS_BOUNCE_INVERSIONS_SPEEDS;
    switch ((int)speeds[_index]) {
    case 50:
        return "bounce_inv_50";
    case 60:
        return "bounce_inv_60";
    case 70:
        return "bounce_inv_70";
    case 80:
        return "bounce_inv_80";
    case 90:
        return "bounce_inv_90";
    case 100:
        return "bounce_inv_100";
    default:
        return "bounce_inv_unknown";
    }
}

const char* GameXBallBounceInversionOvershootSetting::getTitle() const {
    float speeds[X_AXIS_BOUNCE_INVERSIONS_COUNT] = X_AXIS_BOUNCE_INVERSIONS_SPEEDS;
    switch ((int)speeds[_index])
    {
    case 50:
        return "Bounce overshoot 50%";
    case 60:
        return "Bounce overshoot 60%";
    case 70:
        return "Bounce overshoot 70%";
    case 80:
        return "Bounce overshoot 80%";
    case 90:
        return "Bounce overshoot 90%";
    case 100:
        return "Bounce overshoot 100%";
    default:
        return "Bounce overshoot unknown";
    }
}

const char* GameXBallBounceInversionOvershootSetting::getDescription() const {
    float speeds[X_AXIS_BOUNCE_INVERSIONS_COUNT] = X_AXIS_BOUNCE_INVERSIONS_SPEEDS;
    switch((int)speeds[_index]) 
    {
        case 50:
            return "Ball X-Axis overshoot distance when a bounce inversion occurs at 50% speed";
        case 60:
            return "Ball X-Axis overshoot distance when a bounce inversion occurs at 60% speed";
        case 70:
            return "Ball X-Axis overshoot distance when a bounce inversion occurs at 70% speed";
        case 80:
            return "Ball X-Axis overshoot distance when a bounce inversion occurs at 80% speed";
        case 90:
            return "Ball X-Axis overshoot distance when a bounce inversion occurs at 90% speed";
        case 100:
            return "Ball X-Axis overshoot distance when a bounce inversion occurs at 100% speed";
        default:
            return "Ball X-Axis overshoot distance when a bounce inversion occurs at unknown speed";
    }
}

