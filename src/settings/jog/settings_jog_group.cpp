#include "settings/jog/settings_jog_group.hpp"

const char* SettingsJogGroup::getName() const {
    return "jog";
}

const char* SettingsJogGroup::getTitle() const {
    return "Jog Settings";
}

ISetting** SettingsJogGroup::getSettings() {
    return _settings;
}
