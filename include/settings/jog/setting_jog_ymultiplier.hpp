#pragma once

#include "settings/setting.hpp"
#include "game/encodemultijog.hpp"

class SettingJogYMultiplier : public Setting<float> {
    private:
        encoder_multi_jog_config_t& _config;

    public:
        SettingJogYMultiplier(encoder_multi_jog_config_t& config) : _config(config) {}

        float getValue() const override {
            return _config.y_encoder_multiplier;
        }

        void setValue(float value) override {
            _config.y_encoder_multiplier = value;
        }

        const char* getName() const override {
            return "y_multiplier";
        }

        const char* getTitle() const override {
            return "Y-Axis multiplier";
        }

        const char* getDescription() const override {
            return "Y-Axis multiplier from encoder encoder counts to axis position";
        }

        const char* getUnit() const override {
            return "";
        }

        SettingType getType() const {
            return SettingType::FloatType;
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