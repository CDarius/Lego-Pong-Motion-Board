#pragma once

#include <Arduino.h>
#include "utils/logger.hpp"

#define IO_BOARD_COMMAND_DATA_SEPARATOR "::"
#define IO_BOARD_DATA_VALUES_SEPARATOR "##"

#define IO_BOARD_DATA_TYPE_INIT     "INIT"
#define IO_BOARD_DATA_TYPE_LOG      "LOG"
#define IO_BOARD_DATA_TYPE_AXES_POS "AXES"
#define IO_BOARD_DATA_TYPE_PING     "PING"
#define IO_BOARD_DATA_TYPE_PONG     "PONG"
#define IO_BOARD_DATA_TYPE_SCORE    "SCORE"
#define IO_BOARD_DATA_TYPE_TEXT     "TEXT"
#define IO_BOARD_DATA_TYPE_SOUND    "SOUND"

#define IO_BOARD_SOUND_PADDLE   0x01
#define IO_BOARD_SOUND_WALL     0x02
#define IO_BOARD_SOUND_SCORE    0x03
#define IO_BOARD_SOUND_BEEP     0x04
#define IO_BOARD_SOUND_START    0x05
#define IO_BOARD_SOUND_ALARM    0x06
#define IO_BOARD_SOUND_BEEP2    0x07

class IOBoard {
public:
    IOBoard(HardwareSerial& serial) : _serial(serial) {}

    /*
    Test the connection with the IO board.
    Sends a PING command and waits for a PONG response.
    */
    bool testConnection(unsigned long timeout_ms);

    /*
    Send an INIT command to the IO board.
    If start_web_server is true, the IO board will start the web server.
    */
    void sendInit(bool start_web_server) {
        String data = start_web_server ? "1" : "0";
        sendData(IO_BOARD_DATA_TYPE_INIT, data.c_str());
    }

    /*
    Send a log message to the IO board
    */
    void sendLog(LogLevel level, const char* message);

    /*
    Send the position of the axes to the IO board.
    */
    void sendAxesPosition(float x, float y, float l, float r) {
        String data = String(x) + IO_BOARD_DATA_VALUES_SEPARATOR + String(y) + IO_BOARD_DATA_VALUES_SEPARATOR + String(l) + IO_BOARD_DATA_VALUES_SEPARATOR + String(r);
        sendData(IO_BOARD_DATA_TYPE_AXES_POS, data.c_str());
    }

    /* 
    Show the score on the IO board.
    The score is displayed as "score1:score2
    If animation_delay_ms is greater than 0, the score will be animated with the specified delay.
    If blink_interval_ms is greater than 0, the text will blink with the specified interval.
    */
    void showScore(int score1, int score2, int animation_delay_ms = 0, int blink_interval_ms = 0);

    /*
    Show a text message on the IO board.
    The text will be displayed without scrolling.
    If blink_interval_ms is greater than 0, the text will blink with the specified interval.
    */
    void showText(const char* text, int blink_interval_ms = 0) {
        showScrollingText(text, 0, false, blink_interval_ms);
    }

    /*
    Show a text message on the IO board.
    The text will be displayed without scrolling.
    If blink_interval_ms is greater than 0, the text will blink with the specified interval.
    */
    void showText(String text, int blink_interval_ms = 0) {
        showScrollingText(text.c_str(), 0, false, blink_interval_ms);
    }

    /*
    Clear the text displayed on the IO board.
    */
    void clearText() {
        showScrollingText("", 0, false, 0);
    }

    /*
    Show scrolling text on the IO board.
    The text will scroll with the specified animation delay.
    If repeat is true, the text will scroll indefinitely.
    If blink_interval_ms is greater than 0, the text will blink with the specified interval.
    */
    void showScrollingText(const char* text, int animation_delay_ms, bool repeat, int blink_interval_ms = 0);

    /*
    Show scrolling text on the IO board.
    The text will scroll with the specified animation delay.
    If repeat is true, the text will scroll indefinitely.
    If blink_interval_ms is greater than 0, the text will blink with the specified interval.
    */
    void showScrollingText(String text, int animation_delay_ms, bool repeat, int blink_interval_ms = 0) {
        showScrollingText(text.c_str(), animation_delay_ms, repeat, blink_interval_ms);
    }

    /*
    Play a sound on the IO board.
    sound_id is the ID of the sound to play.
    repeat_count is the number of times to repeat the sound.
    */
    void playSound(uint8_t sound_id, int repeat_count = 1);

private:
    HardwareSerial& _serial;
    SemaphoreHandle_t _xMutex = xSemaphoreCreateMutex();

    /*
    Sends dato to the IO board.
    The data_type and data are separated by IO_BOARD_COMMAND_DATA_SEPARATOR.
    The data is expected to be a String that can contain multiple values separated by IO_BOARD_DATA_VALUES_SEPARATOR.
    For example, to send a data_type with multiple values:
        sendData("COMMAND_NAME", "value1#value2#value3");
    */
    void sendData(const char* data_type, const char* data);

    /*
    Receives data from the IO board.
    The data is expected to be in the format "data_type::data_value".
    The data_type is separated from the data_value by IO_BOARD_COMMAND_DATA_SEPARATOR.
    The data_value may contain multiple values separated by IO_BOARD_DATA_VALUES_SEPARATOR.    
    */
    bool receiveData(String& data_type, String& data);

    /*
    Receives data of a specific type from the IO board.
    This function will block until the expected data_type is received or the timeout is reached.
    */
    bool receiveData(const char* data_type, String& data, unsigned long timeout_ms);
};
