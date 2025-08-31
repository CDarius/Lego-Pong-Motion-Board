/*!
 * @brief A rotary encoder expansion From M5Stack
 * @copyright Copyright (c) 2022 by M5Stack[https://m5stack.com]
 *
 * @Links [Unit Encoder](https://docs.m5stack.com/en/unit/encoder)
 * @version  V0.0.2
 * @date  2022-07-11
 */
#ifndef _UNIT_ENCODER_H_
#define _UNIT_ENCODER_H_

#include "Arduino.h"
#include "Wire.h"

#define UNIT_ENC_ENCODER_ADDR   0x40

#define UNIT_ENC_ENCODER_REG    0x10
#define UNIT_ENC_BUTTON_REG     0x20
#define UNIT_ENC_RGB_LED_REG    0x30
#define UNIT_ENC_RESET_REG      0X40

class UnitEncoder {
   private:
    uint8_t _addr;
    TwoWire* _wire;
    void writeBytes(uint8_t addr, uint8_t reg, uint8_t* buffer, uint8_t length);
    void readBytes(uint8_t addr, uint8_t reg, uint8_t* buffer, uint8_t length);

   public:
    void begin(TwoWire* wire = &Wire, uint8_t addr = UNIT_ENC_ENCODER_ADDR);
    int16_t getValue();
    void setValue(int16_t value);
    void clearValue();
    bool isButtonPressed();
    void setLEDColor(uint8_t index, uint32_t color);
};

#endif