#pragma once

#include <Arduino.h>
#include <Wire.h>
#include "logger.hpp"

/**
 * @brief Scan for I2C devices on the specified TwoWire instance and log their addresses.
 * 
 * This function iterates through all possible I2C addresses (1 to 126) and checks if a device responds.
 * If a device is found, its address is logged. If no devices are found, it logs that no devices were found.
 * 
 * @param wire Pointer to the TwoWire instance (e.g., &Wire).
 */
void scanI2CDevices(TwoWire* wire);

/**
 * @brief Test if an I2C device exists at the specified address. If the device does not exist, it logs an error message.
 * 
 * @param wire Pointer to the TwoWire instance (e.g., &Wire).
 * @param address The I2C address of the device to check.
 * @param device_name A string representing the name of the device for logging purposes.
 * @return true if the device exists, false otherwise.
 */
bool testI2CDeviceExist(TwoWire* wire, uint8_t address, const char* device_name);
