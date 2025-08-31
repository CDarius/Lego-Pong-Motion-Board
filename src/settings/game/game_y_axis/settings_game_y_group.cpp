#include "settings/game/game_y_axis/settings_game_y_group.hpp"

const char* SettingsGameAxisYGroup::getName() const {
    return "game_y_axis";
}

const char* SettingsGameAxisYGroup::getTitle() const {
    return "Game Y Axis";
}

ISetting** SettingsGameAxisYGroup::getSettings() {
    return _settings;
}
