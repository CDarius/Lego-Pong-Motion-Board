#include "settings/setting.hpp"
#include "settings/settings_group.hpp"
#include "setting_axisswitchhoming_speed.hpp"
#include "setting_axisswitchhoming_minimumtravel.hpp"
#include "setting_axisswitchhoming_homeswitchpos.hpp"
#include "setting_axisswitchhoming_posafterhome.hpp"
#include "motor_control\motorwithreferenceswitch.hpp"

class SettingsAxisSwitchHomingGroup : public SettingsGroup {
    private:
        const char* _name;
        const char* _description;

        MotorWithReferenceSwitch& _motor;
        AxisSwitchHomingSpeedSetting _speed = AxisSwitchHomingSpeedSetting(*_motor.config());
        AxisSwitchHomingMinimumTravelSetting _minimumTravel = AxisSwitchHomingMinimumTravelSetting(*_motor.config());
        AxisSwitchHomingHomeSwitchPosSetting _homeSwitchPos = AxisSwitchHomingHomeSwitchPosSetting(*_motor.config());
        AxisSwitchHomingPosAfterHomeSetting _posAfterHome = AxisSwitchHomingPosAfterHomeSetting(*_motor.config());

        ISetting* _settings[4] = {&_speed, &_minimumTravel, &_homeSwitchPos, &_posAfterHome};

    public:
        SettingsAxisSwitchHomingGroup(const char* name, const char* description, MotorWithReferenceSwitch& motor);

        const char* getName() const;
        const char* getTitle() const;

        ISetting** getSettings();

        uint16_t getSettingsCount() const {
            return sizeof(_settings) / sizeof(ISetting*);
        }
};