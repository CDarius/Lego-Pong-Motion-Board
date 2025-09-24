#pragma once

#include "motor_control\motor.hpp"
#include "settings\setting.hpp"

class AxisPidKpSetting : public SettingUInt16 {
    private:
        Motor& _motor;

    public:
        AxisPidKpSetting(Motor& motor) : _motor(motor) {}

        uint16_t getValue() const override {
            uint16_t kp;
            uint16_t ki;
            uint16_t kd;
            float integral_deadzone;
            float integral_rate;

            _motor.get_pid(&kp, &ki, &kd, &integral_deadzone, &integral_rate);
            return kp;
        }

        void setValue(const uint16_t value) override {
            uint16_t kp;
            uint16_t ki;
            uint16_t kd;
            float integral_deadzone;
            float integral_rate;

            _motor.get_pid(&kp, &ki, &kd, &integral_deadzone, &integral_rate);
            kp = value;
            _motor.set_pid(kp, ki, kd, integral_deadzone, integral_rate);
        }

        const char* getName() const override {
            return "pid_kp";
        }

        const char* getTitle() const override {
            return "PID Kp";
        }

        const char* getDescription() const override {
            return "Proportional gain for the PID controller";
        }

        const char* getUnit() const override {
            return "";
        }

        const uint16_t getMinValue() const override {
            return 0;
        }

        const uint16_t getMaxValue() const override {
            return 2000;
        }

        const uint16_t getChangeStep() const override {
            return 10;
        }
    };
