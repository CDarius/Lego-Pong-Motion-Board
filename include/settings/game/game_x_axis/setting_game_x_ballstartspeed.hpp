#pragma once

#include "game\game_settings.hpp"
#include "settings\setting.hpp"

class GameXBallStartSpeedSetting : public SettingUInt8 {
    private:
        GameXAxisSettings& _settings;

    public:
        GameXBallStartSpeedSetting(GameXAxisSettings& settings) : _settings(settings) {}

        uint8_t getValue() const override {
            return _settings.startBallGameSpeed;
        }

        void setValue(const uint8_t value) override {
            _settings.startBallGameSpeed = value;
        }

        const char* getName() const override {
            return "ball_x_start_sp";
        }

        const char* getTitle() const override {
            return "Ball start speed on X-Axis";
        }

        const char* getDescription() const override {
            return "Ball X-Axis speed when a new game start or after a goal";
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
