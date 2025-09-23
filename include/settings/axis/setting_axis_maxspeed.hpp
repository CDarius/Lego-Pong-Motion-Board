#pragma once

#include "motor_control\motor.hpp"
#include "settings\setting.hpp"

class AxisMaxSpeedSetting : public SettingFloat {
    private:
        Motor& _motor;

    public:
        AxisMaxSpeedSetting(Motor& motor) : _motor(motor) {}

        float getValue() const override {
            return _motor.get_speed_limit();
        }

        void setValue(const float value) override {
            _motor.set_speed_limit(value);
        }

        const char* getName() const override {
            return "max_speed";
        }

        const char* getTitle() const override {
            return "Maximum speed";
        }

        const char* getDescription() const override {
            return "Axis maximum speed used in close loop commands";
        }

        const char* getUnit() const override {
            return "stud/s";
        }

        const bool hasMinValue() const override {
            return true;
        }

        const float getMinValue() const override {
            return 2;
        }

        const bool hasMaxValue() const override {
            return true;
        }

        const float getMaxValue() const override {
            return 2000.0;
        }

        const bool hasChangeStep() const override {
            return true;
        }

        const float getChangeStep() const override {
            return 1;
        }
    };
