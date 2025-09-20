#include "settings/setting.hpp"
#include "settings/settings_group.hpp"
#include "setting_axis_swlimitm.hpp"
#include "setting_axis_swlimitp.hpp"
#include "setting_axis_maxspeed.hpp"
#include "setting_axis_postolerance.hpp"
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

        ISetting* _settings[4] = {&_swLimitM, &_swLimitP, &_maxSpeed, &_posTolerance};

    public:
        SettingsAxisGroup(const char* name, const char* title, Motor& motor);

        const char* getName() const;
        const char* getTitle() const;

        ISetting** getSettings();

        uint16_t getSettingsCount() const {
            return sizeof(_settings) / sizeof(ISetting*);
        }
};