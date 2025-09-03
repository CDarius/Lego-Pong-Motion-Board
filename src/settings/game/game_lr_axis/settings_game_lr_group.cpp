#include "settings/game/game_lr_axis/settings_game_lr_group.hpp"

const char* SettingsGameAxisLRGroup::getName() const {
    return "game_lr_axis";
}

const char* SettingsGameAxisLRGroup::getTitle() const {
    return "Game L/R Axis";
}

ISetting** SettingsGameAxisLRGroup::getSettings() {
    return _settings;
}
