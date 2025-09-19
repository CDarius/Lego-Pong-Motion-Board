#include "settings/ai_player/settings_aiplayer_group.hpp"

SettingsAIPlayerGroup::SettingsAIPlayerGroup(const char* name, const char* description, GameAIPlayerSettings& aiSettings) : _aiSettings(aiSettings) 
{ 
    _name = name;
    _title = description;
}

//virtual const char* getName() const;
const char* SettingsAIPlayerGroup::getName() const {
    return _name;
}

const char* SettingsAIPlayerGroup::getTitle() const {
    return _title;
}

ISetting** SettingsAIPlayerGroup::getSettings() {
    return _settings;
}
