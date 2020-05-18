#include "OmWs2812.h"
#include "SPI.h"

/*
 https://www.arrow.com/en/research-and-events/articles/protocol-for-the-ws2812b-programmable-led
 https://cdn-shop.adafruit.com/datasheets/WS2812B.pdf

 Tinic Uro Use this as inspriration: https://github.com/PetrakovKirill/ws2812_lightDrum/blob/519a484ccd5e66dee91bbb83bc8156aec00a12ee/program/ws2812_stm32.c
 or my code to build the table from scratch: https://github.com/tinic/lightkraken/blob/537765937a4326fbb8b2c68ea827fc128539f2da/strip.h

 ESP8266 is little-endian.
 WS2818 receives data GRB big endian

 From spec sheet. "Note: Follow the order of GRB to sent data and the high bit sent at first."

 * This table takes one byte of color component data for WS2812 and represents
 * it as a series of pulses to be sent by SPI to an LED strip.
 *
 * This table is LSB throughout; a 32-bit entry here will be sent from Right to Left.
 * So we tell SPI to be in LSB mode.
 *
 * ws2812 wants the most significant bit first. Hence, entry 128 looks like 1111113, that
 * rightmost 3 goes out first, starting from the right hand side.
 */
static const uint32_t ws2818SpiTable[] =
{
    0x11111111, 0x31111111, 0x13111111, 0x33111111, 0x11311111, 0x31311111, 0x13311111, 0x33311111,
    0x11131111, 0x31131111, 0x13131111, 0x33131111, 0x11331111, 0x31331111, 0x13331111, 0x33331111,
    0x11113111, 0x31113111, 0x13113111, 0x33113111, 0x11313111, 0x31313111, 0x13313111, 0x33313111,
    0x11133111, 0x31133111, 0x13133111, 0x33133111, 0x11333111, 0x31333111, 0x13333111, 0x33333111,
    0x11111311, 0x31111311, 0x13111311, 0x33111311, 0x11311311, 0x31311311, 0x13311311, 0x33311311,
    0x11131311, 0x31131311, 0x13131311, 0x33131311, 0x11331311, 0x31331311, 0x13331311, 0x33331311,
    0x11113311, 0x31113311, 0x13113311, 0x33113311, 0x11313311, 0x31313311, 0x13313311, 0x33313311,
    0x11133311, 0x31133311, 0x13133311, 0x33133311, 0x11333311, 0x31333311, 0x13333311, 0x33333311,
    0x11111131, 0x31111131, 0x13111131, 0x33111131, 0x11311131, 0x31311131, 0x13311131, 0x33311131,
    0x11131131, 0x31131131, 0x13131131, 0x33131131, 0x11331131, 0x31331131, 0x13331131, 0x33331131,
    0x11113131, 0x31113131, 0x13113131, 0x33113131, 0x11313131, 0x31313131, 0x13313131, 0x33313131,
    0x11133131, 0x31133131, 0x13133131, 0x33133131, 0x11333131, 0x31333131, 0x13333131, 0x33333131,
    0x11111331, 0x31111331, 0x13111331, 0x33111331, 0x11311331, 0x31311331, 0x13311331, 0x33311331,
    0x11131331, 0x31131331, 0x13131331, 0x33131331, 0x11331331, 0x31331331, 0x13331331, 0x33331331,
    0x11113331, 0x31113331, 0x13113331, 0x33113331, 0x11313331, 0x31313331, 0x13313331, 0x33313331,
    0x11133331, 0x31133331, 0x13133331, 0x33133331, 0x11333331, 0x31333331, 0x13333331, 0x33333331,
    0x11111113, 0x31111113, 0x13111113, 0x33111113, 0x11311113, 0x31311113, 0x13311113, 0x33311113,
    0x11131113, 0x31131113, 0x13131113, 0x33131113, 0x11331113, 0x31331113, 0x13331113, 0x33331113,
    0x11113113, 0x31113113, 0x13113113, 0x33113113, 0x11313113, 0x31313113, 0x13313113, 0x33313113,
    0x11133113, 0x31133113, 0x13133113, 0x33133113, 0x11333113, 0x31333113, 0x13333113, 0x33333113,
    0x11111313, 0x31111313, 0x13111313, 0x33111313, 0x11311313, 0x31311313, 0x13311313, 0x33311313,
    0x11131313, 0x31131313, 0x13131313, 0x33131313, 0x11331313, 0x31331313, 0x13331313, 0x33331313,
    0x11113313, 0x31113313, 0x13113313, 0x33113313, 0x11313313, 0x31313313, 0x13313313, 0x33313313,
    0x11133313, 0x31133313, 0x13133313, 0x33133313, 0x11333313, 0x31333313, 0x13333313, 0x33333313,
    0x11111133, 0x31111133, 0x13111133, 0x33111133, 0x11311133, 0x31311133, 0x13311133, 0x33311133,
    0x11131133, 0x31131133, 0x13131133, 0x33131133, 0x11331133, 0x31331133, 0x13331133, 0x33331133,
    0x11113133, 0x31113133, 0x13113133, 0x33113133, 0x11313133, 0x31313133, 0x13313133, 0x33313133,
    0x11133133, 0x31133133, 0x13133133, 0x33133133, 0x11333133, 0x31333133, 0x13333133, 0x33333133,
    0x11111333, 0x31111333, 0x13111333, 0x33111333, 0x11311333, 0x31311333, 0x13311333, 0x33311333,
    0x11131333, 0x31131333, 0x13131333, 0x33131333, 0x11331333, 0x31331333, 0x13331333, 0x33331333,
    0x11113333, 0x31113333, 0x13113333, 0x33113333, 0x11313333, 0x31313333, 0x13313333, 0x33313333,
    0x11133333, 0x31133333, 0x13133333, 0x33133333, 0x11333333, 0x31333333, 0x13333333, 0x33333333,
};

static uint8_t *spiBuffer = NULL;
static int spiBufferSize = 0;

#define RESETK 100 // generate hold-low reset pulse, in bytes. must be multiple of 4.
#define SPIRATE 3000000
void OmWs2812Writer::showLeds(CRGB *leds, int ledCount)
{

    int bytesNeeded = ledCount * 3 * 4 + RESETK;

    if(!spiBuffer)
    {
        spiBuffer = (uint8_t *)malloc(bytesNeeded);
        if(spiBuffer)
            spiBufferSize = bytesNeeded;
    }

    if(bytesNeeded > spiBufferSize)
    {
        static bool reported = false;
        if(!reported)
        {
            Serial.printf("buffer %d too small for %d LEDs, need %d\n", spiBufferSize, ledCount, bytesNeeded);
            reported = true;
        }
        return;
    }

    uint8_t *sb = spiBuffer;

    // the reset pulse
    for(int ix = 0; ix < RESETK; ix++)
        *sb++ = 0;

    // the led array
    uint32_t *sb32 = (uint32_t *)sb;
    for(int ix = 0; ix < ledCount; ix++)
    {
        *sb32++ = ws2818SpiTable[leds->g];
        *sb32++ = ws2818SpiTable[leds->r];
        *sb32++ = ws2818SpiTable[leds->b];
        leds++;
    }
    sb = (uint8_t *)sb32;

    int bytesToSend = sb - spiBuffer;
    if(bytesToSend != bytesNeeded)
        Serial.printf("what? bytesToSend %d != bytesNeeded %d?\n", bytesToSend, bytesNeeded);

    SPI.begin();
    SPI.beginTransaction(SPISettings(SPIRATE, LSBFIRST, SPI_MODE0));
    SPI.transfer(spiBuffer, bytesToSend);

    return;
}
