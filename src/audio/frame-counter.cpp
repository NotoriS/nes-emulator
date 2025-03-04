#include "frame-counter.h"

FrameCounter::FrameCounter(PulseWaveGenerator (&pulseChannel)[2])
    : m_pulseChannel(pulseChannel)
{
}

FrameCounter::~FrameCounter()
{
}

void FrameCounter::Clock()
{
    m_clockAccumulator += 1;

    if (!m_fiveStepModeEnabled && m_clockAccumulator >= FOUR_STEP_TICK_THRESHOLD)
    {
        m_clockAccumulator -= FOUR_STEP_TICK_THRESHOLD;

        if (m_frameStep == 4) m_frameStep = 1;
        else m_frameStep++;

        FourStepTick();
    }
    else if (m_fiveStepModeEnabled && m_clockAccumulator >= FIVE_STEP_TICK_THRESHOLD)
    {
        m_clockAccumulator -= FIVE_STEP_TICK_THRESHOLD;

        if (m_frameStep == 5) m_frameStep = 1;
        else m_frameStep++;

        FiveStepTick();
    }
}

void FrameCounter::SetFiveStepMode(bool value)
{
    m_clockAccumulator = 0;
    m_fiveStepModeEnabled = value;

    if (m_fiveStepModeEnabled)
    {
        TickLengthCounterAndSweep();
        TickEnvelopeAndLinearCounter();
    }
}

void FrameCounter::FourStepTick()
{
    if (m_frameStep == 2 || m_frameStep == 4) TickLengthCounterAndSweep();
    TickEnvelopeAndLinearCounter();
}

void FrameCounter::FiveStepTick()
{
    if (m_frameStep == 2 || m_frameStep == 5) TickLengthCounterAndSweep();
    if (m_frameStep != 4) TickEnvelopeAndLinearCounter();
}

void FrameCounter::TickLengthCounterAndSweep()
{
    m_pulseChannel[0].DecrementLengthCounter();
    m_pulseChannel[1].DecrementLengthCounter();
    
    m_pulseChannel[0].ClockSweepUnit();
    m_pulseChannel[1].ClockSweepUnit();
}

void FrameCounter::TickEnvelopeAndLinearCounter()
{
    m_pulseChannel[0].ClockEnvelope();
    m_pulseChannel[1].ClockEnvelope();
}
