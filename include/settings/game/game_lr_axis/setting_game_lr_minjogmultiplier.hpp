#pragma once

#include "game/game_settings.hpp"
#include "settings/setting.hpp"

class GameLRPaddleMinJogMultiplierSetting : public Setting<float> {
    private:
        GameLRAxisSettings& _settings;

    public:
        GameLRPaddleMinJogMultiplierSetting(GameLRAxisSettings& settings) : _settings(settings) {}

        float getValue() const override {
            return _settings.minJogEncoderMultiplier;
        }

        void setValue(const float value) override {
            _settings.minJogEncoderMultiplier = value;
        }

        const char* getName() const override {
            return "min_jog_multi";
        }

        const char* getTitle() const override {
            return "Minimum jog multiplier";
        }

        const char* getDescription() const override {
            return "Minimum jog multiplier applied at left/right paddles during a game";
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
            return 0.1;
        }

        const bool hasMaxValue() const override {
            return true;
        }

        const float getMaxValue() const override {
            return 0.8;
        }

        const bool hasChangeStep() const override {
            return true;
        }

        const float getChangeStep() const override {
            return 0.1;
        }
    };