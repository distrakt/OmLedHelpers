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
#include "OmLedUtils.h"
#include <stdio.h>
#include <vector>

#ifndef UNUSED
#define UNUSED(x) (void)(x)
#endif

typedef enum EOmLedPatternParamType
{
    PPT_NONE = 0,
    PPT_INT = 1,
    PPT_COLOR = 2,
    PPT_BUTTON = 3 // button or action
} EOmLedPatternParamType;

template <typename LEDT>
class OmLedTPattern
{
public:

    class OmLedPatternParam
    {
    public:
        OmLedPatternParam()
        {
            this->name = "";
            this->colorValue = LEDT(0,0,0);
            this->intValue = 0;
        }
        EOmLedPatternParamType type = PPT_INT;
        const char *name;
        union
        {
            LEDT colorValue;
            int32_t intValue;
        };
    };

    std::vector<OmLedPatternParam> params;

    const char *name = NULL;
    char nameX[16];

    int ledCount;

    uint32_t totalMs;

    OmLedTPattern()
    {
        return;
    }

    virtual ~OmLedTPattern()
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

    int getParamCount()
    {
        return (int)(this->params.size());
    }

    const char *getParamName(int ix)
    {
        if(ix < 0 || ix >= this->params.size())
            return NULL;
        return this->params[ix].name;
    }

    EOmLedPatternParamType getParamType(int ix)
    {
        if(ix < 0 || ix >= this->params.size())
            return PPT_NONE;
        return this->params[ix].type;
    }

    LEDT getParamValueColor(int ix)
    {
        if(ix < 0 || ix >= this->params.size())
            return LEDT(0,0,0);
        OmLedPatternParam *pp = &this->params[ix];
        if(pp->type != PPT_COLOR)
            return LEDT(0,0,0);
        return pp->colorValue;
    }

    int getParamValueInt(int ix)
    {
        if(ix < 0 || ix >= this->params.size())
            return 0;

        OmLedPatternParam *pp = &this->params[ix];
        if(pp->type == PPT_COLOR)
            return pp->colorValue.toHex();
        if(pp->type == PPT_INT)
            return pp->intValue;
        return 0;
    }

    void setParamValueColor(int ix, LEDT co)
    {
        if(ix < 0 || ix >= this->params.size())
            return;
        OmLedPatternParam *pp = &this->params[ix];
        if(pp->type == PPT_COLOR)
            pp->colorValue = co;
        if(pp->type == PPT_INT)
            pp->intValue = co.toHex();
    }

    void setParamValueInt(int ix, int x)
    {
        if(ix < 0 || ix >= this->params.size())
            return;
        OmLedPatternParam *pp = &this->params[ix];
        if(pp->type == PPT_COLOR)
            pp->colorValue = LEDT(x);
        if(pp->type == PPT_INT)
            pp->intValue = x;
    }

    std::vector<OmLedPatternParam *> getSomeParams(EOmLedPatternParamType type)
    {
        std::vector<OmLedPatternParam *> result;
        for(OmLedPatternParam &aParam : this->params)
        {
            if(aParam.type == type)
                result.push_back(&aParam);
        }
        return result;
    }

    OmLedPatternParam *getAParam(EOmLedPatternParamType type, unsigned int ix)
    {
        auto ps = getSomeParams(type);
        if(ix < ps.size())
            return ps[ix];
        else
            return NULL;
    }

    std::vector<int> getK()
    {
        std::vector<int> result;
        return result;
    }

    std::vector<const char *> getSomeParamNames(EOmLedPatternParamType type)
    {
        auto ps = this->getSomeParams(type);
        std::vector<const char *> result;
        for(OmLedPatternParam *p : ps)
            result.push_back(p->name);
        return result;
    }

    std::vector<const char *> getColorParamNames()
    {
        return this->getSomeParamNames(PPT_COLOR);
    }

    std::vector<const char *> getIntParamNames()
    {
        return this->getSomeParamNames(PPT_INT);
    }

    std::vector<LEDT> getColorParamValues()
    {
        auto ps = this->getSomeParams(PPT_COLOR);
        std::vector<LEDT> result;
        for(OmLedPatternParam *p : ps)
            result.push_back(p->colorValue);
        return result;
    }

    LEDT getColorParamValue(unsigned int ix)
    {
        OmLedPatternParam *pp = this->getAParam(PPT_COLOR, ix);
        if(pp)
            return pp->colorValue;
        else
            return LEDT(0,0,0);
    }

    void setColorParamValue(unsigned int ix, LEDT value)
    {
        auto ps = this->getSomeParams(PPT_COLOR);
        if(ix < ps.size())
            ps[ix]->colorValue = value;
    }

    int getIntParamValue(unsigned int ix)
    {
        OmLedPatternParam *pp = this->getAParam(PPT_INT, ix);
        if(pp)
            return pp->intValue;
        else
            return 0;
    }

    std::vector<int> getIntParamValues()
    {
        auto ps = this->getSomeParams(PPT_INT);
        std::vector<int> result;
        for(OmLedPatternParam *p : ps)
            result.push_back(p->intValue);
        return result;
    }

    void setIntParamValue(unsigned int ix, int value)
    {
        auto pp = this->getAParam(PPT_INT, ix);
        if(pp)
            pp->intValue = value;
    }

    std::vector<const char *> getActionNames()
    {
        return this->getSomeParamNames(PPT_BUTTON);
    }

    void doAction(unsigned int ix, bool buttonDown)
    {
        auto ps = this->getSomeParams(PPT_BUTTON);
        if(ix < ps.size())
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
    void addColorParam(const char *name, LEDT value)
    {
        OmLedPatternParam p;
        p.type = PPT_COLOR;
        p.name = name;
        p.colorValue = value;
        this->params.push_back(p);
    }

    void addIntParam(const char *name, int value)
    {
        OmLedPatternParam p;
        p.type = PPT_INT;
        p.name = name;
        p.intValue = value;
        this->params.push_back(p);
    }

    void addAction(const char *name)
    {
        OmLedPatternParam p;
        p.type = PPT_BUTTON;
        p.name = name;
        this->params.push_back(p);
    }

    virtual void innerInit() = 0;
    virtual void innerTick(unsigned int milliseconds, OmLed16Strip *strip) = 0;
    virtual void innerDoAction(unsigned int ix, bool buttonDown) { UNUSED(ix); UNUSED(buttonDown); };
    bool rotaryButtonIsDown = false;
    virtual void innerDoRotaryControl(int incDec, bool buttonDown)
    {
        // default implementation changes the first slider or second slider.
        if(incDec)
        {
            unsigned int paramIx = this->rotaryButtonIsDown ? 1 : 0;
            OmLedPatternParam *pp = this->getAParam(PPT_INT, paramIx);
            if(pp)
                pp->intValue = pinRange(pp->intValue + incDec, 0, 100);
        }
        else
        {
            this->rotaryButtonIsDown = buttonDown;
        }
    }
};

typedef OmLedTPattern<OmLed16> OmLed16Pattern;

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
    bool b0 = false;
    void innerInit() override
    {
        this->addColorParam("color", OmLed16(0x8000, 0x4000, 0x4000));
        this->addIntParam("rate", 1);
        this->addAction("go");

        auto a = this->getColorParamNames();
        auto b = this->getColorParamValues();
        auto c = this->getIntParamNames();
        auto d = this->getIntParamValues();
        auto e = this->getActionNames();
    }

    void innerTick(unsigned int ms, OmLed16Strip *strip) override
    {
        float rate = this->getIntParamValues()[0];
        OmLed16 color = this->getColorParamValues()[0];

        this->k += ms * rate / 1000.0;

        if(strip)
        {
            strip->fillRange(this->k, this->k + 1, color);
        }
    }

    void innerDoAction(unsigned int ix, bool buttonDown) override
    {
        this->b0 = buttonDown;
    }
};

#endif // __OmLed16Patterns_h__
