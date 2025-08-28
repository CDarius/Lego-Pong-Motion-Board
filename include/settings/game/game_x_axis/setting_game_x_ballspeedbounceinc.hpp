#pragma once

#include "game\game_settings.hpp"
#include "settings\setting.hpp"

class GameXBallSpeedBounceIncrementSetting : public SettingUInt8 {
    private:
        GameXAxisSettings& _settings;

    public:
        GameXBallSpeedBounceIncrementSetting(GameXAxisSettings& settings) : _settings(settings) {}

        uint8_t getValue() const override {
            return _settings.ballBounceSpeedIncrement;
        }

        void setValue(const uint8_t value) override {
            _settings.ballBounceSpeedIncrement = value;
        }

        const char* getName() const override {
            return "ball_x_bspd_inc";
        }

        const char* getTitle() const override {
            return "Ball X-Axis bounce speed increment";
        }

        const char* getDescription() const override {
            return "Ball X-Axis speed increment after bounce on paddle";
        }

        const char* getUnit() const override {
            return "%";
        }

        const uint8_t getMinValue() const override {
            return 1;
        }

        const uint8_t getMaxValue() const override {
            return 20;
        }
    };
