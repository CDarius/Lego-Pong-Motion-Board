#pragma once

#include "settings/setting.hpp"
#include "game/encodermultijog.hpp"

class SettingJogXUseTrack : public SettingBool {
    private:
        encoder_multi_jog_config_t& _config;

    public:
        SettingJogXUseTrack(encoder_multi_jog_config_t& config) : _config(config) {}

        bool getValue() const override {
            return _config.x_use_track;
        }

        void setValue(bool value) override {
            _config.x_use_track = value;
            _config.update_counter++;
        }

        const char* getName() const override {
            return "x_use_track";
        }

        const char* getTitle() const override {
            return "X-Axis jog move mode";
        }

        const char* getDescription() const override {
            return "Use track (true) or run_target (false) to jog X-Axis";
        }
};