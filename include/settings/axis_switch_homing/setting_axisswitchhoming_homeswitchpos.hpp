#pragma once

#include "motor_control\motorwithreferenceswitch.hpp"
#include "settings\setting.hpp"

class AxisSwitchHomingHomeSwitchPosSetting : public Setting<float> {
    private:
        switch_homing_config_t& _config;

    public:
        AxisSwitchHomingHomeSwitchPosSetting(switch_homing_config_t& config) : _config(config) {}

        float getValue() const override {
            return _config.axis_position_at_home_marker;
        }

        void setValue(const float value) override {
            _config.axis_position_at_home_marker = value;
        }

        const char* getName() const override {
            return "home_switch_pos";
        }

        const char* getTitle() const override {
            return "Home switch position";
        }

        const char* getDescription() const override {
            return "Axis position when hit the home switch";
        }

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
            return -5.0;
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
            return 0.1;
        }
    };