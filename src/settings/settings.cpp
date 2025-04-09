#include "settings/settings.hpp"

SettingsGroup* Settings::getSetting(const char* name) {
    for(uint16_t i=0; i < _groupsCount; i++) {
        SettingsGroup* group = _groups[i];
        if (strcmp(name, group->getName()) == 0)
        {
            return group;
        }
    }

    return nullptr;
}

SettingsGroup** Settings::getGroups() {
    return _groups;
}

uint16_t Settings::getGroupsCount() const {
    return _groupsCount;
}

void storeGroupInNVS(Preferences& preferences, SettingsGroup& group) {
    ISetting** settings = group.getSettings();
    uint16_t settingsCount = group.getSettingsCount();

    for(uint16_t i=0; i < settingsCount; i++) {
        ISetting* setting = settings[i];

        switch (setting->getType())
        {
        case SettingType::FloatType:
        {
            Setting<float>* fsetting = (Setting<float>*)setting;
            preferences.putFloat(setting->getName(), fsetting->getValue());
            break;
        }
        case SettingType::UInt8:
        {
            Setting<uint8_t>* usetting = (Setting<uint8_t>*)setting;
            preferences.putUChar(setting->getName(), usetting->getValue());
            break;
        }
        
        default:
            Serial.print("Failed to save a setting of type ");
            Serial.print((int)setting->getType());
            Serial.println(" to NVS storage. Unknown type");
            break;
        }
    }
}

void Settings::storeInNVS() {
    Preferences preferences;

    // Store each group in a different preference
    for(uint16_t i=0; i < _groupsCount; i++) {
        SettingsGroup* group = _groups[i];

        // Open a writable preference for the group
        if (!preferences.begin(group->getName(), false)) {
            Serial.print("Failed to open NVS to store ");
            Serial.print(group->getName());
            Serial.println(" group!!");
            continue;;
        }

        storeGroupInNVS(preferences, *group);

        // Close the preference
        preferences.end();
    }
}

void readGroupFromNVS(Preferences& preferences, SettingsGroup& group) {
    ISetting** settings = group.getSettings();
    uint16_t settingsCount = group.getSettingsCount();

    for(uint16_t i=0; i < settingsCount; i++) {
        ISetting* setting = settings[i];

        switch (setting->getType())
        {
        case SettingType::FloatType:
        {
            Setting<float>* fsetting = (Setting<float>*)setting;
            fsetting->setValue(preferences.getFloat(setting->getName(), fsetting->getValue()));            
            break;
        }
        case SettingType::UInt8:
        {
            Setting<uint8_t>* usetting = (Setting<uint8_t>*)setting;
            usetting->setValue(preferences.getUChar(setting->getName(), usetting->getValue()));            
            break;
        }
        
        default:
            Serial.print("Failed to read a setting of type ");
            Serial.print((int)setting->getType());
            Serial.println(" to NVS storage. Unknown type");
            break;
        }
    }
}

void Settings::restoreFromNVS() {
    Preferences preferences;

    // Store each group in a different preference
    for(uint16_t i=0; i < _groupsCount; i++) {
        SettingsGroup* group = _groups[i];

        // Open a readonly preference for the group
        if (!preferences.begin(group->getName(), true)) {
            Serial.print("Failed to open NVS to read ");
            Serial.print(group->getName());
            Serial.println(" group!!");
            continue;;
        }

        readGroupFromNVS(preferences, *group);

        // Close the preference
        preferences.end();
    }
}

