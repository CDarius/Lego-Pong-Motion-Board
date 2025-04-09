#include "settings/settings_group.hpp"

ISetting* SettingsGroup::getSetting(const char* name) {
    ISetting** settings = getSettings();
    uint16_t count = getSettingsCount();

    for(uint16_t i = 0; i < count; i++) {
        ISetting* setting = settings[i];
        if (strcmp(name, setting->getName()) == 0) {
            return setting;
        }
    }

    return nullptr;
}