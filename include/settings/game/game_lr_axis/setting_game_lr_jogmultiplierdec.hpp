#pragma once

#include "game/game_settings.hpp"
#include "settings/setting.hpp"

class GameLRPaddleJogMultiplierDecrementSetting : public Setting<float> {
    private:
        GameLRAxisSettings& _settings;

    public:
        GameLRPaddleJogMultiplierDecrementSetting(GameLRAxisSettings& settings) : _settings(settings) {}

        float getValue() const override {
            return _settings.jogMultiplierDecrement;
        }

        void setValue(const float value) override {
            _settings.jogMultiplierDecrement = value;
        }

        const char* getName() const override {
            return "jog_multi_dec";
        }

        const char* getTitle() const override {
            return "Jog multiplier decrement";
        }

        const char* getDescription() const override {
            return "Jog multiplier decrement applied at left/right paddles during a game at every bounce on a paddle";
        }

        const char* getUnit() const override {
            return "stud/count";
        }

        SettingType getType() const {
            return SettingType::FloatType;
        }

        const bool hasMinValue() const override {
            return true;
        }

        const float getMinValue() const override {
            return 0.01;
        }

        const bool hasMaxValue() const override {
            return true;
        }

        const float getMaxValue() const override {
            return 0.4;
        }

        const bool hasChangeStep() const override {
            return true;
        }

        const float getChangeStep() const override {
            return 0.01;
        }
    };