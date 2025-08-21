#ifndef __SETTING_GAME_BALLSTARTXSPEED_HPP__
#define __SETTING_GAME_BALLSTARTXSPEED_HPP__

#include "game\game_settings.hpp"
#include "settings\setting.hpp"

class GameXBallStartSpeedSetting : public SettingUInt8 {
    private:
        GameSettings& _gameSettings;

    public:
        GameXBallStartSpeedSetting(GameSettings& gameSettings) : _gameSettings(gameSettings) {}

        uint8_t getValue() const override {
            return _gameSettings.xAxis.startBallGameSpeed;
        }

        void setValue(const uint8_t value) override {
            _gameSettings.xAxis.startBallGameSpeed = value;
        }

        const char* getName() const override {
            return "ball_x_start_speed";
        }

        const char* getTitle() const override {
            return "Ball start speed on X-Axis";
        }

        const char* getDescription() const override {
            return "Ball X-Axis speed when a new game start or after a goal";
        }

        const char* getUnit() const override {
            return "stud/s";
        }

        const uint8_t getMinValue() const override {
            return 10;
        }

        const uint8_t getMaxValue() const override {
            return 100;
        }
    };

#endif