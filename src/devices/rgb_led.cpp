#include "devices/rgb_led.hpp"

void RGBLed::begin() {
    FastLED.addLeds<WS2812B, RGB_LED_PIN, GRB>(leds, 1);
    FastLED.setBrightness(100); 
    FastLED.clear();
    FastLED.show();
}

void RGBLed::setColor(CRGB color) {
    leds[0] = CRGB(color);
    FastLED.show();
}

/**
 * @brief Blink the LED in red color to indicate an unrecoverable error.
 * This function will run indefinitely, blinking the LED every 500 milliseconds.
 */
void RGBLed::unrecoverableError() {
    while (true)
    {
        leds[0] = CRGB::Red;
        FastLED.show();
        delay(200);
        leds[0] = CRGB::Black;
        FastLED.show();
        delay(200);
    }
    
}
