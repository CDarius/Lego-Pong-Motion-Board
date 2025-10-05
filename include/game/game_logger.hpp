#pragma once

#include <Arduino.h>
#include "esp_heap_caps.h"

struct GameLogBaseEntry {
    uint16_t cycle;
    uint16_t subCycle;
};

struct GameLogFirstEntry {
    uint16_t cycle;
    uint16_t subCycle;
    float ballX;
    float ballY;
    float paddleL;
    float paddleR;
    uint32_t timestampMs;
    uint32_t reserved;
};

struct GameLogSubEntry {
    uint16_t cycle;
    uint16_t subCycle;
    float targetX;
    float targetY;
    float targetPaddleL;
    float targetPaddleR;
    float speedX;
    float speedY;
};

union GameLogEntry {
    GameLogBaseEntry baseEntry;
    GameLogFirstEntry firstEntry;
    GameLogSubEntry subEntry;
};

class GameLogger {
    private:
        GameLogEntry* _buffer = nullptr;
        uint32_t _bufferSize = 0;
        uint32_t _cycle = 0;
        uint32_t _subCycle = 0;
        uint32_t _startTimeMs = 0;
        bool _isLogging = false;

    uint32_t _writeIndex = 0;
    uint32_t _entryCount = 0;

    private:
        /*
        * Add a new entry to the log
        */
        void incrementWriteIndex();

    public:
        ~GameLogger() {
            deleteLog();
        }

        /*
        * Start logging with a maximum number of entries.
        * maxEntries: Maximum number of log entries to store
        */
        void start(uint32_t maxEntries);

        /* 
        * Stop logging 
        */
        void stop()  {
            _isLogging = false;
        }

        /*
        * Delete all log entries and release memory
        */
        void deleteLog();

        /*
        * Log a new cycle
        * ballX: Ball X position
        * ballY: Ball Y position
        * paddleL: Left paddle position
        * paddleR: Right paddle position
        */
        void logNewCycle(float ballX, float ballY, float paddleL, float paddleR);

        /*
        * Log a new sub-cycle
        * targetBallX: Target Ball X position
        * targetBallY: Target Ball Y position
        * targetPaddleL: Target Left paddle position
        * targetPaddleR: Target Right paddle position
        */
        void logSubCycle(float targetBallX, float targetBallY, float targetPaddleL, float targetPaddleR, float speedX, float speedY);

        /*
        * Get the current log length
        */
        uint32_t getLogLen() const { return _entryCount; }

        /*
        * Get the current log buffer
        */
        GameLogEntry* getLogEntry(uint32_t index) const {
            if (index >= _entryCount) {
                return nullptr;
            }
            uint32_t realIndex = (_writeIndex + _bufferSize - _entryCount + index) % _bufferSize;
            return &_buffer[realIndex];
        }

        /*
        * Check if logging is active
        */
       bool isLoggingActive() const { return _isLogging; }
};