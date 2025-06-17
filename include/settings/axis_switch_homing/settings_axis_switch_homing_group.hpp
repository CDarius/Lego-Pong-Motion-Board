#include "settings/setting.hpp"
#include "settings/settings_group.hpp"
#include "setting_axisswitchhoming_speed.hpp"
#include "setting_axisswitchhoming_minimumtravel.hpp"
#include "setting_axisswitchhoming_retractdistance.hpp"
#include "setting_axisswitchhoming_homeswitchpos.hpp"
#include "motor_control\motorhoming.hpp"

class SettingsAxisSwitchHomingGroup : public SettingsGroup {
    private:
        const char* _name;
        const char* _description;

        switch_homing_config_t& _config;
        AxisSwitchHomingSpeedSetting _speed = AxisSwitchHomingSpeedSetting(_config);
        AxisSwitchHomingMinimumTravelSetting _minimumTravel = AxisSwitchHomingMinimumTravelSetting(_config);
        AxisSwitchHomingRetractDistanceSetting _retractDistance = AxisSwitchHomingRetractDistanceSetting(_config);
        AxisSwitchHomingHomeSwitchPosSetting _homeSwitchPos = AxisSwitchHomingHomeSwitchPosSetting(_config);

        ISetting* _settings[4] = {&_speed, &_minimumTravel, &_retractDistance, &_homeSwitchPos};

    public:
        SettingsAxisSwitchHomingGroup(const char* name, const char* description, switch_homing_config_t& config);

        const char* getName() const;
        const char* getTitle() const;

        ISetting** getSettings();

        uint16_t getSettingsCount() const {
            return sizeof(_settings) / sizeof(ISetting*);
        }
};