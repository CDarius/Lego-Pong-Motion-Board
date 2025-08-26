#pragma once

#include "settings/setting.hpp"
#include "game/encodemultijog.hpp"

class SettingJogXMultiplier : public Setting<float> {
    private:
        encoder_multi_jog_config_t& _config;

    public:
        SettingJogXMultiplier(encoder_multi_jog_config_t& config) : _config(config) {}

        float getValue() const override {
            return _config.x_encoder_multiplier;
        }

        void setValue(float value) override {
            _config.x_encoder_multiplier = value;
        }

        const char* getName() const override {
            return "x_multiplier";
        }

        const char* getTitle() const override {
            return "X-Axis multiplier";
        }

        const char* getDescription() const override {
            return "X-Axis multiplier from encoder encoder counts to axis position";
        }

        const char* getUnit() const override {
            return "stud/count";
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