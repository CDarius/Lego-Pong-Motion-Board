#pragma once

#include "settings/setting.hpp"
#include "settings/settings_group.hpp"
#include "game/game_settings.hpp"
#include "setting_game_y_ballbounceinverson.hpp"
#include "settings/game/setting_axis_minspeed.hpp"

class SettingsGameAxisYGroup : public SettingsGroup {
    private:
        GameYAxisSettings& _gameSettings;

        GameMinSpeedSetting _gameMinSpeedY = GameMinSpeedSetting(_gameSettings.minCloseLoopSpeed);
        GameYBallBounceInversionOvershootSetting _gameBallBounceInvOvershoot60 = GameYBallBounceInversionOvershootSetting(_gameSettings, 0);
        GameYBallBounceInversionOvershootSetting _gameBallBounceInvOvershoot70 = GameYBallBounceInversionOvershootSetting(_gameSettings, 1);
        GameYBallBounceInversionOvershootSetting _gameBallBounceInvOvershoot80 = GameYBallBounceInversionOvershootSetting(_gameSettings, 2);
        GameYBallBounceInversionOvershootSetting _gameBallBounceInvOvershoot90 = GameYBallBounceInversionOvershootSetting(_gameSettings, 3);
        GameYBallBounceInversionOvershootSetting _gameBallBounceInvOvershoot100 = GameYBallBounceInversionOvershootSetting(_gameSettings, 4);

        ISetting* _settings[6] = {
            &_gameMinSpeedY,
            &_gameBallBounceInvOvershoot60, &_gameBallBounceInvOvershoot70, 
            &_gameBallBounceInvOvershoot80, &_gameBallBounceInvOvershoot90, 
            &_gameBallBounceInvOvershoot100
        };

    public:
        SettingsGameAxisYGroup(GameYAxisSettings& settings) : _gameSettings(settings) {}

        const char* getName() const;
        const char* getTitle() const;

        ISetting** getSettings();

        uint16_t getSettingsCount() const {
            return sizeof(_settings) / sizeof(ISetting*);
        }
};