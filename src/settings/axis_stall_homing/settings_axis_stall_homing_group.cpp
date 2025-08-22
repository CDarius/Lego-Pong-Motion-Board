#include "settings/axis_stall_homing/settings_axis_stall_homing_group.hpp"

SettingsAxisStallHomingGroup::SettingsAxisStallHomingGroup(const char* name, const char* description,  MotorWithStallReference& motor) : _motor(motor) 
{ 
    _name = name;
    _description = description;
}

const char* SettingsAxisStallHomingGroup::getName() const {
    return _name;
}

const char* SettingsAxisStallHomingGroup::getTitle() const {
    return _description;
}

ISetting** SettingsAxisStallHomingGroup::getSettings() {
    return _settings;
}
