#pragma once

#include "motor_control\motorhoming.hpp"
#include "settings\setting.hpp"

class AxisStallHomingHomeObstaclePosSetting : public SettingFloat {
    private:
        base_homing_config_t& _config;

    public:
        AxisStallHomingHomeObstaclePosSetting(base_homing_config_t& motorHoming) : _config(motorHoming) {}

        float getValue() const override {
            return _config.axis_position_at_home_marker;
        }

        void setValue(const float value) override {
            _config.axis_position_at_home_marker = value;
        }

        const char* getName() const override {
            return "obstacle_pos";
        }

        const char* getTitle() const override {
            return "Home obstacle position";
        }

        const char* getDescription() const override {
            return "Axis position when hit the reference obstacle";
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
            return 0.1;
        }
    };