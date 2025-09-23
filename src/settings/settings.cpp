#include "settings/settings.hpp"

SettingsGroup* Settings::getGroup(const char* name) {
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
        case SettingType::Float:
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
        case SettingType::UInt16:
        {
            Setting<uint16_t>* usetting = (Setting<uint16_t>*)setting;
            preferences.putUShort(setting->getName(), usetting->getValue());
            break;
        }        
        case SettingType::Bool:
        {
            Setting<bool>* bsetting = (Setting<bool>*)setting;
            preferences.putBool(setting->getName(), bsetting->getValue());
            break;
        }
        
        default:
            String errorMsg = "Failed to save a setting of type " + String((int)setting->getType()) + " to NVS storage. Unknown type";
            Logger::instance().logW(errorMsg.c_str());
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
            String errorMsg = "Failed to open NVS to store " + String(group->getName()) + " group!!";
            Logger::instance().logW(errorMsg.c_str());
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
        case SettingType::Float:
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
        case SettingType::UInt16:
        {
            Setting<uint16_t>* usetting = (Setting<uint16_t>*)setting;
            usetting->setValue(preferences.getUShort(setting->getName(), usetting->getValue()));            
            break;
        }
        case SettingType::Bool:
        {
            Setting<bool>* bsetting = (Setting<bool>*)setting;
            bsetting->setValue(preferences.getBool(setting->getName(), bsetting->getValue()));            
            break;
        }
                
        default:
            String errorMsg = "Failed to read a setting of type " + String((int)setting->getType()) + " to NVS storage. Unknown type";
            Logger::instance().logW(errorMsg.c_str());
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
            String errorMsg = "Failed to open NVS to read " + String(group->getName()) + " group!!";
            Logger::instance().logW(errorMsg.c_str());
            continue;
        }

        readGroupFromNVS(preferences, *group);

        // Close the preference
        preferences.end();
    }
}

