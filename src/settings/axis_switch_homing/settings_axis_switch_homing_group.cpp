#include "settings/axis_switch_homing/settings_axis_switch_homing_group.hpp"

SettingsAxisSwitchHomingGroup::SettingsAxisSwitchHomingGroup(const char* name, const char* description,  MotorWithReferenceSwitch& motor) : _motor(motor) 
{ 
    _name = name;
    _description = description;
}

const char* SettingsAxisSwitchHomingGroup::getName() const {
    return _name;
}

const char* SettingsAxisSwitchHomingGroup::getTitle() const {
    return _description;
}

ISetting** SettingsAxisSwitchHomingGroup::getSettings() {
    return _settings;
}
