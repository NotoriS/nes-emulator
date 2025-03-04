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
        m_timer = m_timerPeriod;
        m_sequenceStep = (m_sequenceStep + 1) % 8;
    }
    else
    {
        m_timer--;
    }
}

float PulseWaveGenerator::Sample() const
{
    if (!m_enabled) return 0.0F;
    if (!(m_sequence & (0x80 >> m_sequenceStep))) return 0.0F;
    if (m_sweepTargetPeriod > 0x7FF) return 0.0F;
    if (m_lengthCounter == 0) return 0.0F;
    if (m_timerPeriod < 8) return 0.0F;
    
    return m_constantVolume ? m_volume : m_envelope.GetVolume();
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
    m_timerPeriod &= 0xFF00;
    m_timerPeriod |= timerLow;

    CalculateSweepTargetPeriod();
}

void PulseWaveGenerator::SetTimerHigh(uint8_t timerHigh)
{
    m_timerPeriod &= 0x00FF;
    m_timerPeriod |= (timerHigh << 8);

    CalculateSweepTargetPeriod();
}

void PulseWaveGenerator::SetLengthCounter(uint8_t lookupIndex)
{
    m_lengthCounter = LENGTH_COUNTER_LOOKUP[lookupIndex];
    m_sequenceStep = 0;
}

void PulseWaveGenerator::DecrementLengthCounter()
{
    if (m_lengthCounter > 0 && !m_infinitePlay && m_enabled) m_lengthCounter--;
}

void PulseWaveGenerator::UpdateSweepSettings(uint8_t settings)
{
    m_sweepEnabled = settings & 0x80;
    m_sweepPeriod = (settings & 0x70) >> 4;
    m_sweepNegateFlag = settings & 0x08;
    m_sweepShiftCount = settings & 0x07;

    m_sweepReloadFlag = true;
    CalculateSweepTargetPeriod();
}

void PulseWaveGenerator::CalculateSweepTargetPeriod()
{
    int16_t delta = m_timerPeriod >> m_sweepShiftCount;
    if (m_sweepNegateFlag) delta *= -1;
    if (m_sweepNegateFlag && m_channelNumber == 1) delta--;

    int16_t targetPeriod = static_cast<int16_t>(m_timerPeriod) + delta;
    if (targetPeriod < 0) targetPeriod = 0;
    m_sweepTargetPeriod = static_cast<uint16_t>(targetPeriod);
}

void PulseWaveGenerator::ClockSweepUnit()
{
    if (m_sweepDivider == 0 && m_sweepEnabled && m_sweepShiftCount != 0)
    {
        bool sweepIsMuting = m_timerPeriod < 8 || m_sweepTargetPeriod > 0x7FF;
        if (!sweepIsMuting)
        {
            m_timerPeriod = m_sweepTargetPeriod;
            CalculateSweepTargetPeriod();
        }
    }

    if (m_sweepDivider == 0 || m_sweepReloadFlag)
    {
        m_sweepDivider = m_sweepPeriod;
        m_sweepReloadFlag = false;
    }
    else
    {
        m_sweepDivider--;
    }
}
