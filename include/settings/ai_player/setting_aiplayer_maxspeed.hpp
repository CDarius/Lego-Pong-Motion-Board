#pragma once

#include "settings/setting.hpp"
#include "game/game_settings.hpp"

class AIPlayerMaxSpeedSetting : public Setting<float> {
    private:
        GameAIPlayerSettings& _settings;

    public:
        AIPlayerMaxSpeedSetting(GameAIPlayerSettings& settings) : _settings(settings) {}

        float getValue() const override {
            return _settings.paddleMaxSpeed;
        }

        void setValue(const float value) override {
            _settings.paddleMaxSpeed = value;
        }

        const char* getName() const override {
            return "max_speed";
        }

        const char* getTitle() const override {
            return "Maximum speed";
        }

        const char* getDescription() const override {
            return "Axis maximum close speed for AI player";
        }

        const char* getUnit() const override {
            return "stud/s";
        }

        SettingType getType() const {
            return SettingType::FloatType;
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
            return 100.0;
        }

        const bool hasChangeStep() const override {
            return true;
        }

        const float getChangeStep() const override {
            return 1.0;
        }
    };