#pragma once

#include <cstdint>

#include "length-counter-lookup.h"
#include "../debug/logger.h"

class PulseWaveGenerator
{
    uint16_t m_maxTimerValue = 0;
    uint16_t m_timer = 0;

    uint8_t m_lengthCounter = 0;

    uint8_t m_sequence = 0;
    uint8_t m_sequenceStep = 0;

    bool m_infinitePlay = false;
    bool m_constantVolume = true;

    uint8_t m_volume = 0;

    bool m_isOddClockCycle = true;

public:
    PulseWaveGenerator();
    ~PulseWaveGenerator();

    void Clock();
    float Sample() const;

    void SetDuty(uint8_t duty);
    inline void SetInfinitePlayFlag(bool value) { m_infinitePlay = value; }
    inline void SetConstantVolumeFlag(bool value) { m_constantVolume = value; }
    inline void SetVolume(uint8_t volume) { m_volume = volume; }
    void SetTimerLow(uint8_t timerLow);
    void SetTimerHigh(uint8_t timerHigh);
    void SetLengthCounter(uint8_t lookupIndex);

    void DecrementLengthCounter();
};