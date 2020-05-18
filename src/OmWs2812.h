/*
 * 2020-05-17 Plague Time
 * A SPI-based implementation of WS2812 driver.
 * It's for ESP8266 and must be on the SPI data
 * pin (D7 for a Wemod D1). It also ends up using
 * the clock pin of course but oh well.
 */

#ifndef __OmWs2812_h__
#define __OmWs2812_h__

#include "OmLed16.h"
#include "FastLED.h" // we use CRGB as one of our formats.

#ifndef DIGITAL_WRITE
#define DIGITAL_WRITE(_pin, _value) digitalWrite(_pin, _value)
#endif


class OmWs2812Writer
{
public:
    void showLeds(CRGB *leds, int ledCount);
};

#endif
