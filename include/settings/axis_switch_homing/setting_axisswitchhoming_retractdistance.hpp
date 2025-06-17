#ifndef __SETTING_AXISSWITCHHOMING_RETRACT_DISTANCE_HPP__
#define __SETTING_AXISSWITCHHOMING_RETRACT_DISTANCE_HPP__

#include "motor_control\motorhoming.hpp"
#include "settings\setting.hpp"

class AxisSwitchHomingRetractDistanceSetting : public Setting<float> {
    private:
        switch_homing_config_t& _config;

    public:
        AxisSwitchHomingRetractDistanceSetting(switch_homing_config_t& config) : _config(config) {}

        float getValue() const override {
            return _config.retract_distance;
        }

        void setValue(const float value) override {
            _config.retract_distance = value;
        }

        const char* getName() const override {
            return "retract_distance";
        }

        const char* getTitle() const override {
            return "Retract distance";
        }

        const char* getDescription() const override {
            return "Retract distance after hitting the home switch";
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
            return 0.0;
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
            return 1.0;
        }
    };

#endif