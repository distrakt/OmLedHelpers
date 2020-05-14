#include "FastLED.h"

#ifndef _OMLEDS_H_
#define _OMLEDS_H_

#include "OmLed16.h"
#include "OmLed16Patterns.h"
#include "OmSk9822.h"

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

/// random range
float rr(float low, float high);
/// random range from low to high-1
int ir(int low, int high);
/// random range, with 50% chance of sign-flip.
float rrS(float low, float high);

CRGB chsv(int hue, int saturation, int value);

int migrateI(int x, int dest, int delta);
float migrateF(float x, float dest, float delta);
float mapRange(float x, float inLow, float inHigh, float outLow, float outHigh);
float mapRangeNoPin(float x, float inLow, float inHigh, float outLow, float outHigh);
float pinRange(float x, float bound0, float bound1);
#endif // _OMLEDS_H_
