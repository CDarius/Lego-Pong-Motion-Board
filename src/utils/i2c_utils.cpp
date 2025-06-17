#include "utils/i2c_utils.hpp"

void scanI2CDevices(TwoWire *wire)
{
    byte error, address;
    bool device_found = false;

    for (address = 1; address < 127; address++)
    {
        // The i2c_scanner uses the return value of
        // the Write.endTransmisstion to see if
        // a device did acknowledge to the address.
        wire->beginTransmission(address);
        error = wire->endTransmission();

        if (error == 0)
        {
            String message = "I2C device found at address 0x" + String(address, HEX);
            Logger::instance().logI(message);
            device_found = true;
        }
        else if (error == 4)
        {
            String warning = "Unknown error at address 0x" + String(address, HEX);
            Logger::instance().logW(warning);
        }
    }
    if (!device_found) {
        Logger::instance().logI("No I2C devices found");
    }
}

bool testI2CDeviceExist(TwoWire* wire, uint8_t address, const char* device_name) {
    wire->beginTransmission(address);
    if (wire->endTransmission() != 0) {
        String error = "I2C device \"" + String(device_name) + "\" (0x" + String(address, HEX) + ") not found!";
        Logger::instance().logE(error);
        return false;
    }

    return true;
}
