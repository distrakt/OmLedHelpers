#include "OmLedUtils.h"
#include <stdlib.h>

#ifdef _WIN32
#define FRAND ((float)rand() / (float)RAND_MAX)
#else
#define FRAND drand48()
#endif

int ir(int low, int high)
{
    if(low == high)
        return low;
    return rand() % (high - low) + low;
}

int ir(int max)
{
    return ir(0, max);
}

/// random range
float rr(float high)
{
    return rr(0, high);
}

float rr(float low, float high)
{
    return low + drand48() * (high - low);
}

/// random range, with 50% chance of sign-flip.
float rrS(float low, float high)
{
    float r = rr(low, high);
    if(ir(0,2))
        r = -r;
    return r;
}


CRGB chsv(int hue, int saturation, int value)
{
    unsigned char h = hue;
    unsigned char s = saturation;
    unsigned char v = value;

    CRGB result;
    unsigned char* rgbOut = result.raw;
    s = 255 - s; // now it's more like the "whiteness level", or min per component
    s = s * v / 255; // scaled by brightness.

#define otrMap(_x, _inLow, _inHigh, _outLow, _outHigh) (_x - _inLow) * (_outHigh - _outLow) / (_inHigh - _inLow) + _outLow
    if(h < 43)
    {
        rgbOut[0] = v;
        rgbOut[1] = otrMap(h, 0, 43, s, v);
        rgbOut[2] = s;
    }
    else if(h < 85)
    {
        rgbOut[0] = otrMap(h, 43, 85, v, s);
        rgbOut[1] = v;
        rgbOut[2] = s;
    }
    else if(h < 128)
    {
        rgbOut[0] = s;
        rgbOut[1] = v;
        rgbOut[2] = otrMap(h, 85, 128, s, v);
    }
    else if(h < 170)
    {
        rgbOut[0] = s;
        rgbOut[1] = otrMap(h, 128, 170, v, s);
        rgbOut[2] = v;
    }
    else if(h < 213)
    {
        rgbOut[0] = otrMap(h, 170, 217, s, v);
        rgbOut[1] = s;
        rgbOut[2] = v;
    }
    else /* if(h <= 255) always is */
    {
        rgbOut[0] = v;
        rgbOut[1] = s;
        rgbOut[2] = otrMap(h, 213, 256, v, s);
    }

    return result;
}

int migrateI(int x, int dest, int delta)
{
    if(x < dest)
    {
        x += delta;
        if(x > dest)
            x = dest;
    }
    else if(x > dest)
    {
        x -= delta;
        if(x < dest)
            x = dest;
    }
    return x;
}

float migrateF(float x, float dest, float delta)
{
    if(x < dest)
    {
        x += delta;
        if(x > dest)
            x = dest;
    }
    else if(x > dest)
    {
        x -= delta;
        if(x < dest)
            x = dest;
    }
    return x;
}

float pinRangeI(int x, int bound0, int bound1)
{
    if(bound0 > bound1)
    {
        int t = bound0;
        bound0 = bound1;
        bound1 = t;
    }
    if(x < bound0)
        return bound0;
    if(x >= bound1)
        return bound1 - 1;
    return x;
}

float pinRange(float x, float bound0, float bound1)
{
    if(bound0 > bound1)
    {
        float t = bound0;
        bound0 = bound1;
        bound1 = t;
    }
    if(x < bound0)
        return bound0;
    if(x > bound1)
        return bound1;
    return x;
}

float mapRange(float x, float inLow, float inHigh, float outLow, float outHigh)
{
    x -= inLow;
    x = x * (outHigh - outLow) / (inHigh - inLow);
    x += outLow;

    x = pinRange(x, outLow, outHigh);
    return x;
}

float mapRangeNoPin(float x, float inLow, float inHigh, float outLow, float outHigh)
{
    x -= inLow;
    x = x * (outHigh - outLow) / (inHigh - inLow);
    x += outLow;
    return x;
}

int umod(int a, int b)
{
    int result = a % b;
    if(result < 0)
        result += b;
    return result;
}
