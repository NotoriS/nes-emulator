#pragma once

#include <cstdint>

class Envelope
{
    bool m_start = true;

    uint8_t m_divider = 0;
    uint8_t m_dividerReload = 0;

    uint8_t m_decayLevel = 0;

    bool m_loop = false;

public:
    Envelope() {}
    ~Envelope() {}

    void Restart() { m_start = true; }
    
    void Clock();

    uint8_t GetVolume() const { return m_decayLevel; }
    
    void SetPeriod(uint8_t period) { m_dividerReload = period; }
    void SetLoopMode(bool value) { m_loop = value; }

private:
    void ClockDivider();
    void ClockDecayLevel();
};