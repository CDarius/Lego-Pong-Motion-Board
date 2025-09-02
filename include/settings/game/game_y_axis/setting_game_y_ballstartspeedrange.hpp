#pragma once

#include "game\game_settings.hpp"
#include "settings\setting.hpp"

class GameYBallStartSpeedRangeSetting : public SettingUInt8 {
    private:
        GameYAxisSettings& _settings;

    public:
        GameYBallStartSpeedRangeSetting(GameYAxisSettings& settings) : _settings(settings) {}

        uint8_t getValue() const override {
            return _settings.startSpeedRange;
        }

        void setValue(const uint8_t value) override {
            _settings.startSpeedRange = value;
        }

        const char* getName() const override {
            return "ball_y_start_sp";
        }

        const char* getTitle() const override {
            return "Ball start speed range on Y-Axis";
        }

        const char* getDescription() const override {
            return "Ball Y-Axis speed range for serve when a new game start or after a goal";
        }

        const char* getUnit() const override {
            return "%";
        }

        const uint8_t getMinValue() const override {
            return 10;
        }

        const uint8_t getMaxValue() const override {
            return 100;
        }

        const uint8_t getChangeStep() const override {
            return 5;
        }
    };
