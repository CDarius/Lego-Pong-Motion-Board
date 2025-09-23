#pragma once

#include "settings/setting.hpp"
#include "game/encodermultijog.hpp"

class SettingJogRInvertEncoder : public SettingBool {
    private:
        encoder_multi_jog_config_t& _config;

    public:
        SettingJogRInvertEncoder(encoder_multi_jog_config_t& config) : _config(config) {}

        bool getValue() const override {
            return _config.r_encoder_invert;
        }

        void setValue(bool value) override {
            _config.r_encoder_invert = value;
        }

        const char* getName() const override {
            return "r_enc_invert";
        }

        const char* getTitle() const override {
            return "R-Axis jog inversion";
        }

        const char* getDescription() const override {
            return "Invert the jog direction for R-Axis";
        }
};