#pragma once

#include "settings/setting.hpp"
#include "settings/settings_group.hpp"
#include "game/game_settings.hpp"
#include "setting_game_y_ballbounceinverson.hpp"
#include "setting_game_y_ballbounceminspeed.hpp"
#include "setting_game_y_ballbouncemaxspeed.hpp"
#include "settings/game/setting_axis_minspeed.hpp"
#include "settings/game/setting_game_paddletol.hpp"

class SettingsGameAxisYGroup : public SettingsGroup {
    private:
        GameYAxisSettings& _gameSettings;

        GameYBallBounceMinSpeedSetting _gameBallBounceMinSpeed = GameYBallBounceMinSpeedSetting(_gameSettings);
        GameYBallBounceMaxSpeedSetting _gameBallBounceMaxSpeed = GameYBallBounceMaxSpeedSetting(_gameSettings);
        GameMinSpeedSetting _gameMinSpeedY = GameMinSpeedSetting(_gameSettings.minCloseLoopSpeed);
        GamePaddleCollisionToleranceSetting _gamePaddleCollisionToleranceY = GamePaddleCollisionToleranceSetting(_gameSettings.paddleCollisionTolerance);
        GameYBallBounceInversionOvershootSetting _gameBallBounceInvOvershoot60 = GameYBallBounceInversionOvershootSetting(_gameSettings, 0);
        GameYBallBounceInversionOvershootSetting _gameBallBounceInvOvershoot70 = GameYBallBounceInversionOvershootSetting(_gameSettings, 1);
        GameYBallBounceInversionOvershootSetting _gameBallBounceInvOvershoot80 = GameYBallBounceInversionOvershootSetting(_gameSettings, 2);
        GameYBallBounceInversionOvershootSetting _gameBallBounceInvOvershoot90 = GameYBallBounceInversionOvershootSetting(_gameSettings, 3);
        GameYBallBounceInversionOvershootSetting _gameBallBounceInvOvershoot100 = GameYBallBounceInversionOvershootSetting(_gameSettings, 4);

        ISetting* _settings[9] = {
            &_gameBallBounceMinSpeed, &_gameBallBounceMaxSpeed,
            &_gameMinSpeedY, &_gamePaddleCollisionToleranceY,
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