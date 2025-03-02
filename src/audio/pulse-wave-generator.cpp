#include "pulse-wave-generator.h"

PulseWaveGenerator::PulseWaveGenerator()
{
}

PulseWaveGenerator::~PulseWaveGenerator()
{
}

void PulseWaveGenerator::Clock()
{
    m_isOddClockCycle = !m_isOddClockCycle;
    if (m_isOddClockCycle) return;

    if (m_timer == 0)
    {
        m_timer = m_maxTimerValue;
        m_sequenceStep = (m_sequenceStep + 1) % 8;
    }
    else
    {
        m_timer--;
    }
}

float PulseWaveGenerator::Sample() const
{
    if (!(m_sequence & (0x80 >> m_sequenceStep))) return 0.0F;
    if (m_lengthCounter == 0) return 0.0F;
    
    return m_volume;
}

void PulseWaveGenerator::SetDuty(uint8_t duty)
{
    switch (duty)
    {
    case 0: m_sequence = 0b01000000; break;
    case 1: m_sequence = 0b01100000; break;
    case 2: m_sequence = 0b01111000; break;
    case 3: m_sequence = 0b10011111; break;
    default: Logger::GetInstance().Warn("unexpected duty value passed to pulse wave generator"); break;
    }
}

void PulseWaveGenerator::SetTimerLow(uint8_t timerLow)
{
    m_maxTimerValue &= 0xFF00;
    m_maxTimerValue |= timerLow;
}

void PulseWaveGenerator::SetTimerHigh(uint8_t timerHigh)
{
    m_maxTimerValue &= 0x00FF;
    m_maxTimerValue |= (timerHigh << 8);
}

void PulseWaveGenerator::SetLengthCounter(uint8_t lookupIndex)
{
    m_lengthCounter = LENGTH_COUNTER_LOOKUP[lookupIndex];
    m_sequenceStep = 0;
}

void PulseWaveGenerator::DecrementLengthCounter()
{
    if (m_lengthCounter > 0 && !m_infinitePlay) m_lengthCounter--;
}
