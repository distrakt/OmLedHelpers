#include "OmLedHelpers.h"
#include <math.h>
#include <stdlib.h>

OmLeds::OmLeds(CRGB *leds, int ledCount, bool isRing)
{
    this->ledCount = ledCount;
    this->leds = leds;
    this->isRing = isRing;
}

void OmLeds::clear()
{
    for(int ix = 0; ix < this->ledCount; ix++)
        this->leds[ix] = 0;
}

void OmLeds::clear1(int ix)
{
    if(this->isRing)
    {
        ix = abs(ix % this->ledCount);
    }
    if(ix >= 0 && ix <= this->ledCount)
        this->leds[ix] = 0;
}
void OmLeds::setI(CRGB co, int x)
{
    if(this->isRing)
    {
        if(x < 0)
            x = -(-x % this->ledCount);
        else if(x >= this->ledCount)
            x = x % this->ledCount;
        this->leds[x] += co;
    }
    else if(x >= 0 && x < this->ledCount)
        this->leds[x] += co;
}

void OmLeds::set(CRGB co, float x)
{
    int x0 = floor(x);
    float r = x - x0;
    
    if(this->isRing)
    {
        x0 = x0 % this->ledCount;
        if(x0 < 0)
            x0 += this->ledCount;
    }
    int x1 = x0 + 1;
    if(this->isRing)
        x1 = x1 % this->ledCount;
    
    if(x1 >= 0 && x1 < this->ledCount && r)
        this->leds[x1] = co * r;
    r = 1.0 - r;
    if(x0 >= 0 && x0 < this->ledCount)
        this->leds[x0] += co * r;
}

void OmLeds::set(CRGB co, float x, float e)
{
    int xi = floor(x);
    int ei = floor(e);
    if (xi == ei)
    {
        // only one LED lit
        float f = e - x;
        CRGB coF = co * f;
        this->setI(coF, xi);
    }
    else
    {
        CRGB coF;
        float f;
        
        // leftmost pixel
        f = xi + 1 - x;
        coF = co * f;
        this->setI(coF, xi);
        
        // middle pixels, if any
        for (int k = xi + 1; k < ei; k++)
            this->setI(co, k);
        
        // rightmost pixel
        f = e - ei;
        coF = co * f;
        this->setI(coF, ei);
    }
}

void OmLeds::show()
{
    FastLED.show();
}

CRGB operator *(CRGB &co, float n)
{
    CRGB result;
    result.r = co.r * n;
    result.g = co.g * n;
    result.b = co.b * n;
    return result;
}

CRGB operator *(float n, CRGB &co)
{
    return co * n;
}

// ==========================================
OmLeds2d::OmLeds2d(OmLeds *leds, int width)
{
    this->leds = leds;
    this->w = width;
    this->h = leds->ledCount / this->w;
}
void OmLeds2d::setI(CRGB co, int x, int y)
{
    if(x < 0 || x >= this->w || y < 0 || y >= this->h)
        return;
    
    int ix = this->w * y + x;
    leds->set(co, ix);
}

void OmLeds2d::set(CRGB co, float x, float y)
{
    this->set(co, x, y, x+1, y+1);
}

/// pin range inclusive of each end
float pinRangeF(float x, float low, float high)
{
    if(low > high)
    {
        float t = low;
        low = high;
        high = t;
    }
    if(x < low)
        x = low;
    else if(x > high)
        x = high;
    return x;
}



// draws a rectangle, antialiased
void OmLeds2d::set(CRGB co, float x0, float y0, float x1, float y1)
{
    x0 = pinRangeF(x0, 0, this->w);
    x1 = pinRangeF(x1, 0, this->w);
    y0 = pinRangeF(y0, 0, this->h);
    y1 = pinRangeF(y1, 0, this->h);

    int y0i = floor(y0);
    int y1i = floor(y1);

    int xOffset = y0i * this->w;

    if (y0i == y1i)
    {
        // only one LED row lit
        float f = y1 - y0;
        CRGB coF = co * f;
        this->leds->set(coF, xOffset + x0, xOffset + x1);
    }
    else
    {
        CRGB coF;
        float f;
        
        // topmost row
        f = y0i + 1 - y0;
        coF = co * f;
        this->leds->set(coF, xOffset + x0, xOffset + x1);
        xOffset += this->w;

        // middle rows, if any
        for (int k = y0i + 1; k < y1i; k++)
        {
            this->leds->set(co, xOffset + x0, xOffset + x1);
            xOffset += this->w;
        }
        
        // bottom pixel row
        f = y1 - y1i;
        coF = co * f;
        this->leds->set(coF, xOffset + x0, xOffset + x1);
    }
}



