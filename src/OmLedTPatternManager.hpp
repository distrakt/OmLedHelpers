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
#define PM_HISTORY_SIZE 20
    int msHistoryIx = 0;
    int msHistory[PM_HISTORY_SIZE];

    // the cross fade pattern is the OLD pattern.
    // we set the new one immediately so the parameters are current.
    unsigned int crossfadeMs = 0;
    unsigned int crossfadeMsSoFar = 0;
    PATTERNT *crossfadePattern = NULL;
    int crossfadePatternIndex;
    STRIPT *crossfadeStrip = NULL;

    PATTERNT *addPattern(PATTERNT *pattern)
    {
        this->patterns.push_back(pattern);
        return pattern;
    }

    void initPatterns(int ledCount)
    {
        this->ledCount = ledCount;
        for(PATTERNT *pattern : this->patterns)
        {
            pattern->init(ledCount);
        }

        for(int ix = 0; ix < PM_HISTORY_SIZE; ix++)
            this->msHistory[ix] = 0;
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
        if(nextPatternIx == this->currentPatternIndex)
            return;
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
        this->msHistoryIx = (this->msHistoryIx + 1) % PM_HISTORY_SIZE;
        this->msHistory[this->msHistoryIx] = ms;

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

    void getPerformanceInfo(int &millisecondsPerFrameOut, int &framesPerSecondOut)
    {
        float result = 0;
        for (int ix = 0; ix < PM_HISTORY_SIZE; ix++)
            result += msHistory[ix];
        result = result / PM_HISTORY_SIZE;
        if (result < 1)
            result = 1;
        millisecondsPerFrameOut = result + 0.5;
        framesPerSecondOut = (1000.0 / result) + 0.5;
    }


};

typedef OmLedTPatternManager<OmLed8> OmLed8PatternManager;
typedef OmLedTPatternManager<OmLed16> OmLed16PatternManager;

#endif /* OmLedTPatternManager_hpp */
