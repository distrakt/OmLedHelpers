//
//  OmLedTPatternManager.hpp
//  PatternsTester
//
//  Created by David Van Brink on 3/23/21.
//  Copyright © 2021 David Van Brink. All rights reserved.
//

#ifndef OmLedTPatternManager_hpp
#define OmLedTPatternManager_hpp

#include "OmLedTPatterns.h"

template <typename LEDT>
class OmLedTPatternManager
{
public:
    typedef OmLedTPattern<LEDT> PATTERNT;
    typedef OmLedTStrip<LEDT> STRIPT;

    int ledCount = 0;
    std::vector<PATTERNT *> patterns;
    int currentPatternIndex = -1;
    PATTERNT *currentPattern = NULL;

    // the cross fade pattern is the OLD pattern.
    // we set the new one immediately so the parameters are current.
    unsigned int crossfadeMs = 0;
    unsigned int crossfadeMsSoFar = 0;
    PATTERNT *crossfadePattern = NULL;
    int crossfadePatternIndex;
    STRIPT *crossfadeStrip = NULL;

    void addPattern(PATTERNT *pattern)
    {
        this->patterns.push_back(pattern);
    }

    void initPatterns(int ledCount)
    {
        this->ledCount = ledCount;
        for(PATTERNT *pattern : this->patterns)
        {
            pattern->init(ledCount);
        }
    }

    int getPatternCount()
    {
        return (int)this->patterns.size();
    }

    PATTERNT *getPattern(int ix)
    {
        if(ix < 0 || ix >= this->patterns.size())
            return NULL;
        return this->patterns[ix];
    }

    PATTERNT *getPattern()
    {
        return this->getPattern(this->currentPatternIndex);
    }

    int getPatternIndex()
    {
        return this->currentPatternIndex;
    }

    /// Change to pattern, crossfaded over some milliseconds.
    void setPattern(int nextPatternIx, unsigned int ms)
    {
        PATTERNT *nextPattern = NULL;
        if(nextPatternIx < 0 || nextPatternIx >= this->patterns.size())
        {
            nextPattern = NULL;
            nextPatternIx = -1;
        }
        else
        {
            nextPattern = this->patterns[nextPatternIx];
        }

        this->crossfadeMs = ms;
        this->crossfadeMsSoFar = 0;
        this->crossfadePattern = this->currentPattern;
        this->crossfadePatternIndex = this->currentPatternIndex;

        // change it right now, in place
        this->currentPattern = nextPattern;
        this->currentPatternIndex = nextPatternIx;
    }

    void setPattern(int ix)
    {
        this->setPattern(ix, 0);
    }

    void tickInto(unsigned int ms, STRIPT *ledStrip, PATTERNT *pattern)
    {
        if(pattern)
            pattern->tick(ms, ledStrip);
        else
            ledStrip->clear();
    }

    void tick(unsigned int ms, STRIPT *ledStrip)
    {
        if(this->crossfadeMs)
        {
            // we are crossfading! so be it.
            this->crossfadeMsSoFar += ms;
            if(this->crossfadeMsSoFar >= this->crossfadeMs)
            {
                // we're done crossfading. we have arrived.
                this->crossfadeMs = 0;
                this->crossfadeMsSoFar = 0;
                goto singlePattern;
            }

            // ok yes crossfade
            if(!this->crossfadeStrip)
                this->crossfadeStrip = new STRIPT(this->ledCount);
            float t = (float)this->crossfadeMsSoFar / this->crossfadeMs;
            this->tickInto(ms, ledStrip, this->currentPattern);
            this->tickInto(ms, crossfadeStrip, crossfadePattern);

            *ledStrip *= t;
            *crossfadeStrip *= 1.0 - t;
            *ledStrip += crossfadeStrip;
        }
        else
        {
        singlePattern:
            this->tickInto(ms, ledStrip, this->currentPattern);
        }
    }
};

typedef OmLedTPatternManager<OmLed8> OmLed8PatternManager;
typedef OmLedTPatternManager<OmLed16> OmLed16PatternManager;

OmLed16PatternManager *getPatternManager(int ledCount);

#endif /* OmLedTPatternManager_hpp */
