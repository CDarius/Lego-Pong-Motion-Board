#pragma once

#include "settings/setting.hpp"
#include "game/game_settings.hpp"
#include "game/game.hpp"

class AIPlayerMaxSpeedSetting : public SettingFloat {
    private:
        GameAIPlayerSettings& _settings;
        GameAILevel _aiLevel;

    public:
        AIPlayerMaxSpeedSetting(GameAIPlayerSettings& settings, GameAILevel aiLevel) : _settings(settings), _aiLevel(aiLevel) {}

        float getValue() const override {
            return _settings.paddleMaxSpeed;
        }

        void setValue(const float value) override {
            _settings.paddleMaxSpeed = value;
        }

        const char* getName() const override {
            switch (_aiLevel) {
                case GameAILevel::EASY:
                    return "max_speed_e";
                case GameAILevel::MEDIUM:
                    return "max_speed_m";
                case GameAILevel::HARD:
                    return "max_speed_h";
            }
            return "???";
        }

        const char* getTitle() const override {
            switch (_aiLevel) {
                case GameAILevel::EASY:
                    return "Maximum Speed (Easy)";
                case GameAILevel::MEDIUM:
                    return "Maximum Speed (Medium)";
                case GameAILevel::HARD:
                    return "Maximum Speed (Hard)";
            }
            return "???";
        }

        const char* getDescription() const override {
            switch (_aiLevel) {
                case GameAILevel::EASY:
                    return "Axis maximum close speed for AI player (Easy)";
                case GameAILevel::MEDIUM:
                    return "Axis maximum close speed for AI player (Medium)";
                case GameAILevel::HARD:
                    return "Axis maximum close speed for AI player (Hard)";
            }
            return "???";
        }

        const char* getUnit() const override {
            return "stud/s";
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