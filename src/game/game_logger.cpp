#include "game/game_logger.hpp"

void GameLogger::addEntryToLog(float ballX, float ballY, float paddleL, float paddleR) {
    GameLogEntry entry;
    entry.cycle = _cycle;
    entry.subCycle = _subCycle;
    entry.ballX = ballX;
    entry.ballY = ballY;
    entry.paddleL = paddleL;
    entry.paddleR = paddleR;

    _buffer[_writeIndex] = entry;
    _writeIndex = (_writeIndex + 1) % _bufferSize;
    if (_entryCount < _bufferSize) {
        _entryCount++;
    }
}

void GameLogger::start(uint32_t maxEntries) {
    deleteLog();
    
    _cycle = 0;
    _subCycle = 0;
    _writeIndex = 0;
    _entryCount = 0;
    _bufferSize = maxEntries;
    _buffer = (GameLogEntry*)heap_caps_malloc(maxEntries * sizeof(GameLogEntry), MALLOC_CAP_SPIRAM);
    _isLogging = true;
}

void GameLogger::deleteLog() {
    if (_buffer != nullptr) {
        free(_buffer);
        _buffer = nullptr;
    }
    _bufferSize = 0;
    _isLogging = false;
    _writeIndex = 0;
    _entryCount = 0;
}
