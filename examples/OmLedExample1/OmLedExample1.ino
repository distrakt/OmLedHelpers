#include "FastLED.h"
#include "OmLedHelpers.h"


#define LED_COUNT 24
#define DATA_PIN D5

CRGB pixels[LED_COUNT];
OmLeds leds(pixels, LED_COUNT);

void setup()
{
    Serial.begin(115200);
    pinMode(DATA_PIN, OUTPUT);
    FastLED.addLeds<WS2812, DATA_PIN, RGB>(pixels, LED_COUNT);
}

void loop()
{
    static int ticks = 0;
    static float x = 0;
    static float hue = 0;
    delay(16.666);

    ticks++;
    if(ticks % 123 == 0)
    {
        Serial.print("ticks ");
        Serial.println(ticks);
    }

    leds.clear();
    CRGB co = 0;
    co = CHSV(hue, 255, 180);
    hue += 1.8;
    if(hue >= 256)
    hue -= 256;
    x += 0.04;
    if(x > LED_COUNT)
    x -= LED_COUNT;
    leds.set(co, x);
    leds.show();
}
