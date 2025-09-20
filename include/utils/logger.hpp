#pragma once

#include <Arduino.h>
#include <functional>

enum class LogLevel
{
    Error,
    Warning,
    Info
};

class Logger {
public:
    // Get the singleton instance
    static Logger& instance() {
        static Logger logger_instance;
        return logger_instance;
    }

    // Delete copy and move constructors and assignment operators
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;
    Logger(Logger&&) = delete;
    Logger& operator=(Logger&&) = delete;

    void logE(const char* message);
    void logE(const String& message);

    void logW(const char* message);
    void logW(const String& message);

    void logI(const char* message);
    void logI(const String& message);

    void setOutput(std::function<void(LogLevel, const char*)> func) {
        outputFunc = func;
    }

private:
    // Private constructor to prevent instantiation
    Logger() = default;
    ~Logger() = default;

    std::function<void(LogLevel, const char*)> outputFunc;
};
