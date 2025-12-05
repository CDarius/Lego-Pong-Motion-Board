#pragma once

#include "game\game_settings.hpp"
#include "settings\setting.hpp"

class GameXBallSpeedBounceIncrementSetting : public SettingFloat {
    private:
        GameXAxisSettings& _settings;

    public:
        GameXBallSpeedBounceIncrementSetting(GameXAxisSettings& settings) : _settings(settings) {}

        float getValue() const override {
            return _settings.ballBounceSpeedIncrement;
        }

        void setValue(const float value) override {
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
            return "stud/s";
        }

        const bool hasMinValue() const override {
            return true;
        }

        const float getMinValue() const override {
            return 1.0f;
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
