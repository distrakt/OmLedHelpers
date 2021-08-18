#include "FastLED.h"

#ifndef _OMLEDS_H_
#define _OMLEDS_H_

// for desktop test builds, preprocessor defines these.
// for arduino, here's where they arrive.
#ifndef DIGITAL_WRITE
#define DIGITAL_WRITE(_pin, _value) digitalWrite(_pin, _value)
#define PIN_MODE(_pin, _mode) pinMode(_pin, _mode)
#endif

#include "OmLedT.h"
#include "OmLedTStrip.h"
#include "OmLedTPatterns.h"
#include "OmLedTPatternManager.hpp"

#include "OmSk9822.h"
#include "OmWs2812.h"
#include "OmLedUtils.h"

class OmLeds
{
public:
    CRGB *leds = 0;
    int ledCount = 0;
    bool isRing = true;
    
    OmLeds(CRGB *leds, int ledCount, bool isRing = true);
    
    void clear();
    void clear1(int ix);
    void setI(CRGB co, int x);
    void set(CRGB co, float x);
    void set(CRGB co, float x0, float x1);
    void show();
};
 class OmLeds2d
{
public:
        OmLeds *leds;
    int w;
    int h;
    OmLeds2d(OmLeds *leds, int width);
    void setI(CRGB co, int x, int y);
    void set(CRGB co, float x, float y);
    void set(CRGB co, float x0, float y0, float x1, float y1);
};

CRGB operator *(CRGB &co, float n);
CRGB operator *(float n, CRGB &co);


#endif // _OMLEDS_H_
