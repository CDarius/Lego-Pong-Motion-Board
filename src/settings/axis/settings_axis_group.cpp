#include "settings/axis/settings_axis_group.hpp"

SettingsAxisGroup::SettingsAxisGroup(const char* name, const char* description, Motor& motor) : _motor(motor) 
{ 
    _name = name;
    _description = description;
}

//virtual const char* getName() const;
const char* SettingsAxisGroup::getName() const {
    return _name;
}

const char* SettingsAxisGroup::getTitle() const {
    return _description;
}

ISetting** SettingsAxisGroup::getSettings() {
    return _settings;
}
