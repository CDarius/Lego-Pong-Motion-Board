#pragma once

#include "motor_control\motor.hpp"
#include "settings\setting.hpp"

class AxisMaxWindupFactorSetting : public SettingFloat {
    private:
        Motor& _motor;

    public:
        AxisMaxWindupFactorSetting(Motor& motor) : _motor(motor) {}

        float getValue() const override {
            uint16_t kp;
            uint16_t ki;
            uint16_t kd;
            float integral_deadzone;
            float integral_rate;
            float max_windup_factor;

            _motor.get_pid(&kp, &ki, &kd, &integral_deadzone, &integral_rate, &max_windup_factor);
            return max_windup_factor;
        }

        void setValue(const float value) override {
            uint16_t kp;
            uint16_t ki;
            uint16_t kd;
            float integral_deadzone;
            float integral_rate;
            float max_windup_factor;

            _motor.get_pid(&kp, &ki, &kd, &integral_deadzone, &integral_rate, &max_windup_factor);
            max_windup_factor = value;
            _motor.set_pid(kp, ki, kd, integral_deadzone, integral_rate, max_windup_factor);
        }

        const char* getName() const override {
            return "max_windup_factor";
        }

        const char* getTitle() const override {
            return "Max Windup Factor";
        }

        const char* getDescription() const override {
            return "Factor to limit/increase the integrator max windup. 1 means default windup. A value < 1 reduces windup, while a value > 1 increases it.";
        }

        const char* getUnit() const override {
            return "";
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