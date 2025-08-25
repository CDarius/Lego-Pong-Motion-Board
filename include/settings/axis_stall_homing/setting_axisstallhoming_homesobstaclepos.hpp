#pragma once

#include "motor_control\motorhoming.hpp"
#include "settings\setting.hpp"

class AxisStallHomingHomeObstaclePosSetting : public Setting<float> {
    private:
        IMotorHoming& _motorHoming;

    public:
        AxisStallHomingHomeObstaclePosSetting(IMotorHoming& motorHoming) : _motorHoming(motorHoming) {}

        float getValue() const override {
            return _motorHoming.axis_position_at_home_marker;
        }

        void setValue(const float value) override {
            _motorHoming.axis_position_at_home_marker = value;
        }

        const char* getName() const override {
            return "obstacle_pos";
        }

        const char* getTitle() const override {
            return "Home obstacle position";
        }

        const char* getDescription() const override {
            return "Axis position when hit the home obstacle";
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