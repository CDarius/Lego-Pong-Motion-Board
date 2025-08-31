#pragma once

#include "game\game_settings.hpp"
#include "settings\setting.hpp"

class GameXBallBounceInversionOvershootSetting : public Setting<float> {
    private:
        GameXAxisSettings& _settings;
        uint8_t _index;

    public:
        GameXBallBounceInversionOvershootSetting(GameXAxisSettings& settings, uint8_t index) : _settings(settings), _index(index) {}

        float getValue() const override {
            return _settings.bounceInversionOvershootAtSpeed[_index];
        }

        void setValue(const float value) override {
            _settings.bounceInversionOvershootAtSpeed[_index] = value;
        }

        const char* getName() const override;

        const char* getTitle() const override;

        const char* getDescription() const override;

        const char* getUnit() const override {
            return "stud";
        }

        SettingType getType() const {
            return SettingType::FloatType;
        }

        const bool hasMinValue() const override {
            return true;
        }

        const float getMinValue() const override {
            return 0;
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
            return 0.1;
        }
};
