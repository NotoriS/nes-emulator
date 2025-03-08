#pragma once

#include <cstdint>

#include "length-counter-lookup.h"

const float TRIANGLE_SEQUENCE[]
{
    15.0F, 14.0F, 13.0F, 12.0F, 11.0F, 10.0F, 9.0F, 8.0F, 7.0F, 6.0F, 5.0F, 4.0F, 3.0F, 2.0F, 1.0F, 0.0F,
    0.0F, 1.0F, 2.0F, 3.0F, 4.0F, 5.0F, 6.0F, 7.0F, 8.0F, 9.0F, 10.0F, 11.0F, 12.0F, 13.0F, 14.0F, 15.0F
};

class TriangleWaveGenerator
{
    bool m_enabled = false;

    uint16_t m_timer = 0;
    uint16_t m_timerPeriod = 0;

    uint8_t m_lengthCounter = 0;

    uint8_t m_linearCounter = 0;
    uint8_t m_linearCounterReloadValue = 0;
    bool m_linearCounterReloadFlag = false;

    bool m_controlFlag = false;

    uint8_t m_sequenceStep = 0;

public:
    TriangleWaveGenerator();
    ~TriangleWaveGenerator();

    void Clock();
    float Sample() const;

    void SetEnabled(bool value);
    void SetControlFlag(bool value) { m_controlFlag = value; }
    void SetLinearCounterReloadValue(uint8_t value) { m_linearCounterReloadValue = value; }
    void SetTimerLow(uint8_t timerLow);
    void SetTimerHigh(uint8_t timerHigh);
    void SetLengthCounter(uint8_t lookupIndex);

    void ClockLengthCounter();
    void ClockLinearCounter();
};