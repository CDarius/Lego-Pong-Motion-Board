#pragma once

#include "motor_control\motor.hpp"
#include "settings\setting.hpp"

class AxisIntegralRangeSetting : public SettingFloat {
    private:
        Motor& _motor;

    public:
        AxisIntegralRangeSetting(Motor& motor) : _motor(motor) {}

        float getValue() const override {
            uint16_t kp;
            uint16_t ki;
            uint16_t kd;
            float integral_deadzone;
            float integral_rate;
            float max_windup_factor;

            _motor.get_pid(&kp, &ki, &kd, &integral_deadzone, &integral_rate, &max_windup_factor);
            return integral_deadzone;
        }

        void setValue(const float value) override {
            uint16_t kp;
            uint16_t ki;
            uint16_t kd;
            float integral_deadzone;
            float integral_rate;
            float max_windup_factor;

            _motor.get_pid(&kp, &ki, &kd, &integral_deadzone, &integral_rate, &max_windup_factor);
            integral_deadzone = value;
            _motor.set_pid(kp, ki, kd, integral_deadzone, integral_rate, max_windup_factor);
        }

        const char* getName() const override {
            return "intgral_rng";
        }

        const char* getTitle() const override {
            return "Integral Range";
        }

        const char* getDescription() const override {
            return "When distance from target is below this value, the integrator can accumulate error";
        }

        const char* getUnit() const override {
            return "stud";
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