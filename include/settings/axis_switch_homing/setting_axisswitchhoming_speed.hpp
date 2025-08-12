#pragma once

#include "motor_control\motorwithreferenceswitch.hpp"
#include "settings\setting.hpp"

class AxisSwitchHomingSpeedSetting : public Setting<float> {
    private:
        switch_homing_config_t& _config;

    public:
        AxisSwitchHomingSpeedSetting(switch_homing_config_t& config) : _config(config) {}

        float getValue() const override {
            return _config.speed;
        }

        void setValue(const float value) override {
            _config.speed = value;
        }

        const char* getName() const override {
            return "speed";
        }

        const char* getTitle() const override {
            return "Homing speed";
        }

        const char* getDescription() const override {
            return "Axis speed when moving toward the home switch";
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
            return 1.0;
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
