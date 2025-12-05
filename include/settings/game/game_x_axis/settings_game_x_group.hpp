#pragma once

#include "settings/setting.hpp"
#include "settings/settings_group.hpp"
#include "game/game_settings.hpp"
#include "setting_game_x_ballstartspeed.hpp"
#include "setting_game_x_ballspeedbounceinc.hpp"
#include "setting_game_x_ballbouncemaxspeed.hpp"
#include "setting_game_x_ballbounceinverson.hpp"
#include "setting_game_x_ballpaddledist.hpp"
#include "settings/game/setting_axis_minspeed.hpp"
#include "settings/game/setting_game_paddletol.hpp"

class SettingsGameAxisXGroup : public SettingsGroup {
    private:
        GameXAxisSettings& _gameSettings;

        GameXBallStartSpeedSetting _gameBallStartSpeedX = GameXBallStartSpeedSetting(_gameSettings);
        GameXBallSpeedBounceIncrementSetting _gameBallSpeedBounceIncX = GameXBallSpeedBounceIncrementSetting(_gameSettings);
        GameXBallBounceMaxSpeedSetting _gameBallBounceMaxSpeedX = GameXBallBounceMaxSpeedSetting(_gameSettings);
        GameMinSpeedSetting _gameMinSpeedX = GameMinSpeedSetting(_gameSettings.minCloseLoopSpeed);
        GameXBallPaddleDistSetting _gameBallPaddleDistX = GameXBallPaddleDistSetting(_gameSettings);
        GamePaddleCollisionToleranceSetting _gamePaddleCollisionToleranceX = GamePaddleCollisionToleranceSetting(_gameSettings.paddleCollisionTolerance);
        GameXBallBounceInversionOvershootSetting _gameBallBounceInvOvershoot10 = GameXBallBounceInversionOvershootSetting(_gameSettings, 0);
        GameXBallBounceInversionOvershootSetting _gameBallBounceInvOvershoot15 = GameXBallBounceInversionOvershootSetting(_gameSettings, 1);
        GameXBallBounceInversionOvershootSetting _gameBallBounceInvOvershoot20 = GameXBallBounceInversionOvershootSetting(_gameSettings, 2);
        GameXBallBounceInversionOvershootSetting _gameBallBounceInvOvershoot25 = GameXBallBounceInversionOvershootSetting(_gameSettings, 3);
        GameXBallBounceInversionOvershootSetting _gameBallBounceInvOvershoot30 = GameXBallBounceInversionOvershootSetting(_gameSettings, 4);

        ISetting* _settings[11] = {
            &_gameBallStartSpeedX, &_gameBallSpeedBounceIncX, &_gameBallBounceMaxSpeedX,
            &_gameMinSpeedX,
            &_gameBallPaddleDistX, &_gamePaddleCollisionToleranceX,
            &_gameBallBounceInvOvershoot10, &_gameBallBounceInvOvershoot15,
            &_gameBallBounceInvOvershoot20, &_gameBallBounceInvOvershoot25, 
            &_gameBallBounceInvOvershoot30
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