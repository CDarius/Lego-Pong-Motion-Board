#ifndef __RGB_LED_HPP__
#define __RGB_LED_HPP__

#include <Arduino.h>
#include <FastLED.h>

#define RGB_LED_PIN             48

// Common color constants
#define RGB_COLOR_RED     CRGB::Red
#define RGB_COLOR_GREEN   CRGB::Green
#define RGB_COLOR_BLUE    CRGB::Blue
#define RGB_COLOR_WHITE   CRGB::White
#define RGB_COLOR_YELLOW  CRGB::Yellow
#define RGB_COLOR_CYAN    CRGB::Cyan
#define RGB_COLOR_MAGENTA CRGB::Magenta
#define RGB_COLOR_BLACK   CRGB::Black

class RGBLed {
public:
    void begin();
    void setColor(CRGB color);

    void unrecoverableError();

private:
    CRGB leds[1];
};

#endif