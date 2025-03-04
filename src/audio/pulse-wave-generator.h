#pragma once

#include <cstdint>

#include "envelope.h"
#include "length-counter-lookup.h"
#include "../debug/logger.h"

class PulseWaveGenerator
{
    bool m_enabled = false;

    Envelope m_envelope;

    uint16_t m_timerPeriod = 0;
    uint16_t m_timer = 0;

    uint8_t m_lengthCounter = 0;

    uint8_t m_sequence = 0;
    uint8_t m_sequenceStep = 0;

    bool m_infinitePlay = false;
    bool m_constantVolume = true;

    uint8_t m_volume = 0;

    bool m_isOddClockCycle = true;

    bool m_sweepEnabled = false;
    uint8_t m_sweepDivider = 0;
    uint8_t m_sweepPeriod = 0;
    uint8_t m_sweepShiftCount = 0;
    uint16_t m_sweepTargetPeriod = 0;
    bool m_sweepNegateFlag = false;
    bool m_sweepReloadFlag = false;

    uint8_t m_channelNumber = 0;

public:
    PulseWaveGenerator();
    ~PulseWaveGenerator();

    void Clock();
    float Sample() const;

    void SetEnabled(bool value) { m_enabled = value; }
    void SetDuty(uint8_t duty);
    inline void SetInfinitePlayFlag(bool value) { m_infinitePlay = value; m_envelope.SetLoopMode(value); }
    inline void SetConstantVolumeFlag(bool value) { m_constantVolume = value; }
    inline void SetVolume(uint8_t volume) { m_volume = volume; m_envelope.SetPeriod(volume); }
    void SetTimerLow(uint8_t timerLow);
    void SetTimerHigh(uint8_t timerHigh);
    void SetLengthCounter(uint8_t lookupIndex);
    void SetChannelNumber(uint8_t number) { m_channelNumber = number; }

    void ClockEnvelope() { m_envelope.Clock(); }
    void RestartEnvelope() { m_envelope.Restart(); }

    void DecrementLengthCounter();

    void UpdateSweepSettings(uint8_t settings);
    void CalculateSweepTargetPeriod();
    void ClockSweepUnit();
};