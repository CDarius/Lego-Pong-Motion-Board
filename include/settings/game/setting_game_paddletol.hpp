#pragma once

#include "settings\setting.hpp"

class GamePaddleCollisionToleranceSetting : public Setting<float> {
    private:
        float& _tolerance;

    public:
        GamePaddleCollisionToleranceSetting(float& tolerance) : _tolerance(tolerance) {}

        float getValue() const override {
            return _tolerance;
        }

        void setValue(const float value) override {
            _tolerance = value;
        }

        const char* getName() const override {
            return "paddle_col_tol";
        }

        const char* getTitle() const override {
            return "Paddle Collision Tolerance";
        }

        const char* getDescription() const override {
            return "Ball-paddle collision tolerance";
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
            return 0.0f;
        }

        const bool hasMaxValue() const override {
            return true;
        }

        const float getMaxValue() const override {
            return 5.0f;
        }

        const bool hasChangeStep() const override {
            return true;
        }

        const float getChangeStep() const override {
            return 0.1f;
        }
    };