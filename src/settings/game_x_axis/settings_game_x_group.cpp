#include "settings/game_x_axis/settings_game_x_group.hpp"

SettingsGameAxisXGroup::SettingsGameAxisXGroup(GameSettings& gameSetttings) : _gameSetttings(gameSetttings) 
{ 
}

//virtual const char* getName() const;
const char* SettingsGameAxisXGroup::getName() const {
    return "game_x_axis";
}

const char* SettingsGameAxisXGroup::getTitle() const {
    return "Game X Axis";
}

ISetting** SettingsGameAxisXGroup::getSettings() {
    return _settings;
}
