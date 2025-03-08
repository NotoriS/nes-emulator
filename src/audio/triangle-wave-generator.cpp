#include "triangle-wave-generator.h"

TriangleWaveGenerator::TriangleWaveGenerator()
{
}

TriangleWaveGenerator::~TriangleWaveGenerator()
{
}

void TriangleWaveGenerator::Clock()
{
    if (m_timer == 0)
    {
        m_timer = m_timerPeriod;

        if (m_linearCounter > 0 && m_lengthCounter > 0)
            m_sequenceStep = (m_sequenceStep + 1) % 32;
    }
    else
    {
        m_timer--;
    }
}

float TriangleWaveGenerator::Sample() const
{
    return TRIANGLE_SEQUENCE[m_sequenceStep];
}

void TriangleWaveGenerator::SetEnabled(bool value)
{
    m_enabled = value;
    if (!m_enabled) m_lengthCounter = 0;
}

void TriangleWaveGenerator::SetTimerLow(uint8_t timerLow)
{
    m_timerPeriod &= 0xFF00;
    m_timerPeriod |= timerLow;
}

void TriangleWaveGenerator::SetTimerHigh(uint8_t timerHigh)
{
    m_timerPeriod &= 0x00FF;
    m_timerPeriod |= (timerHigh << 8);

    // Side Effect
    m_linearCounterReloadFlag = true;
}

void TriangleWaveGenerator::SetLengthCounter(uint8_t lookupIndex)
{
    if (!m_enabled) return;
    m_lengthCounter = LENGTH_COUNTER_LOOKUP[lookupIndex];
}

void TriangleWaveGenerator::ClockLengthCounter()
{
    if (m_lengthCounter > 0 && !m_controlFlag) m_lengthCounter--;
}

void TriangleWaveGenerator::ClockLinearCounter()
{
    if (m_linearCounterReloadFlag)
        m_linearCounter = m_linearCounterReloadValue;
    else if (m_linearCounter > 0)
        m_linearCounter--;

    if (!m_controlFlag) m_linearCounterReloadFlag = false;
}
