#pragma once

#include "settings\setting.hpp"

class GameMinSpeedSetting : public SettingFloat {
    private:
        float& _speed;

    public:
        GameMinSpeedSetting(float& speed) : _speed(speed) {}

        float getValue() const override {
            return _speed;
        }

        void setValue(const float value) override {
            _speed = value;
        }

        const char* getName() const override {
            return "min_speed";
        }

        const char* getTitle() const override {
            return "Minimum speed";
        }

        const char* getDescription() const override {
            return "Axis minimum speed used in close loop commands";
        }

        const char* getUnit() const override {
            return "stud/s";
        }

        const bool hasMinValue() const override {
            return true;
        }

        const float getMinValue() const override {
            return 1.0;
        }

        const bool hasMaxValue() const override {
            return true;
        }

        const float getMaxValue() const override {
            return 20.0;
        }

        const bool hasChangeStep() const override {
            return true;
        }

        const float getChangeStep() const override {
            return 1;
        }
    };