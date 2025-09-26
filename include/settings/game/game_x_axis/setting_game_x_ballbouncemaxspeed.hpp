#pragma once

#include "game\game_settings.hpp"
#include "settings\setting.hpp"

class GameXBallBounceMaxSpeedSetting : public SettingFloat {
    private:
        GameXAxisSettings& _settings;

    public:
        GameXBallBounceMaxSpeedSetting(GameXAxisSettings& settings) : _settings(settings) {}

        float getValue() const override {
            return _settings.ballBounceMaxSpeed;
        }

        void setValue(const float value) override {
            _settings.ballBounceMaxSpeed = value;
        }

        const char* getName() const override {
            return "ball_x_bnc_max";
        }

        const char* getTitle() const override {
            return "Ball maximum bounce speed";
        }

        const char* getDescription() const override {
            return "Ball X-Axis maximum bounce speed";
        }

        const char* getUnit() const override {
            return "stud/s";
        }

        const bool hasMinValue() const override {
            return true;
        }

        const float getMinValue() const override {
            return 10.0f;
        }

        const bool hasMaxValue() const override {
            return true;
        }

        const float getMaxValue() const override {
            return 60.0f;
        }

        const bool hasChangeStep() const override {
            return true;
        }
        
        const float getChangeStep() const override {
            return 1.0f;
        }
    };
