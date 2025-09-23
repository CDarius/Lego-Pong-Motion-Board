#include "settings/setting.hpp"
#include "settings/settings_group.hpp"
#include "setting_axis_swlimitm.hpp"
#include "setting_axis_swlimitp.hpp"
#include "setting_axis_maxspeed.hpp"
#include "setting_axis_postolerance.hpp"
#include "settings/axis/setting_axis_pidkp.hpp"
#include "settings/axis/setting_axis_pidki.hpp"
#include "settings/axis/setting_axis_pidkd.hpp"
#include "settings/axis/setting_axis_integralrange.hpp"
#include "settings/axis/setting_axis_integralrate.hpp"
#include "motor_control\motor.hpp"

class SettingsAxisGroup : public SettingsGroup {
    private:
        const char* _name;
        const char* _title;

        Motor& _motor;
        AxisSwLimitMSetting _swLimitM = AxisSwLimitMSetting(_motor);
        AxisSwLimitPSetting _swLimitP = AxisSwLimitPSetting(_motor);
        AxisMaxSpeedSetting _maxSpeed = AxisMaxSpeedSetting(_motor);
        AxisPosToleranceSetting _posTolerance = AxisPosToleranceSetting(_motor);
        AxisPidKpSetting _pidKp = AxisPidKpSetting(_motor);
        AxisPidKiSetting _pidKi = AxisPidKiSetting(_motor);
        AxisPidKdSetting _pidKd = AxisPidKdSetting(_motor);
        AxisIntegralRangeSetting _integralRange = AxisIntegralRangeSetting(_motor);
        AxisIntegralRateSetting _integralRate = AxisIntegralRateSetting(_motor);

        ISetting* _settings[9] = {
            &_swLimitM, &_swLimitP, 
            &_maxSpeed, &_posTolerance, 
            &_pidKp, &_pidKi, &_pidKd,
            &_integralRange, &_integralRate
        };

    public:
        SettingsAxisGroup(const char* name, const char* title, Motor& motor);

        const char* getName() const;
        const char* getTitle() const;

        ISetting** getSettings();

        uint16_t getSettingsCount() const {
            return sizeof(_settings) / sizeof(ISetting*);
        }
};