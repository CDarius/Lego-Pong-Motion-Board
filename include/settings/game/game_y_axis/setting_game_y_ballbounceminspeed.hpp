#pragma once

#include "game\game_settings.hpp"
#include "settings\setting.hpp"

class GameYBallBounceMinSpeedSetting : public SettingUInt8 {
    private:
        GameYAxisSettings& _settings;

    public:
        GameYBallBounceMinSpeedSetting(GameYAxisSettings& settings) : _settings(settings) {}

        uint8_t getValue() const override {
            return _settings.bounceSpeedMin;
        }

        void setValue(const uint8_t value) override {
            _settings.bounceSpeedMin = value;
        }

        const char* getName() const override {
            return "ball_y_spd_min";
        }

        const char* getTitle() const override {
            return "Ball minimum bounce speed";
        }

        const char* getDescription() const override {
            return "Ball Y-Axis minimum speed for serve or bounce";
        }

        const char* getUnit() const override {
            return "%";
        }

        const uint8_t getMinValue() const override {
            return 40;
        }

        const uint8_t getMaxValue() const override {
            return 60;
        }

        const uint8_t getChangeStep() const override {
            return 5;
        }
    };
