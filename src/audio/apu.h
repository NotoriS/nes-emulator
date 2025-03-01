#pragma once

#include <cstdint>
#include <vector>

#include "frame-counter.h"
#include "pulse-wave-generator.h"

class APU
{
    std::vector<float> m_sampleBuffer;

    FrameCounter m_frameCounter;

    PulseWaveGenerator m_pulseChannelOne;
    PulseWaveGenerator m_pulseChannelTwo;

public:
    APU();
    ~APU();

    void Clock();

    uint8_t Read(uint16_t address);
    void Write(uint16_t address, uint8_t data);

    std::vector<float>& GetBuffer() { return m_sampleBuffer; }
};