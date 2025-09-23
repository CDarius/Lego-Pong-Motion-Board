#pragma once

#include "game/game_settings.hpp"
#include "settings/setting.hpp"

class GameXBallPaddleDistSetting : public SettingFloat {
    private:
        GameXAxisSettings& _settings;

    public:
        GameXBallPaddleDistSetting(GameXAxisSettings& settings) : _settings(settings) {}

        float getValue() const override {
            return _settings.ballPaddleDistance;
        }

        void setValue(const float value) override {
            _settings.ballPaddleDistance = value;
        }

        const char* getName() const override {
            return "ball_pdl_dist";
        }

        const char* getTitle() const override {
            return "Ball Paddle Distance";
        }

        const char* getDescription() const override {
            return "Ball distance from paddle of the serving player";
        }

        const char* getUnit() const override {
            return "stud";
        }

        const bool hasMinValue() const override {
            return true;
        }

        const float getMinValue() const override {
            return 0.0;
        }

        const bool hasMaxValue() const override {
            return true;
        }

        const float getMaxValue() const override {
            return 1.5;
        }

        const bool hasChangeStep() const override {
            return true;
        }

        const float getChangeStep() const override {
            return 0.1;
        }
    };