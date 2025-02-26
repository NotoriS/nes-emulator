#include "apu.h"

APU::APU()
{
}

APU::~APU()
{
}

void APU::Clock()
{
    m_pulseChannelOne.Clock();
    m_pulseChannelTwo.Clock();

    float pulseSample = 95.88 / ((8128.0 / (m_pulseChannelOne.Sample() + m_pulseChannelTwo.Sample())) + 100.0);
    m_sampleBuffer.push_back(pulseSample);
}

uint8_t APU::Read(uint16_t address)
{
    return 0;
}

void APU::Write(uint16_t address, uint8_t data)
{
    switch (address)
    {
    case 0x4000:
        m_pulseChannelOne.SetDuty((data & 0xC0) >> 6);
        break;
    case 0x4001:
        break;
    case 0x4002:
        m_pulseChannelOne.SetTimerLow(data);
        break;
    case 0x4003:
        m_pulseChannelOne.SetTimerHigh(data & 0x07);
        break;
    case 0x4004:
        m_pulseChannelTwo.SetDuty((data & 0xC0) >> 6);
        break;
    case 0x4005:
        break;
    case 0x4006:
        m_pulseChannelTwo.SetTimerLow(data);
        break;
    case 0x4007:
        m_pulseChannelTwo.SetTimerHigh(data & 0x07);
        break;
    case 0x4008:
        break;
    case 0x4009:
        break;
    case 0x400A:
        break;
    case 0x400B:
        break;
    case 0x400C:
        break;
    case 0x400D:
        break;
    case 0x400E:
        break;
    case 0x400F:
        break;
    case 0x4010:
        break;
    case 0x4011:
        break;
    case 0x4012:
        break;
    case 0x4013:
        break;
    case 0x4015:
        break;
    case 0x4017:
        break;
    default:
        Logger::GetInstance().Warn("unexpected write to APU at address " + static_cast<int>(address));
        break;
    }
}
