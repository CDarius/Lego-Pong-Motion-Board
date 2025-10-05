#pragma once

#include "settings/setting.hpp"
#include "settings/settings_group.hpp"
#include "game/game_settings.hpp"
#include "setting_game_y_ballbounceinverson.hpp"
#include "setting_game_y_ballservemaxspeed.hpp"
#include "setting_game_y_ballbouncemaxspeed.hpp"
#include "settings/game/setting_axis_minspeed.hpp"
#include "settings/game/setting_game_paddletol.hpp"

class SettingsGameAxisYGroup : public SettingsGroup {
    private:
        GameYAxisSettings& _gameSettings;

        GameYBallServeMaxSpeedSetting _gameBallServeMaxSpeed = GameYBallServeMaxSpeedSetting(_gameSettings);
        GameYBallBounceMaxSpeedSetting _gameBallBounceMaxSpeed = GameYBallBounceMaxSpeedSetting(_gameSettings);
        GameMinSpeedSetting _gameMinSpeedY = GameMinSpeedSetting(_gameSettings.minCloseLoopSpeed);
        GamePaddleCollisionToleranceSetting _gamePaddleCollisionToleranceY = GamePaddleCollisionToleranceSetting(_gameSettings.paddleCollisionTolerance);
        GameYBallBounceInversionOvershootSetting _gameBallBounceInvOvershoot5 = GameYBallBounceInversionOvershootSetting(_gameSettings, 0);
        GameYBallBounceInversionOvershootSetting _gameBallBounceInvOvershoot10 = GameYBallBounceInversionOvershootSetting(_gameSettings, 1);
        GameYBallBounceInversionOvershootSetting _gameBallBounceInvOvershoot15 = GameYBallBounceInversionOvershootSetting(_gameSettings, 2);
        GameYBallBounceInversionOvershootSetting _gameBallBounceInvOvershoot20 = GameYBallBounceInversionOvershootSetting(_gameSettings, 3);
        GameYBallBounceInversionOvershootSetting _gameBallBounceInvOvershoot25 = GameYBallBounceInversionOvershootSetting(_gameSettings, 4);
        GameYBallBounceInversionOvershootSetting _gameBallBounceInvOvershoot30 = GameYBallBounceInversionOvershootSetting(_gameSettings, 5);
        GameYBallBounceInversionOvershootSetting _gameBallBounceInvOvershoot35 = GameYBallBounceInversionOvershootSetting(_gameSettings, 6);
        GameYBallBounceInversionOvershootSetting _gameBallBounceInvOvershoot40 = GameYBallBounceInversionOvershootSetting(_gameSettings, 7);

        ISetting* _settings[12] = {
            &_gameBallServeMaxSpeed, &_gameBallBounceMaxSpeed,
            &_gameMinSpeedY, &_gamePaddleCollisionToleranceY,
            &_gameBallBounceInvOvershoot5, &_gameBallBounceInvOvershoot10,
            &_gameBallBounceInvOvershoot15, &_gameBallBounceInvOvershoot20,
            &_gameBallBounceInvOvershoot25, &_gameBallBounceInvOvershoot30,
            &_gameBallBounceInvOvershoot35, &_gameBallBounceInvOvershoot40
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