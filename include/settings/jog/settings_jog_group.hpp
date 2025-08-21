#pragma once

#include "settings/setting.hpp"
#include "settings/settings_group.hpp"
#include "game/encodemultijog.hpp"
#include "setting_jog_updateinterval.hpp"
#include "setting_jog_xmultiplier.hpp"
#include "setting_jog_ymultiplier.hpp"
#include "setting_jog_lrmultiplier.hpp"

class SettingsJogGroup : public SettingsGroup {
    private:
        encoder_multi_jog_config_t& _config;

        SettingJogUpdateInterval _settingJogUpdateInterval = SettingJogUpdateInterval(_config);
        SettingJogXMultiplier _settingJogXMultiplier = SettingJogXMultiplier(_config);
        SettingJogYMultiplier _settingJogYMultiplier = SettingJogYMultiplier(_config);
        SettingJogLRMultiplier _settingJogLRMultiplier = SettingJogLRMultiplier(_config);

        ISetting* _settings[4] = {
            &_settingJogUpdateInterval,
            &_settingJogXMultiplier,
            &_settingJogYMultiplier,
            &_settingJogLRMultiplier
        };
        
    public:
        SettingsJogGroup(encoder_multi_jog_config_t& config) : _config(config) {}

        const char* getName() const;
        const char* getTitle() const;

        ISetting** getSettings();

        uint16_t getSettingsCount() const {
            return sizeof(_settings) / sizeof(ISetting*);
        }
};