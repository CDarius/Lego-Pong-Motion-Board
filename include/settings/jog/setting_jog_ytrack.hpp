#pragma once

#include "settings/setting.hpp"
#include "game/encodermultijog.hpp"

class SettingJogYUseTrack : public SettingBool {
    private:
        encoder_multi_jog_config_t& _config;

    public:
        SettingJogYUseTrack(encoder_multi_jog_config_t& config) : _config(config) {}

        bool getValue() const override {
            return _config.y_use_track;
        }

        void setValue(bool value) override {
            _config.y_use_track = value;
            _config.update_counter++;
        }

        const char* getName() const override {
            return "y_use_track";
        }

        const char* getTitle() const override {
            return "Y-Axis jog move mode";
        }

        const char* getDescription() const override {
            return "Use track (true) or run_target (false) to jog Y-Axis";
        }
};