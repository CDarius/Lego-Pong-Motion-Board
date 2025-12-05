#pragma once

#include "settings/setting.hpp"
#include "game/encodermultijog.hpp"

class SettingJogLRUseTrack : public SettingBool {
    private:
        encoder_multi_jog_config_t& _config;

    public:
        SettingJogLRUseTrack(encoder_multi_jog_config_t& config) : _config(config) {}

        bool getValue() const override {
            return _config.l_r_use_track;
        }

        void setValue(bool value) override {
            _config.l_r_use_track = value;
            _config.update_counter++;
        }

        const char* getName() const override {
            return "l_r_use_track";
        }

        const char* getTitle() const override {
            return "L/R-Axis jog move mode";
        }

        const char* getDescription() const override {
            return "Use track (true) or run_target (false) to jog L/R-Axis";
        }
};