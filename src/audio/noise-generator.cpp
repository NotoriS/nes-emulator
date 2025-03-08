#include "noise-generator.h"

NoiseGenerator::NoiseGenerator()
{
}

NoiseGenerator::~NoiseGenerator()
{
}

void NoiseGenerator::Clock()
{
    if (m_timer == 0)
    {
        m_timer = m_timerPeriod;
        
        uint8_t selectedBit = m_modeFlag ? 6 : 1;
        bool feedback = (m_linearFeedbackShiftRegister & 1)
            ^ ((m_linearFeedbackShiftRegister & (1 << selectedBit)) >> selectedBit);

        m_linearFeedbackShiftRegister >>= 1;

        if (feedback)
            m_linearFeedbackShiftRegister |= 0x4000;
        else
            m_linearFeedbackShiftRegister &= ~0x4000;
    }
    else
    {
        m_timer--;
    }
}

float NoiseGenerator::Sample() const
{
    if (m_linearFeedbackShiftRegister & 1) return 0.0F;
    if (m_lengthCounter == 0) return 0.0F;

    return m_constantVolume ? m_volume : m_envelope.GetVolume();
}

void NoiseGenerator::SetEnabled(bool value)
{
    m_enabled = value;
    if (!m_enabled) m_lengthCounter = 0;
}

void NoiseGenerator::SetTimerPeriod(uint8_t lookupIndex)
{
    m_timerPeriod = NOISE_TIMER_PEROID_LOOKUP[lookupIndex];
}

void NoiseGenerator::SetLengthCounter(uint8_t lookupIndex)
{
    if (!m_enabled) return;
    m_lengthCounter = LENGTH_COUNTER_LOOKUP[lookupIndex];
}

void NoiseGenerator::ClockLengthCounter()
{
    if (m_lengthCounter > 0 && !m_infinitePlay) m_lengthCounter--;
}
