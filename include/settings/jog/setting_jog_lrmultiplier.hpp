#pragma once

#include "settings/setting.hpp"
#include "game/encodermultijog.hpp"

class SettingJogLRMultiplier : public SettingFloat {
    private:
        encoder_multi_jog_config_t& _config;

    public:
        SettingJogLRMultiplier(encoder_multi_jog_config_t& config) : _config(config) {}

        float getValue() const override {
            return _config.l_r_encoder_multiplier;
        }

        void setValue(float value) override {
            _config.l_r_encoder_multiplier = value;
            _config.update_counter++;
        }

        const char* getName() const override {
            return "l_r_multiplier";
        }

        const char* getTitle() const override {
            return "L/R-Axis multiplier";
        }

        const char* getDescription() const override {
            return "L/R-Axis multiplier from encoder encoder counts to axis position";
        }

        const char* getUnit() const override {
            return "stud/count";
        }

        const bool hasMinValue() const override {
            return true;
        }

        const float getMinValue() const override {
            return 0.01;
        }

        const bool hasMaxValue() const override {
            return true;
        }

        const float getMaxValue() const override {
            return 2.0;
        }

        const bool hasChangeStep() const override {
            return true;
        }

        const float getChangeStep() const override {
            return 0.1;
        }
};