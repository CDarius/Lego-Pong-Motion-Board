#pragma once

#include "motor_control\motorhoming.hpp"
#include "settings\setting.hpp"

class AxisSwitchHomingPosAfterHomeSetting : public SettingFloat {
    private:
        base_homing_config_t& _config;

    public:
        AxisSwitchHomingPosAfterHomeSetting(base_homing_config_t& config) : _config(config) {}

        float getValue() const override {
            return _config.axis_position_after_home;
        }

        void setValue(const float value) override {
            _config.axis_position_after_home = value;
        }

        const char* getName() const override {
            return "pos_after_home";
        }

        const char* getTitle() const override {
            return "Position reached after home";
        }

        const char* getDescription() const override {
            return "Axis position reached after the homing procedure is complete";
        }

        const char* getUnit() const override {
            return "stud";
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
            return 0.5;
        }
    };