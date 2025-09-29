#pragma once

#include "settings/setting.hpp"
#include "game/encodermultijog.hpp"

class SettingJogMediumEncSpeedCount : public SettingUInt8 {
    private:
        encoder_multi_jog_config_t& _config;

    public:
        SettingJogMediumEncSpeedCount(encoder_multi_jog_config_t& config) : _config(config) {}

        uint8_t getValue() const override {
            return _config.medium_enc_speed_count;
        }

        void setValue(uint8_t value) override {
            _config.medium_enc_speed_count = value;
            _config.update_counter++;
        }

        const char* getName() const override {
            return "med_speed_cnt";
        }

        const char* getTitle() const override {
            return "Medium Speed Count";
        }

        const char* getDescription() const override {
            return "Number of encoder counts to consider user encoder rotation as medium speed";
        }

        const char* getUnit() const override {
            return "counts";
        }

        const uint8_t getMinValue() const override {
            return 1;
        }

        const uint8_t getMaxValue() const override {
            return 100;
        }
};