#pragma once

#include <cstdint>
#include <vector>
#include <memory>
#include <format>

#include "frame-counter.h"
#include "pulse-wave-generator.h"

class APU
{
    std::vector<float> m_sampleBuffer;

    std::unique_ptr<FrameCounter> m_frameCounter;

    PulseWaveGenerator m_pulseChannel[2];

public:
    APU();
    ~APU();

    void Clock();

    uint8_t Read(uint16_t address);
    void Write(uint16_t address, uint8_t data);

    std::vector<float>& GetBuffer() { return m_sampleBuffer; }
};