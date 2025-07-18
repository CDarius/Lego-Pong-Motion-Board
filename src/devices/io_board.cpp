#include "devices/io_board.hpp"

void IOBoard::sendLog(LogLevel level, const char* message) {
    const char* level_str = nullptr;
    switch (level) {
        case LogLevel::Error:
            level_str = "E";
            break;
        case LogLevel::Warning:
            level_str = "W";
            break;
        case LogLevel::Info:
            level_str = "I";
            break;
        default:
            return; // Unknown log level, do nothing
    }
    
    String data = String(level_str) + IO_BOARD_DATA_VALUES_SEPARATOR + message;
    sendData(IO_BOARD_DATA_TYPE_LOG, data.c_str());
}

void IOBoard::showScore(int score1, int score2, int animation_delay_ms, int blink_interval_ms) {
    String data = String(score1) + 
                  IO_BOARD_DATA_VALUES_SEPARATOR + String(score2) + 
                  IO_BOARD_DATA_VALUES_SEPARATOR + (animation_delay_ms > 0 ? "1" : "0") + 
                  IO_BOARD_DATA_VALUES_SEPARATOR + String(animation_delay_ms) +
                  IO_BOARD_DATA_VALUES_SEPARATOR + String(blink_interval_ms);
    sendData(IO_BOARD_DATA_TYPE_SCORE, data.c_str());
}

void IOBoard::showScrollingText(const char* text, int animation_delay_ms, bool repeat, int blink_interval_ms) {
    String data = String(text) + 
                    IO_BOARD_DATA_VALUES_SEPARATOR + (animation_delay_ms > 0 ? "1" : "0") + 
                    IO_BOARD_DATA_VALUES_SEPARATOR + String(animation_delay_ms) + 
                    IO_BOARD_DATA_VALUES_SEPARATOR + (repeat ? "1" : "0") + 
                    IO_BOARD_DATA_VALUES_SEPARATOR + String(blink_interval_ms);
    sendData(IO_BOARD_DATA_TYPE_TEXT, data.c_str());
}

void IOBoard::playSound(uint8_t sound_id, int repeat_count) {
    String data = String(sound_id) +
                    IO_BOARD_DATA_VALUES_SEPARATOR + String(repeat_count);
    sendData(IO_BOARD_DATA_TYPE_SOUND, data.c_str());
}

bool IOBoard::testConnection(unsigned long timeout_ms) {
    String data;
    int retry = timeout_ms / 500;
    for (int i = 0; i < retry; ++i) {
        sendData(IO_BOARD_DATA_TYPE_PING, "");
        if (receiveData(IO_BOARD_DATA_TYPE_PONG, data, 500)) {
            return true; // Successfully received PONG response
        }
    }
    return false; // Timeout reached without receiving PONG response
}

void IOBoard::sendData(const char* data_type, const char* data) {
    if (xSemaphoreTake(_xMutex, portMAX_DELAY)) {
        _serial.print(data_type);
        _serial.print(IO_BOARD_COMMAND_DATA_SEPARATOR);
        _serial.println(data);

        xSemaphoreGive(_xMutex);
    }
}

bool IOBoard::receiveData(String& data_type, String& data) {
    if (_serial.available()) {
        String line = _serial.readStringUntil('\n');
        int sepIndex = line.indexOf(IO_BOARD_COMMAND_DATA_SEPARATOR);
        if (sepIndex > 0) {
            data_type = line.substring(0, sepIndex);
            data = line.substring(sepIndex + 2);
            data.trim(); // Remove trailing \r or spaces
            return true;
        }
    }
    return false;
}

bool IOBoard::receiveData(const char* data_type, String& data, unsigned long timeout_ms) {
    String recDataType;

    unsigned long startTime = millis();
    while (millis() - startTime < timeout_ms) {
        if (receiveData(recDataType, data)) {
            if (recDataType.equals(data_type)) {
                return true; // Received the expected data type
            }
        }
        delay(10); // Avoid busy waiting
    }
    return false; // Timeout reached without receiving data
}
