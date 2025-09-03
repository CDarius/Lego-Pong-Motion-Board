#pragma once

#include "settings/setting.hpp"
#include "settings/settings_group.hpp"
#include "game/game_settings.hpp"
#include "setting_game_lr_minjogmultiplier.hpp"
#include "setting_game_lr_jogmultiplierdec.hpp"

class SettingsGameAxisLRGroup : public SettingsGroup {
    private:
        GameLRAxisSettings& _gameSettings;

        GameLRPaddleMinJogMultiplierSetting _gameLRPaddleMinJogMultiplier = GameLRPaddleMinJogMultiplierSetting(_gameSettings);
        GameLRPaddleJogMultiplierDecrementSetting _gameLRPaddleJogMultiplierDecrement = GameLRPaddleJogMultiplierDecrementSetting(_gameSettings);

        ISetting* _settings[2] = {
            &_gameLRPaddleMinJogMultiplier, &_gameLRPaddleJogMultiplierDecrement
        };

    public:
        SettingsGameAxisLRGroup(GameLRAxisSettings& settings) : _gameSettings(settings) {}

        const char* getName() const;
        const char* getTitle() const;

        ISetting** getSettings();

        uint16_t getSettingsCount() const {
            return sizeof(_settings) / sizeof(ISetting*);
        }
};