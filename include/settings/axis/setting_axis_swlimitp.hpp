#pragma once

#include "motor_control\motor.hpp"
#include "settings\setting.hpp"

class AxisSwLimitPSetting : public SettingFloat {
    private:
        Motor& _motor;

    public:
        AxisSwLimitPSetting(Motor& motor) : _motor(motor) {}

        float getValue() const override {
            return _motor.getSwLimitPlus();
        }

        void setValue(const float value) override {
            _motor.setSwLimitPlus(value);            
        }

        const char* getName() const override {
            return "sw_limit_p";
        }

        const char* getTitle() const override {
            return "Software limit plus";
        }

        const char* getDescription() const override {
            return "Maximum position that the axis can reach in a close loop position command";
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
