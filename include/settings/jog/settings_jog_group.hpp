#pragma once

#include "settings/setting.hpp"
#include "settings/settings_group.hpp"
#include "game/encodermultijog.hpp"
#include "setting_jog_updateinterval.hpp"
#include "setting_jog_xmultiplier.hpp"
#include "setting_jog_ymultiplier.hpp"
#include "setting_jog_lrmultiplier.hpp"
#include "setting_jog_xtrack.hpp"
#include "setting_jog_ytrack.hpp"
#include "setting_jog_lrtrack.hpp"
#include "setting_jog_linvert.hpp"
#include "setting_jog_rinvert.hpp"
#include "setting_jog_mediumencspeed.hpp"
#include "setting_jog_highencspeed.hpp"
#include "setting_jog_mediumspeedfactor.hpp"
#include "setting_jog_highspeedfactor.hpp"

class SettingsJogGroup : public SettingsGroup {
    private:
        encoder_multi_jog_config_t& _config;

        SettingJogUpdateInterval _settingJogUpdateInterval = SettingJogUpdateInterval(_config);
        SettingJogXMultiplier _settingJogXMultiplier = SettingJogXMultiplier(_config);
        SettingJogYMultiplier _settingJogYMultiplier = SettingJogYMultiplier(_config);
        SettingJogLRMultiplier _settingJogLRMultiplier = SettingJogLRMultiplier(_config);
        SettingJogXUseTrack _settingJogXUseTrack = SettingJogXUseTrack(_config);
        SettingJogYUseTrack _settingJogYUseTrack = SettingJogYUseTrack(_config);
        SettingJogLRUseTrack _settingJogLRUseTrack = SettingJogLRUseTrack(_config);
        SettingJogLInvertEncoder _settingJogLInvertEncoder = SettingJogLInvertEncoder(_config);
        SettingJogRInvertEncoder _settingJogRInvertEncoder = SettingJogRInvertEncoder(_config);
        SettingJogMediumEncSpeedCount _settingJogMediumEncSpeedCount = SettingJogMediumEncSpeedCount(_config);
        SettingJogHighEncSpeedCount _settingJogHighEncSpeedCount = SettingJogHighEncSpeedCount(_config);
        SettingJogMediumSpeedFactor _settingJogMediumSpeedFactor = SettingJogMediumSpeedFactor(_config);
        SettingJogHighSpeedFactor _settingJogHighSpeedFactor = SettingJogHighSpeedFactor(_config);

        ISetting* _settings[13] = {
            &_settingJogUpdateInterval,
            &_settingJogXMultiplier,
            &_settingJogYMultiplier,
            &_settingJogLRMultiplier,
            &_settingJogXUseTrack,
            &_settingJogYUseTrack,
            &_settingJogLRUseTrack,
            &_settingJogLInvertEncoder,
            &_settingJogRInvertEncoder,
            &_settingJogMediumEncSpeedCount,
            &_settingJogHighEncSpeedCount,
            &_settingJogMediumSpeedFactor,
            &_settingJogHighSpeedFactor
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