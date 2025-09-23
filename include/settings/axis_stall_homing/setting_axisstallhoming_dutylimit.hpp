#pragma once

#include "motor_control\motorwithstallreference.hpp"
#include "settings\setting.hpp"

class AxisStallHomingDutyLimitSetting : public SettingFloat {
    private:
        stall_homing_config_t& _config;

    public:
        AxisStallHomingDutyLimitSetting(stall_homing_config_t& config) : _config(config) {}

        float getValue() const override {
            return _config.duty_limit;
        }

        void setValue(const float value) override {
            _config.duty_limit = value;
        }

        const char* getName() const override {
            return "duty_limit";
        }

        const char* getTitle() const override {
            return "Stall duty limit";
        }

        const char* getDescription() const override {
            return "Duty cycle limit to detect stall on the reference obstacle";
        }

        const char* getUnit() const override {
            return "%";
        }

        const bool hasMinValue() const override {
            return true;
        }

        const float getMinValue() const override {
            return 10.0;
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
