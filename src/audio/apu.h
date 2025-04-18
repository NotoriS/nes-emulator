#pragma once

#include <cstdint>
#include <vector>
#include <memory>
#include <format>

#include "frame-counter.h"
#include "pulse-wave-generator.h"
#include "triangle-wave-generator.h"
#include "noise-generator.h"

class APU
{
    std::vector<float> m_sampleBuffer;

    std::unique_ptr<FrameCounter> m_frameCounter;

    PulseWaveGenerator m_pulseChannel[2];
    TriangleWaveGenerator m_triangleChannel;
    NoiseGenerator m_noiseChannel;

public:
    APU();
    ~APU();

    void Clock();

    uint8_t Read(uint16_t address);
    void Write(uint16_t address, uint8_t data);

    std::vector<float>& GetBuffer() { return m_sampleBuffer; }
};