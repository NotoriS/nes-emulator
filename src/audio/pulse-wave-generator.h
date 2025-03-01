#pragma once

#include <cstdint>

#include "../debug/logger.h"

class PulseWaveGenerator
{
    uint16_t m_maxTimerValue = 0;
    uint16_t m_timer = 0;

    uint8_t m_sequence = 0;
    uint8_t m_sequenceStep = 0;

    bool m_isOddClockCycle = true;

public:
    PulseWaveGenerator();
    ~PulseWaveGenerator();

    void Clock();
    float Sample() const;

    void SetDuty(uint8_t duty);
    void SetTimerLow(uint8_t timerLow);
    void SetTimerHigh(uint8_t timerHigh);
};