#pragma once

#include <Arduino.h>
#include "utils/logger.hpp"

enum class SettingType {
    Float,
    UInt8,
    UInt16,
};

class ISetting {
public:
    virtual const char* getName() const = 0;
    virtual const char* getTitle() const = 0;
    virtual const char* getDescription() const = 0;
    virtual const char* getUnit() const = 0;
    virtual SettingType getType() const = 0;

    virtual const bool hasMinValue() const = 0;
    virtual const bool hasMaxValue() const = 0;
    virtual const bool hasChangeStep() const = 0;
};

template <typename T>
class Setting : public ISetting {
public:
    virtual T getValue() const = 0;
    virtual void setValue(const T value) = 0;

    virtual const T getMinValue() const = 0;
    virtual const T getMaxValue() const = 0;
    virtual const T getChangeStep() const = 0;
};

class SettingFloat : public Setting<float> {
public:
    SettingType getType() const {
        return SettingType::Float;
    }
};

class SettingUInt8 : public Setting<uint8_t> {
public:
    SettingType getType() const {
        return SettingType::UInt8;
    }

    const bool hasMinValue() const override {
        return true;
    }

    const bool hasMaxValue() const override {
        return true;
    }

    const bool hasChangeStep() const override {
        return true;
    }

    const uint8_t getMinValue() const override {
        return 0;
    }

    const uint8_t getMaxValue() const override {
        return 255;
    }
    
    const uint8_t getChangeStep() const override {
        return 1;
    }
};

class SettingUInt16 : public Setting<uint16_t> {
public:
    SettingType getType() const {
        return SettingType::UInt16;
    }

    const bool hasMinValue() const override {
        return true;
    }

    const bool hasMaxValue() const override {
        return true;
    }

    const bool hasChangeStep() const override {
        return true;
    }

    const uint16_t getMinValue() const override {
        return 0;
    }

    const uint16_t getMaxValue() const override {
        return 65535;
    }

    const uint16_t getChangeStep() const override {
        return 1;
    }
};
