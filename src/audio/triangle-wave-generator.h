#pragma once

#include <cstdint>

const float TRIANGLE_SEQUENCE[]
{
    15.0F, 14.0F, 13.0F, 12.0F, 11.0F, 10.0F, 9.0F, 8.0F, 7.0F, 6.0F, 5.0F, 4.0F, 3.0F, 2.0F, 1.0F, 0.0F,
    0.0F, 1.0F, 2.0F, 3.0F, 4.0F, 5.0F, 6.0F, 7.0F, 8.0F, 9.0F, 10.0F, 11.0F, 12.0F, 13.0F, 14.0F, 15.0F
};

class TriangleWaveGenerator
{
    bool m_enabled = false;

    uint16_t m_timerPeriod = 0;
    uint16_t m_timer = 0;

    uint8_t m_lengthCounter = 0;

    uint8_t m_sequenceStep = 0;

public:
    TriangleWaveGenerator();
    ~TriangleWaveGenerator();
};