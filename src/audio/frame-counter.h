#pragma once

#include <cstdint>

#include "audio-constants.h"

class FrameCounter
{
    static constexpr double FOUR_STEP_TICK_THRESHOLD = AudioConstants::CLOCK_RATE / 60.0 / 4.0;
    static constexpr double FIVE_STEP_TICK_THRESHOLD = AudioConstants::CLOCK_RATE / 60.0 / 5.0;

    bool m_fiveStepModeEnabled = false;
    uint8_t m_frameStep = 0; // Value 1-5 holding most recent step given the current mode
    double m_clockAccumulator = 0;

public:
    FrameCounter();
    ~FrameCounter();

    void Clock();
    void SetFiveStepMode(bool value);

private:
    void FourStepTick();
    void FiveStepTick();

    void TickLengthCounterAndSweep();
    void TickEnvelopeAndLinearCounter();
};