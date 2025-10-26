#pragma once

#include "settings/setting.hpp"
#include "game/game_settings.hpp"
#include "game/game.hpp"

class AIPlayerMaxErrorSetting : public SettingFloat {
    private:
        GameAIPlayerSettings& _settings;
        GameAILevel _aiLevel;

    public:
        AIPlayerMaxErrorSetting(GameAIPlayerSettings& settings, GameAILevel aiLevel) : _settings(settings), _aiLevel(aiLevel) {}

        float getValue() const override {
            return _settings.paddleMaxError;
        }

        void setValue(const float value) override {
            _settings.paddleMaxError = value;
        }

        const char* getName() const override {
            switch (_aiLevel) {
                case GameAILevel::EASY:
                    return "max_error_e";
                case GameAILevel::MEDIUM:
                    return "max_error_m";
                case GameAILevel::HARD:
                    return "max_error_h";
            }
            return "???";
        }

        const char* getTitle() const override {
            switch (_aiLevel) {
                case GameAILevel::EASY:
                    return "Max Injected Error (Easy)";
                case GameAILevel::MEDIUM:
                    return "Max Injected Error (Medium)";
                case GameAILevel::HARD:
                    return "Max Injected Error (Hard)";
            }
            return "???";
        }

        const char* getDescription() const override {
            switch (_aiLevel) {
                case GameAILevel::EASY:
                    return "Axis maximum AI player injected position error when tracking the ball for Easy level";
                case GameAILevel::MEDIUM:
                    return "Axis maximum AI player injected position error when tracking the ball for Medium level";
                case GameAILevel::HARD:
                    return "Axis maximum AI player injected position error when tracking the ball for Hard level";
            }
            return "???";
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