#include "settings/setting.hpp"
#include "settings/settings_group.hpp"
#include "setting_axisstallhoming_speed.hpp"
#include "setting_axisstallhoming_dutylimit.hpp"
#include "setting_axisstallhoming_minimumtravel.hpp"
#include "setting_axisstallhoming_retractdistance.hpp"
#include "setting_axisstallhoming_homesobstaclepos.hpp"
#include "motor_control\motorwithstallreference.hpp"

class SettingsAxisStallHomingGroup : public SettingsGroup {
    private:
        const char* _name;
        const char* _description;

        MotorWithStallReference& _motor;
        AxisStallHomingSpeedSetting _speed = AxisStallHomingSpeedSetting(*_motor.config());
        AxisStallHomingDutyLimitSetting _dutyLimit = AxisStallHomingDutyLimitSetting(*_motor.config());
        AxisStallHomingMinimumTravelSetting _minimumTravel = AxisStallHomingMinimumTravelSetting(*_motor.config());
        AxisStallHomingRetractDistanceSetting _retractDistance = AxisStallHomingRetractDistanceSetting(*_motor.config());
        AxisStallHomingHomeObstaclePosSetting _homeObstaclePos = AxisStallHomingHomeObstaclePosSetting(_motor);

        ISetting* _settings[5] = {&_speed, &_dutyLimit, &_minimumTravel, &_retractDistance, &_homeObstaclePos};

    public:
        SettingsAxisStallHomingGroup(const char* name, const char* description, MotorWithStallReference& motor);

        const char* getName() const;
        const char* getTitle() const;

        ISetting** getSettings();

        uint16_t getSettingsCount() const {
            return sizeof(_settings) / sizeof(ISetting*);
        }
};