#pragma once

#include "game\game_settings.hpp"
#include "settings\setting.hpp"

class GameXBallStartSpeedSetting : public SettingFloat {
    private:
        GameXAxisSettings& _settings;

    public:
        GameXBallStartSpeedSetting(GameXAxisSettings& settings) : _settings(settings) {}

        float getValue() const override {
            return _settings.startBallGameSpeed;
        }

        void setValue(const float value) override {
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
            return "stud/s";
        }

        const bool hasMinValue() const override {
            return true;
        }

        const float getMinValue() const override {
            return 2.0f;
        }

        const bool hasMaxValue() const override {
            return true;
        }

        const float getMaxValue() const override {
            return 20.0f;
        }

        const bool hasChangeStep() const override {
            return true;
        }

        const float getChangeStep() const override {
            return 1.0f;
        }
    };
