#pragma once

#include "settings/setting.hpp"
#include "settings/settings_group.hpp"
#include "game/encodermultijog.hpp"
#include "setting_jog_updateinterval.hpp"
#include "setting_jog_xmultiplier.hpp"
#include "setting_jog_ymultiplier.hpp"
#include "setting_jog_lrmultiplier.hpp"
#include "setting_jog_linvert.hpp"
#include "setting_jog_rinvert.hpp"

class SettingsJogGroup : public SettingsGroup {
    private:
        encoder_multi_jog_config_t& _config;

        SettingJogUpdateInterval _settingJogUpdateInterval = SettingJogUpdateInterval(_config);
        SettingJogXMultiplier _settingJogXMultiplier = SettingJogXMultiplier(_config);
        SettingJogYMultiplier _settingJogYMultiplier = SettingJogYMultiplier(_config);
        SettingJogLRMultiplier _settingJogLRMultiplier = SettingJogLRMultiplier(_config);
        SettingJogLInvertEncoder _settingJogLInvertEncoder = SettingJogLInvertEncoder(_config);
        SettingJogRInvertEncoder _settingJogRInvertEncoder = SettingJogRInvertEncoder(_config);

        ISetting* _settings[6] = {
            &_settingJogUpdateInterval,
            &_settingJogXMultiplier,
            &_settingJogYMultiplier,
            &_settingJogLRMultiplier,
            &_settingJogLInvertEncoder,
            &_settingJogRInvertEncoder
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