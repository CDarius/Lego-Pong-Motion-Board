#pragma once

#include "settings/setting.hpp"
#include "game/game_settings.hpp"

class AIPlayerMaxErrorSetting : public SettingFloat {
    private:
        GameAIPlayerSettings& _settings;

    public:
        AIPlayerMaxErrorSetting(GameAIPlayerSettings& settings) : _settings(settings) {}

        float getValue() const override {
            return _settings.paddleMaxError;
        }

        void setValue(const float value) override {
            _settings.paddleMaxError = value;
        }

        const char* getName() const override {
            return "max_error";
        }

        const char* getTitle() const override {
            return "Maximum position error";
        }

        const char* getDescription() const override {
            return "Axis maximum AI player position error when tracking the ball";
        }

        const char* getUnit() const override {
            return "stud";
        }

        const bool hasMinValue() const override {
            return true;
        }

        const float getMinValue() const override {
            return 2.0;
        }

        const bool hasMaxValue() const override {
            return true;
        }

        const float getMaxValue() const override {
            return 20.0;
        }

        const bool hasChangeStep() const override {
            return true;
        }

        const float getChangeStep() const override {
            return 1.0;
        }
    };