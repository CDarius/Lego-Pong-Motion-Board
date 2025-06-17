#include "utils/logger.hpp"

void Logger::logE(const char* message) {
    const char prefix[] = "LOG:E#";
    size_t prefixLen = sizeof(prefix) - 1; // exclude null terminator
    size_t msgLen = strlen(message);
    size_t bufLen = prefixLen + msgLen;

    uint8_t* buffer = new uint8_t[bufLen];
    memcpy(buffer, prefix, prefixLen);
    memcpy(buffer + prefixLen, message, msgLen);

    Serial.write(buffer, bufLen);

    delete[] buffer;
}

void Logger::logE(const String& message) {
    logE(message.c_str());
}

void Logger::logW(const char* message) {
    const char prefix[] = "LOG:W#";
    size_t prefixLen = sizeof(prefix) - 1; // exclude null terminator
    size_t msgLen = strlen(message);
    size_t bufLen = prefixLen + msgLen;

    uint8_t* buffer = new uint8_t[bufLen];
    memcpy(buffer, prefix, prefixLen);
    memcpy(buffer + prefixLen, message, msgLen);

    Serial.write(buffer, bufLen);
    Serial.write('\n');

    delete[] buffer;
}

void Logger::logW(const String& message) {
    logW(message.c_str());
}

void Logger::logI(const char* message) {
    const char prefix[] = "LOG:I#";
    size_t prefixLen = sizeof(prefix) - 1; // exclude null terminator
    size_t msgLen = strlen(message);
    size_t bufLen = prefixLen + msgLen;

    uint8_t* buffer = new uint8_t[bufLen];
    memcpy(buffer, prefix, prefixLen);
    memcpy(buffer + prefixLen, message, msgLen);

    Serial.write(buffer, bufLen);
    Serial.write('\n');

    delete[] buffer;
}

void Logger::logI(const String& message) {
    logI(message.c_str());
}
