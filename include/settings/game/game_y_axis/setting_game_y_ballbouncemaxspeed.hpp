#pragma once

#include "game\game_settings.hpp"
#include "settings\setting.hpp"

class GameYBallBounceMaxSpeedSetting : public SettingUInt8 {
    private:
        GameYAxisSettings& _settings;

    public:
        GameYBallBounceMaxSpeedSetting(GameYAxisSettings& settings) : _settings(settings) {}

        uint8_t getValue() const override {
            return _settings.bounceSpeedMax;
        }

        void setValue(const uint8_t value) override {
            _settings.bounceSpeedMax = value;
        }

        const char* getName() const override {
            return "ball_y_spd_max";
        }

        const char* getTitle() const override {
            return "Ball maximum bounce speed";
        }

        const char* getDescription() const override {
            return "Ball Y-Axis maximum speed for serve or bounce";
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
