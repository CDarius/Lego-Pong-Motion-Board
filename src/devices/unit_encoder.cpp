#include "devices/unit_encoder.hpp"

/*! @brief Initialize the Encoder. */
void UnitEncoder::begin(TwoWire *wire, uint8_t addr) {
    _wire  = wire;
    _addr  = addr;
}

/*! @brief Write a certain length of data to the specified register address. */
bool UnitEncoder::writeBytes(uint8_t addr, uint8_t reg, uint8_t *buffer,
                              uint8_t length) {
    _wire->beginTransmission(addr);
    _wire->write(reg);
    for (int i = 0; i < length; i++) {
        _wire->write(*(buffer + i));
    }
    uint8_t write_result = _wire->endTransmission();
    return write_result == 0;
}

/*! @brief Read a certain length of data to the specified register address. */
bool UnitEncoder::readBytes(uint8_t addr, uint8_t reg, uint8_t *buffer,
                             uint8_t length) {
    uint8_t index = 0;
    _wire->beginTransmission(addr);
    _wire->write(reg);
    uint8_t write_result =_wire->endTransmission();
    size_t bytes_read = _wire->requestFrom(addr, length);
    for (int i = 0; i < length; i++) {
        buffer[index++] = _wire->read();
    }

    // Return true if the correct number of bytes were read and no error occurred during write
    return (bytes_read == length) && (write_result == 0);
}

/*! @brief Read the encoder value.
    @return The value of the encoder that was read */
bool UnitEncoder::getValue(int16_t &value) {
    uint8_t data[2];
    readBytes(_addr, UNIT_ENC_ENCODER_REG, data, 2);
    value = (int16_t)((data[0]) | (data[1]) << 8);
    return value;
}

/*! @brief Read the encoder value.
    @return The value of the encoder that was read */
bool UnitEncoder::setValue(int16_t value) {
    uint8_t data[2];
    data[0] = value & 0xff;
    data[1] = (value >> 8) & 0xff;
    return writeBytes(_addr, UNIT_ENC_ENCODER_REG, data, 2);
}

/*! @brief Clear the encoder value
    @return The value of the encoder that was read */
bool UnitEncoder::clearValue() {
    uint8_t data[1] = {1};
    return writeBytes(_addr, UNIT_ENC_RESET_REG, data, 1);
}

/*! @brief Get the current status of the rotary encoder keys.
    @return True if the button is pressed, false otherwise */
bool UnitEncoder::isButtonPressed(bool &isPressed) {
    uint8_t data;
    bool success = readBytes(_addr, UNIT_ENC_BUTTON_REG, &data, 1);
    if (success)
        isPressed = data == 0;
    else
        isPressed = false;
    return success;
}

/*! @brief Set the color of the LED (HEX). */
bool UnitEncoder::setLEDColor(uint8_t index, uint32_t color) {
    uint8_t data[4];
    data[3] = color & 0xff;
    data[2] = (color >> 8) & 0xff;
    data[1] = (color >> 16) & 0xff;
    data[0] = index;
    return writeBytes(_addr, UNIT_ENC_RGB_LED_REG, data, 4);
}