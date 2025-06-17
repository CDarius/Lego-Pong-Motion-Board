#ifndef __SETTING_AXIS_SWLIMITM_HPP__
#define __SETTING_AXIS_SWLIMITM_HPP__

#include "motor_control\motor.hpp"
#include "settings\setting.hpp"

class AxisSwLimitMSetting : public Setting<float> {
    private:
        Motor& _motor;

    public:
        AxisSwLimitMSetting(Motor& motor) : _motor(motor) {}

        float getValue() const override {
            return _motor.getSwLimitMinus();
        }

        void setValue(const float value) override {
            _motor.setSwLimitMinus(value);            
        }

        const char* getName() const override {
            return "sw_limit_m";
        }

        const char* getTitle() const override {
            return "Software limit minus";
        }

        const char* getDescription() const override {
            return "Minimum position that the axis can reach in a close loop position command";
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

#endif