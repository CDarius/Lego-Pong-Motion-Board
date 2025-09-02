#include "utils/logger.hpp"

void Logger::logE(const char* message) {
    String prefix = "LOG:E#";
    Serial.println(prefix + message);

    if (outputFunc) {
        outputFunc(LogLevel::Error, message);
    }
}

void Logger::logE(const String& message) {
    logE(message.c_str());
}

void Logger::logW(const char* message) {
    String prefix = "LOG:W#";
    Serial.println(prefix + message);

    if (outputFunc) {
        outputFunc(LogLevel::Warning, message);
    }
}

void Logger::logW(const String& message) {
    logW(message.c_str());
}

void Logger::logI(const char* message) {
    String prefix = "LOG:I#";
    Serial.println(prefix + message);

    if (outputFunc) {
        outputFunc(LogLevel::Info, message);
    }
}

void Logger::logI(const String& message) {
    logI(message.c_str());
}
