#pragma once

#include "motor_control\motorwithreferenceswitch.hpp"
#include "settings\setting.hpp"

class AxisSwitchHomingMinimumTravelSetting : public SettingFloat {
    private:
        switch_homing_config_t& _config;

    public:
        AxisSwitchHomingMinimumTravelSetting(switch_homing_config_t& config) : _config(config) {}

        float getValue() const override {
            return _config.minimum_travel;
        }

        void setValue(const float value) override {
            _config.minimum_travel = value;
        }

        const char* getName() const override {
            return "minimum_travel";
        }

        const char* getTitle() const override {
            return "Minimum required travel";
        }

        const char* getDescription() const override {
            return "Mimum required travel distance before hitting the switch. If the distance is less than this value, the homing will restart.";
        }

        const char* getUnit() const override {
            return "stud";
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
