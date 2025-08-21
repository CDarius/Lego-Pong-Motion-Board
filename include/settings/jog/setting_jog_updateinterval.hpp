#pragma once

#include "settings/setting.hpp"
#include "game/encodemultijog.hpp"

class SettingJogUpdateInterval : public SettingUInt16 {
    private:
        encoder_multi_jog_config_t& _config;

    public:
        SettingJogUpdateInterval(encoder_multi_jog_config_t& config) : _config(config) {}

        uint16_t getValue() const override {
            return _config.update_interval_ms;
        }

        void setValue(uint16_t value) override {
            _config.update_interval_ms = value;
        }

        const char* getName() const override {
            return "update_interval";
        }

        const char* getTitle() const override {
            return "Update interval";
        }

        const char* getDescription() const override {
            return "Axis position update interval";
        }

        const char* getUnit() const override {
            return "ms";
        }

        SettingType getType() const {
            return SettingType::UInt16;
        }

        const uint16_t getMinValue() const override {
            return 1;
        }

        const uint16_t getMaxValue() const override {
            return 500;
        }

        const uint16_t getChangeStep() const override {
            return 5;
        }
};