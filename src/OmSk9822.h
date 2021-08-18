/*
 * 2020-05-06 Plague Time
 * Low-level code to write bits to APA-102/Sk9822 led strips
 * based on 16-bit pixels
 */

// TODO roll back in to digitalWriteFast.

#ifndef __OmSk9822_h__
#define __OmSk9822_h__

#include "OmLedT.h"
#include "OmLedTStrip.h"

template <int CLOCK_PIN, int DATA_PIN, int USE_SPI = 0>
class OmSk9822Writer
{
public:
    inline void w1(int bit)
    {
        DIGITAL_WRITE(CLOCK_PIN, 0);
        //  delay(10);
        DIGITAL_WRITE(DATA_PIN, bit);
        //  delay(10);
        DIGITAL_WRITE(CLOCK_PIN, 1);
        //  delay(10);
    }

    // send N bits down the wire, remembering that it's MSB first.
    void wN(unsigned long value, int bitCount)
    {
        unsigned long mask = 1L << (bitCount - 1);
        while (bitCount--)
        {
            int bit = (value & mask) ? 1 : 0;
            w1(bit);
            value <<= 1;
        }
    }


#if USE_SPI
    inline void writeSpi(uint8_t data) {
        while (!(SPSR & _BV(SPIF))) ;
        SPDR = data;
    }
#endif

    OmSk9822Writer()
    {
#if !USE_SPI
        PIN_MODE(CLOCK_PIN, OUTPUT);
        PIN_MODE(DATA_PIN, OUTPUT);
#endif
    }
    int ww = 0;
    void w32(unsigned long x)
    {
//        if(USE_SPI)
#if USE_SPI
        {
            SPI.transfer (x >> 24);
            SPI.transfer (x >> 16);
            SPI.transfer (x >>  8);
            SPI.transfer (x >>  0);
            ww += x;
        }
//        else
#else
        {
            for(int ix = 0; ix < 32; ix++)
            {
                DIGITAL_WRITE(CLOCK_PIN, 0);
                if(x & 0x80000000)
                {
                    DIGITAL_WRITE(DATA_PIN, 1);
                }
                else
                {
                    DIGITAL_WRITE(DATA_PIN, 0);
                }
                DIGITAL_WRITE(CLOCK_PIN, 1);
                x = x + x;
            }
        }
    }
#endif

    void blacks(int n)
    {
        while(n-- > 0)
            w32(0xe0000000);
    }

    void showStrip(OmLed16Strip *strip)
    {
        for(int ix = 0; ix < strip->ledCount; ix++)
            w32(strip->leds[ix].dot());
        w32(0);
        w32(0); // urr probably needs more of these. or at least the correct number.
    }
};

#endif
