#pragma once

#include "settings/setting.hpp"
#include "game/game_settings.hpp"
#include "config.h"

class AIPlayerUpdateTimeSetting : public SettingUInt16 {
    private:
        GameAIPlayerSettings& _settings;

    public:
        AIPlayerUpdateTimeSetting(GameAIPlayerSettings& settings) : _settings(settings) {}

        uint16_t getValue() const override {
            return _settings.playerUpdateTimeMs;
        }

        void setValue(const uint16_t value) override {
            _settings.playerUpdateTimeMs = value;
        }

        const char* getName() const override {
            return "update_time";
        }

        const char* getTitle() const override {
            return "Update time";
        }

        const char* getDescription() const override {
            return "AI player position update time in milliseconds";
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