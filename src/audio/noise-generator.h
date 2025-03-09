#pragma once

#include <cstdint>

#include "envelope.h"
#include "length-counter-lookup.h"

const uint16_t NOISE_TIMER_PEROID_LOOKUP[]
    { 4, 8, 16, 32, 64, 96, 128, 160, 202, 
    254, 380, 508, 762, 1016, 2034, 4068 };

class NoiseGenerator
{
    bool m_enabled = false;

    Envelope m_envelope;

    uint16_t m_timerPeriod = 0;
    uint16_t m_timer = 0;

    uint16_t m_linearFeedbackShiftRegister = 1;

    uint8_t m_lengthCounter = 0;

    bool m_modeFlag = false;
    bool m_infinitePlay = false;
    bool m_constantVolume = true;

    uint8_t m_volume = 0;

public:
    NoiseGenerator();
    ~NoiseGenerator();

    void Clock();
    float Sample() const;

    void SetEnabled(bool value);
    inline void SetInfinitePlayFlag(bool value) { m_infinitePlay = value; m_envelope.SetLoopMode(value); }
    inline void SetConstantVolumeFlag(bool value) { m_constantVolume = value; }
    inline void SetVolume(uint8_t volume) { m_volume = volume; m_envelope.SetPeriod(volume); }
    inline void SetMode(bool value) { m_modeFlag = value; }
    void SetTimerPeriod(uint8_t lookupIndex);
    void SetLengthCounter(uint8_t lookupIndex);

    void ClockEnvelope() { m_envelope.Clock(); }
    void RestartEnvelope() { m_envelope.Restart(); }

    void ClockLengthCounter();
};