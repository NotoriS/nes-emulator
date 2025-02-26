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
    PulseWaveGenerator() {}
    ~PulseWaveGenerator() {}

    void Clock()
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

    float Sample() const
    {
        if (m_sequence & (0x80 >> m_sequenceStep)) return 1.0;
        else return 0.0;
    }

    void SetDuty(uint8_t duty)
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

    void SetTimerLow(uint8_t timerLow)
    {
        m_maxTimerValue &= 0xFF00;
        m_maxTimerValue |= timerLow;
    }
    void SetTimerHigh(uint8_t timerHigh)
    {
        m_maxTimerValue &= 0x00FF;
        m_maxTimerValue |= (timerHigh << 8);
    }
};