#pragma once

#include "settings/setting.hpp"
#include "game/game_settings.hpp"
#include "game/game_modes.hpp"
#include "config.h"

class AIPlayerErrorUpdateTimeSetting : public SettingUInt16 {
    private:
        GameAIPlayerSettings& _settings;
        GameAILevel _aiLevel;

    public:
        AIPlayerErrorUpdateTimeSetting(GameAIPlayerSettings& settings, GameAILevel aiLevel) : _settings(settings), _aiLevel(aiLevel) {}

        uint16_t getValue() const override {
            return _settings.errorUpdateTimeMs;
        }

        void setValue(const uint16_t value) override {
            _settings.errorUpdateTimeMs = value;
        }

        const char* getName() const override {
            switch (_aiLevel) {
                case GameAILevel::EASY:
                    return "err_upd_time_e";
                case GameAILevel::MEDIUM:
                    return "err_upd_time_m";
                case GameAILevel::HARD:
                    return "err_upd_time_h";
            }
            return "???";
        }

        const char* getTitle() const override {
            switch (_aiLevel) {
                case GameAILevel::EASY:
                    return "AI Error Update Time (Easy)";
                case GameAILevel::MEDIUM:
                    return "AI Error Update Time (Medium)";
                case GameAILevel::HARD:
                    return "AI Error Update Time (Hard)";
            }
            return "???";
        }

        const char* getDescription() const override {
            switch (_aiLevel) {
                case GameAILevel::EASY:
                    return "AI player injected error position update time in milliseconds for Easy level";
                case GameAILevel::MEDIUM:
                    return "AI player injected error position update time in milliseconds for Medium level";
                case GameAILevel::HARD:
                    return "AI player injected error position update time in milliseconds for Hard level";
            }
            return "???";
        }

        const char* getUnit() const override {
            return "ms";
        }

        const uint16_t getMinValue() const override {
            return PBIO_CONFIG_SERVO_PERIOD_MS;
        }

        const uint16_t getMaxValue() const override {
            return 2000;
        }
    };