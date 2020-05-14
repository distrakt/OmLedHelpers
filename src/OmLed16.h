// For Arduino UNO, trying out "hi-res" Sk9822 color control
// Which has 8 bits of r,g,b, and a 5 bit multiplier per pixel
// datasheet https://www.pololu.com/file/0J1234/sk9822_datasheet.pdf
// more good notes: https://cpldcpu.wordpress.com/2016/12/13/sk9822-a-clone-of-the-apa102/

#ifndef __OmLed16_h__
#define __OmLed16_h__

#include <stdlib.h>
#include <math.h>

// class represents 16-bit rgb components, and
// conversions suitable for Sk9822 or Apa102 display.
class OmLed16
{
public:
    unsigned short r;
    unsigned short g;
    unsigned short b;

    static float hexGamma;  // gamma
    static float hexGamma1; // inverse gamma
    static void setHexGamma(float hexGamma)
    {
        OmLed16::hexGamma = hexGamma;
        OmLed16::hexGamma1 = 1.0 / OmLed16::hexGamma;
    }

    OmLed16 operator +(OmLed16 other)
    {
        OmLed16 result(this->r + other.r, this->g + other.g, this->b + other.b);
        return result;
    }

    OmLed16()
    {
        this->r = 0;
        this->g = 0;
        this->b = 0;
    }

    OmLed16(unsigned short r,unsigned short g,unsigned short b)
    {
        this->r = r;
        this->g = g;
        this->b = b;
    }

    OmLed16(int hex)
    {
        unsigned short r = (hex >> 16) & 0xff;
        unsigned short g = (hex >> 8) & 0xff;
        unsigned short b = hex & 0xff;

        float rF = r / 255.0;
        float gF = g / 255.0;
        float bF = b / 255.0;

        this->r = pow(rF, hexGamma) * 65535;;
        this->g = pow(gF, hexGamma) * 65535;;
        this->b = pow(bF, hexGamma) * 65535;;
    }

    int toHex()
    {
        float rF = this->r / 65535.0;
        float gF = this->g / 65535.0;
        float bF = this->b / 65535.0;

        rF = pow(rF, hexGamma1);
        gF = pow(gF, hexGamma1);
        bF = pow(bF, hexGamma1);
        rF = rF * 255.0;
        gF = gF * 255.0;
        bF = bF * 255.0;
        int r = rF + 0.5;
        int g = gF + 0.5;;
        int b = bF + 0.5;;
        int result = (r << 16) | (g << 8) | b;
        return result;
    }

    static OmLed16 hsv(unsigned short h, unsigned short s, unsigned short v)
    {
        OmLed16 r;
        r.setHsv(h, s, v);
        return r;
    }

    // return simple 8-bit dotstar, no low-value scaling.
    unsigned long dot8()
    {
        unsigned long x = 0xff000000 | ((this->r & 0xff00) >> 8) | (this->g & 0xff00) | (((long)this->b & 0xff00) << 8);
        return x;
    }

    unsigned short maxComponent()
    {
        if(this->r > this->g)
        {
            if(this->r > this->b)
                return this->r;
            else
                return this->b;
        }
        else
        {
            if(this->g > this->b)
                return this->g;
            else
                return this->b;
        }
    }

    // return the dot star 32 bit value for this pixel
    // dot() uses one of three Brightness multipliers, 31, 5, or 1.
    unsigned long dot()
    {
        unsigned long x;

        // cheap estimate of max component
        unsigned short maxComponent = this->r | this->g | this->b;

        // we kick in the 31 bit scalar in 3 regimes: 31, 5, and 1.
        // the full 16 bit ranges are:
        // 31/31 * 0xffff --> 65535.00
        //  5/31 * 0xffff --> 10570.16
        //  1/31 * 0xffff -->  2114.03

        if (maxComponent >= 10570)
        {
            // use brightness 31, and rgb directly.
            x = 0xff000000 | (this->r >> 8) | (this->g & 0xff00) | (((long)this->b & 0xff00) << 8);
        }
        else if (maxComponent >= 2114)
        {
            // use brightness 5, and scale up
            unsigned short rr = this->r * 31L / 5;
            unsigned short gg = this->g * 31L / 5;
            unsigned long bb = this->b * 31L / 5;
            x = 0xe5000000 | (rr >> 8) | (gg & 0xff00) | ((bb & 0xff00) << 8);
        }
        else
        {
            // dimmest regime, brightness 1, and scale up
            unsigned long rr = this->r * 31;
            unsigned long gg = this->g * 31;
            unsigned long bb = this->b * 31;
            x = 0xe1000000 | (rr >> 8) | (gg & 0xff00) | ((bb & 0xff00) << 8);
        }
        return x;
    }

    // like dot() with more accurate brigthness, but slower.
    // I find that regular dot() is just fine.
    unsigned long dotY()
    {
        // use all brightness values, 0 up to 31, for best resolution available.
        // r, g, b are 16 bit unsigned 16 bit int.
#define DOTY_BRIGHTNESS_MAX 31
        unsigned long maxComponent = this->r | this->g | this->b;
        unsigned long brightness = (maxComponent * DOTY_BRIGHTNESS_MAX + 65534) / 65535;

        unsigned long scalar = 0x001f0000 / brightness;
        unsigned long rr = this->r * scalar;
        unsigned long gg = this->g * scalar;
        unsigned long bb = this->b * scalar;
        unsigned long x = ((0xe0 + brightness) << 24) | (rr >> 24) | ((gg >> 16) & 0xff00) | ((bb >> 8) & 0xff0000);
        return x;
    }

    void setHsv( unsigned short h,  long s,  long v)
    {
        s = 65535 - s; // now it's more like the "whiteness level", or min per component
        s = (long)s * v / 65535; // scaled by brightness.

#define otrMap(_x, _inLow, _inHigh, _outLow, _outHigh) (_x - _inLow) * (_outHigh - _outLow) / (_inHigh - _inLow) + _outLow

#define hsvHextant0 0
#define hsvHextant1 10923
#define hsvHextant2 21845
#define hsvHextant3 32768
#define hsvHextant4 43691
#define hsvHextant5 54613
#define hsvHextant6 65535


        if (h < hsvHextant1)
        {
            this->r = v;
            this->g = otrMap(h, hsvHextant0, hsvHextant1, s, v);
            this->b = s;
        }
        else if (h < hsvHextant2)
        {
            this->r = otrMap(h, hsvHextant1, hsvHextant2, v, s);
            this->g = v;
            this->b = s;
        }
        else if (h < hsvHextant3)
        {
            this->r = s;
            this->g = v;
            this->b = otrMap(h, hsvHextant2, hsvHextant3, s, v);
        }
        else if (h < hsvHextant4)
        {
            this->r = s;
            this->g = otrMap(h, hsvHextant3, hsvHextant4, v, s);
            this->b = v;
        }
        else if (h < hsvHextant5)
        {
            this->r = otrMap(h, hsvHextant4, hsvHextant5, s, v);
            this->g = s;
            this->b = v;
        }
        else
        {
            this->r = v;
            this->g = s;
            this->b = otrMap(h, hsvHextant5, hsvHextant6, v, s);
        }
    }

    void operator *=(float x)
    {
        this->r *= x;
        this->g *= x;
        this->b *= x;
    }

    OmLed16 operator *(float x)
    {
        OmLed16 result;
        result.r = this->r * x;
        result.g = this->g * x;
        result.b = this->b * x;
        return result;
    }

    //  void operator +=(OmLed16 &other)
    //  {
    //    this->r += other.r;
    //    this->g += other.g;
    //    this->b += other.b;
    //  }

#define ADD_SAT(_a,_b) _a = _a + _b; if(_a < _b) _a = -1;
    void operator +=(OmLed16 other)
    {
        ADD_SAT(this->r, other.r);
        ADD_SAT(this->g, other.g);
        ADD_SAT(this->b, other.b);
    }
};

#include "math.h"

class OmLed16Strip
{
public:
    int ledCount;
    OmLed16 *leds;
    bool needsDispose;

    OmLed16Strip(int ledCount, OmLed16 *leds = NULL)
    {
        this->ledCount = ledCount;
        if(leds)
        {
            this->leds = leds;
            this->needsDispose = false;
        }
        else
        {
            this->leds = (OmLed16 *)calloc(ledCount, sizeof(OmLed16));
            this->needsDispose = true;
        }
    }

    ~OmLed16Strip()
    {
        if(this->needsDispose && this->leds)
        {
            free(this->leds);
        }
        this->leds = NULL;
    }

    void clear()
    {
        OmLed16 *w = this->leds;
        for(int ix = 0; ix < this->ledCount; ix++)
        {
            w->r = 0;
            w->g = 0;
            w->b = 0;
            w++;
        }
    }

    void fillRange(float low, float high, OmLed16 co)
    {
        if(low >= this->ledCount)
            return;
        if(high <= 0)
            return;
        if(low < 0)
            low = 0;
        if(high > ledCount)
            high = ledCount;

        int xi = floor(low);
        int ei = floor(high);

        if (xi == ei)
        {
            // only one LED lit
            float f = high - low;
            this->leds[xi] += co * f;
        }
        else
        {
            OmLed16 coF;
            float f;

            // leftmost pixel
            f = xi + 1 - low;
            this->leds[xi] += co * f;

            // middle pixels, if any
            for (int k = xi + 1; k < ei; k++)
                this->leds[k] += co;

            // rightmost pixel
            f = high - ei;
            this->leds[ei] += co * f;
        }
    }
};

#endif
