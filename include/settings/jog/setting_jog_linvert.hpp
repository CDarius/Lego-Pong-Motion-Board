#pragma once

#include "settings/setting.hpp"
#include "game/encodermultijog.hpp"

class SettingJogLInvertEncoder : public SettingBool {
    private:
        encoder_multi_jog_config_t& _config;

    public:
        SettingJogLInvertEncoder(encoder_multi_jog_config_t& config) : _config(config) {}

        bool getValue() const override {
            return _config.l_encoder_invert;
        }

        void setValue(bool value) override {
            _config.l_encoder_invert = value;
        }

        const char* getName() const override {
            return "l_enc_invert";
        }

        const char* getTitle() const override {
            return "L-Axis jog inversion";
        }

        const char* getDescription() const override {
            return "Invert the jog direction for L-Axis";
        }
};