#pragma once

#include "settings/setting.hpp"
#include "game/encodermultijog.hpp"

class SettingJogHighSpeedFactor : public SettingFloat {
    private:
        encoder_multi_jog_config_t& _config;

    public:
        SettingJogHighSpeedFactor(encoder_multi_jog_config_t& config) : _config(config) {}

        float getValue() const override {
            return _config.high_speed_multiplier_factor;
        }

        void setValue(float value) override {
            _config.high_speed_multiplier_factor = value;
            _config.update_counter++;
        }

        const char* getName() const override {
            return "high_spd_factor";
        }

        const char* getTitle() const override {
            return "High speed factor";
        }

        const char* getDescription() const override {
            return "Multiplier factor to apply to encoder_multiplier when reach high speed";
        }

        const char* getUnit() const override {
            return "";
        }

        const bool hasMinValue() const override {
            return true;
        }

        const float getMinValue() const override {
            return 1.0;
        }

        const bool hasMaxValue() const override {
            return true;
        }

        const float getMaxValue() const override {
            return 10.0;
        }

        const bool hasChangeStep() const override {
            return true;
        }

        const float getChangeStep() const override {
            return 0.1;
        }
};