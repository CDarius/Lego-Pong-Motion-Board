#ifndef __SETTING_AXIS_HOME_SWITCH_POS_HPP__
#define __SETTING_AXIS_HOME_SWITCH_POS_HPP__

#include "motor_control\motor.hpp"
#include "settings\setting.hpp"

class AxisHomeSwitchPosSetting : public Setting<float> {
    private:
        Motor& _motor;

    public:
        AxisHomeSwitchPosSetting(Motor& motor) : _motor(motor) {}

        float getValue() const override {
            return _motor.getHomeSwitchPos();
        }

        void setValue(const float value) override {
            _motor.setHomeSwithcPos(value);            
        }

        const char* getName() const override {
            return "home_switch_pos";
        }

        const char* getTitle() const override {
            return "Home switch position";
        }

        const char* getDescription() const override {
            return "Axis position at the end oft the homing position";
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