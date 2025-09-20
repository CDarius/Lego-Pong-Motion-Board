#pragma once

#include "motor_control\motor.hpp"
#include "settings\setting.hpp"

class AxisPosToleranceSetting : public Setting<float> {
    private:
        Motor& _motor;

    public:
        AxisPosToleranceSetting(Motor& motor) : _motor(motor) {}

        float getValue() const override {
            float speed,position;
            _motor.get_target_tolerances(&speed, &position);
            return position;
        }

        void setValue(const float value) override {
            float speed,position;
            _motor.get_target_tolerances(&speed, &position);
            position = value;
            _motor.set_target_tolerances(speed, position);
        }

        const char* getName() const override {
            return "pos_tolerance";
        }

        const char* getTitle() const override {
            return "Position Tolerance";
        }

        const char* getDescription() const override {
            return "Maximum position deviation tolerance at the end of a close loop movement";
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
            return 0.1;
        }

        const bool hasMaxValue() const override {
            return true;
        }

        const float getMaxValue() const override {
            return 10.0;
        }

        const bool hasChangeStep() const override {
            return true;
        }

        const float getChangeStep() const override {
            return 0.1;
        }
    };