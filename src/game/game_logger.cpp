#include "game/game_logger.hpp"

void GameLogger::incrementWriteIndex() {
    _writeIndex = (_writeIndex + 1) % _bufferSize;
    if (_entryCount < _bufferSize) {
        _entryCount++;
    }
}

void GameLogger::start(uint32_t maxEntries) {
    deleteLog();
    
    _cycle = -1;
    _subCycle = 0;
    _writeIndex = 0;
    _entryCount = 0;
    _bufferSize = maxEntries;
    _buffer = (GameLogEntry*)heap_caps_malloc(maxEntries * sizeof(GameLogEntry), MALLOC_CAP_SPIRAM);
    _startTimeMs = millis();
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

void GameLogger::logNewCycle(float ballX, float ballY, float paddleL, float paddleR) {
    if (!_isLogging)
        return;
    
    _cycle++;
    _subCycle = 0;

    GameLogFirstEntry entry;
    entry.cycle = _cycle;
    entry.subCycle = _subCycle;
    entry.ballX = ballX;
    entry.ballY = ballY;
    entry.paddleL = paddleL;
    entry.paddleR = paddleR;
    entry.timestampMs = millis() - _startTimeMs;

    _buffer[_writeIndex].firstEntry = entry;
    incrementWriteIndex();
}

void GameLogger::logSubCycle(float targetBallX, float targetBallY, float targetPaddleL, float targetPaddleR, float speedX, float speedY)  {
    if (!_isLogging)
        return;
    
    _subCycle++;

    GameLogSubEntry entry;
    entry.cycle = _cycle;
    entry.subCycle = _subCycle;
    entry.targetX = targetBallX;
    entry.targetY = targetBallY;
    entry.targetPaddleL = targetPaddleL;
    entry.targetPaddleR = targetPaddleR;
    entry.speedX = speedX;
    entry.speedY = speedY;

    _buffer[_writeIndex].subEntry = entry;
    incrementWriteIndex();            
}
