//
//  OmLed16Patterns.h
//  OmLedHelpersDev
//
//  Created by David Van Brink on 5/10/2020 Plague Time.
//  Copyright © 2020 David Van Brink. All rights reserved.
//

#ifndef __OmLed16Patterns_h__
#define __OmLed16Patterns_h__

#include "OmLed16.h"
#include <stdio.h>
#include <vector>

class OmLed16Pattern
{
public:
    std::vector<const char *> colorParamNames;
    std::vector<OmLed16> colorParamValues;

    std::vector<const char *> intParamNames;
    std::vector<int> intParamValues;

    std::vector<const char *> actionNames; // buttons

    const char *name = NULL;
    char nameX[16];

    int ledCount;

    uint32_t totalMs;

    OmLed16Pattern()
    {
        return;
    }

    virtual ~OmLed16Pattern()
    {
        return;
    }

    void init(int ledCount)
    {
        this->ledCount = ledCount;
        this->totalMs = 0;
        this->innerInit();
    }
    void tick(unsigned int milliseconds, OmLed16Strip *strip)
    {
        if(milliseconds > 500)
            milliseconds = 100;

        this->totalMs += milliseconds;
        if(strip)
            strip->clear();
        this->innerTick(milliseconds, strip);
    }

    const char *getName()
    {
        if(this->name)
            return name;
        sprintf(this->nameX, "p%08x", (int)(long)this);
        return this->nameX;
    }

    std::vector<const char *> &getColorParamNames()
    {
        return this->colorParamNames;
    }

    std::vector<OmLed16> &getColorParamValues()
    {
        return this->colorParamValues;
    }

    void setColorParamValue(unsigned int ix, OmLed16 value)
    {
        if(ix < this->colorParamValues.size())
            this->colorParamValues[ix] = value;
    }

    std::vector<const char *> &getIntParamNames()
    {
        return this->intParamNames;
    }

    std::vector<int> &getIntParamValues()
    {
        return this->intParamValues;
    }

    void setIntParamValue(unsigned int ix, float value)
    {
        if(ix < this->intParamValues.size())
            this->intParamValues[ix] = value;
    }

    std::vector<const char *> getActionNames()
    {
        return this->actionNames;
    }

    void doAction(unsigned int ix, bool buttonDown)
    {
        if(ix < this->actionNames.size())
            this->innerDoAction(ix, buttonDown);
    }

    void doRotaryControl(int incDec, bool buttonDown)
    {
        if(incDec < 0)
            incDec = -1;
        else if(incDec > 0)
            incDec = +1;
        this->innerDoRotaryControl(incDec, buttonDown);
    }



protected:
    void addColorParam(const char *name, OmLed16 value)
    {
        this->colorParamNames.push_back(name);
        this->colorParamValues.push_back(value);
    }

    void addIntParam(const char *name, int value)
    {
        this->intParamNames.push_back(name);
        this->intParamValues.push_back(value);
    }

    void addAction(const char *name)
    {
        this->actionNames.push_back(name);
    }

    virtual void innerInit() = 0;
    virtual void innerTick(unsigned int milliseconds, OmLed16Strip *strip) = 0;
    virtual void innerDoAction(unsigned int ix, bool buttonDown) {};
    virtual void innerDoRotaryControl(int incDec, bool buttonDown) {};
};

class OmLed16PatternCrossfade
{
public:
    OmLed16Strip *strip0;
    OmLed16Strip *strip1;
    unsigned int transitionTime;
    unsigned int currentTime = 0;

    OmLed16PatternCrossfade(OmLed16Strip *strip0, OmLed16Strip *strip1, unsigned int transitionTime)
    {
        this->strip0 = strip0;
        this->strip1 = strip1;
        if(transitionTime < 1)
            transitionTime = 1;
        this->transitionTime = transitionTime;
    }

    /// return "true" if it's still crossfading.
    bool tick(unsigned int ms, OmLed16Strip *strip)
    {
        bool result = true;
        this->currentTime += ms;
        if(this->currentTime >= this->transitionTime)
            result = false;

        float w1 = (float)this->currentTime / this->transitionTime;
        float w0 = 1.0 - w1;

        for(int ix = 0; ix < strip->ledCount; ix++)
        {
            strip->leds[ix] = strip0->leds[ix] * w0 + strip1->leds[ix] * w1;
            //            strip->leds[ix] = OmLed16(0,0,0);
        }

        return result;
    }
};

class SimplePattern : public OmLed16Pattern
{
public:
    float k = 0;
    void innerInit() override
    {
        this->addColorParam("color", OmLed16(0x8000, 0x4000, 0x4000));
        this->addIntParam("rate", 1);
    }

    void innerTick(unsigned int ms, OmLed16Strip *strip) override
    {
        float rate = this->intParamValues[0];
        OmLed16 color = this->colorParamValues[0];

        this->k += ms * rate / 1000.0;

        if(strip)
        {
            strip->fillRange(this->k, this->k + 1, color);
        }
    }
};

#endif // __OmLed16Patterns_h__
