#pragma once

#include "settings/setting.hpp"
#include "game/encodermultijog.hpp"

class SettingJogHighEncSpeedCount : public SettingUInt8 {
    private:
        encoder_multi_jog_config_t& _config;

    public:
        SettingJogHighEncSpeedCount(encoder_multi_jog_config_t& config) : _config(config) {}

        uint8_t getValue() const override {
            return _config.high_enc_speed_count;
        }

        void setValue(uint8_t value) override {
            _config.high_enc_speed_count = value;
            _config.update_counter++;
        }

        const char* getName() const override {
            return "high_speed_cnt";
        }

        const char* getTitle() const override {
            return "High Speed Count";
        }

        const char* getDescription() const override {
            return "Number of encoder counts to consider user encoder rotation as high speed";
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