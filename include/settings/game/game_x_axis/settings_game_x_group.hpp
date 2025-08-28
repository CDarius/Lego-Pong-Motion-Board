#pragma once

#include "settings/setting.hpp"
#include "settings/settings_group.hpp"
#include "game/game_settings.hpp"
#include "setting_game_x_ballstartspeed.hpp"
#include "setting_game_x_ballspeedbounceinc.hpp"
#include "setting_game_x_ballbounceinverson.hpp"

class SettingsGameAxisXGroup : public SettingsGroup {
    private:
        GameXAxisSettings& _gameSettings;

        GameXBallStartSpeedSetting _gameBallStartSpeedX = GameXBallStartSpeedSetting(_gameSettings);
        GameXBallSpeedBounceIncrementSetting _gameBallSpeedBounceIncX = GameXBallSpeedBounceIncrementSetting(_gameSettings);
        GameXBallBounceInversionOvershootSetting _gameBallBounceInvOvershoot50 = GameXBallBounceInversionOvershootSetting(_gameSettings, 0);
        GameXBallBounceInversionOvershootSetting _gameBallBounceInvOvershoot60 = GameXBallBounceInversionOvershootSetting(_gameSettings, 1);
        GameXBallBounceInversionOvershootSetting _gameBallBounceInvOvershoot70 = GameXBallBounceInversionOvershootSetting(_gameSettings, 2);
        GameXBallBounceInversionOvershootSetting _gameBallBounceInvOvershoot80 = GameXBallBounceInversionOvershootSetting(_gameSettings, 3);
        GameXBallBounceInversionOvershootSetting _gameBallBounceInvOvershoot90 = GameXBallBounceInversionOvershootSetting(_gameSettings, 4);
        GameXBallBounceInversionOvershootSetting _gameBallBounceInvOvershoot100 = GameXBallBounceInversionOvershootSetting(_gameSettings, 5);

        ISetting* _settings[8] = {
            &_gameBallStartSpeedX, &_gameBallSpeedBounceIncX,
            &_gameBallBounceInvOvershoot50, &_gameBallBounceInvOvershoot60,
            &_gameBallBounceInvOvershoot70, &_gameBallBounceInvOvershoot80,
            &_gameBallBounceInvOvershoot90, &_gameBallBounceInvOvershoot100
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