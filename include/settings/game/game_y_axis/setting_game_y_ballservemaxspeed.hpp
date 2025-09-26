#pragma once

#include "game\game_settings.hpp"
#include "settings\setting.hpp"

class GameYBallServeMaxSpeedSetting : public SettingFloat {
    private:
        GameYAxisSettings& _settings;

    public:
        GameYBallServeMaxSpeedSetting(GameYAxisSettings& settings) : _settings(settings) {}

        float getValue() const override {
            return _settings.ballServeSpeedMax;
        }

        void setValue(const float value) override {
            _settings.ballServeSpeedMax = value;
        }

        const char* getName() const override {
            return "ball_y_srv_max";
        }

        const char* getTitle() const override {
            return "Ball maximum serve speed";
        }

        const char* getDescription() const override {
            return "Ball Y-Axis maximum serve speed";
        }

        const char* getUnit() const override {
            return "stud/second";
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
            return 30.0f;
        }

        const bool hasChangeStep() const override {
            return true;
        }
        
        const float getChangeStep() const override {
            return 1.0f;
        }
    };
