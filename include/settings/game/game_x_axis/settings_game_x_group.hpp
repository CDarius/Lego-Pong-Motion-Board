#pragma once

#include "settings/setting.hpp"
#include "settings/settings_group.hpp"
#include "game/game_settings.hpp"
#include "setting_game_x_ballstartspeed.hpp"
#include "setting_game_x_ballspeedbounceinc.hpp"
#include "setting_game_x_ballbounceinverson.hpp"
#include "setting_game_x_ballpaddledist.hpp"
#include "settings/game/setting_axis_minspeed.hpp"

class SettingsGameAxisXGroup : public SettingsGroup {
    private:
        GameXAxisSettings& _gameSettings;

        GameXBallStartSpeedSetting _gameBallStartSpeedX = GameXBallStartSpeedSetting(_gameSettings);
        GameXBallSpeedBounceIncrementSetting _gameBallSpeedBounceIncX = GameXBallSpeedBounceIncrementSetting(_gameSettings);
        GameMinSpeedSetting _gameMinSpeedX = GameMinSpeedSetting(_gameSettings.minCloseLoopSpeed);
        GameXBallPaddleDistSetting _gameBallPaddleDistX = GameXBallPaddleDistSetting(_gameSettings);
        GameXBallBounceInversionOvershootSetting _gameBallBounceInvOvershoot60 = GameXBallBounceInversionOvershootSetting(_gameSettings, 0);
        GameXBallBounceInversionOvershootSetting _gameBallBounceInvOvershoot70 = GameXBallBounceInversionOvershootSetting(_gameSettings, 1);
        GameXBallBounceInversionOvershootSetting _gameBallBounceInvOvershoot80 = GameXBallBounceInversionOvershootSetting(_gameSettings, 2);
        GameXBallBounceInversionOvershootSetting _gameBallBounceInvOvershoot90 = GameXBallBounceInversionOvershootSetting(_gameSettings, 3);
        GameXBallBounceInversionOvershootSetting _gameBallBounceInvOvershoot100 = GameXBallBounceInversionOvershootSetting(_gameSettings, 4);

        ISetting* _settings[9] = {
            &_gameBallStartSpeedX, &_gameBallSpeedBounceIncX, &_gameMinSpeedX,
            &_gameBallPaddleDistX,
            &_gameBallBounceInvOvershoot60, &_gameBallBounceInvOvershoot70, 
            &_gameBallBounceInvOvershoot80, &_gameBallBounceInvOvershoot90, 
            &_gameBallBounceInvOvershoot100
        };

    public:
        SettingsGameAxisXGroup(GameXAxisSettings& settings) : _gameSettings(settings) {}

        const char* getName() const;
        const char* getTitle() const;

        ISetting** getSettings();

        uint16_t getSettingsCount() const {
            return sizeof(_settings) / sizeof(ISetting*);
        }
};