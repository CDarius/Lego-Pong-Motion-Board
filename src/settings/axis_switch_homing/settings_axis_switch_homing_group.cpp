#include "settings/axis_switch_homing/settings_axis_switch_homing_group.hpp"

SettingsAxisSwitchHomingGroup::SettingsAxisSwitchHomingGroup(const char* name, const char* description, switch_homing_config_t& config) : _config(config) 
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
