#ifndef __GAME_SETTING_HPP__
#define __GAME_SETTING_HPP__

#include <Arduino.h>
#include "utils/logger.hpp"

enum class SettingType {
    FloatType,
    UInt8,
};

class ISetting {
public:
    virtual const char* getName() const;
    virtual const char* getTitle() const;
    virtual const char* getDescription() const;
    virtual const char* getUnit() const;
    virtual SettingType getType() const;

    virtual const bool hasMinValue() const;
    virtual const bool hasMaxValue() const;
    virtual const bool hasChangeStep() const;
};

template <typename T>
class Setting : public ISetting {
public:
    virtual T getValue() const;
    virtual void setValue(const T value);

    virtual const T getMinValue() const;
    virtual const T getMaxValue() const;
    virtual const T getChangeStep() const;
};

class SettingUInt8 : public Setting<uint8_t> {
public:
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

#endif