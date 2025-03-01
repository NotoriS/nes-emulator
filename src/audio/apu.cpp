#include "apu.h"

APU::APU()
{
}

APU::~APU()
{
}

void APU::Clock()
{
    m_frameCounter.Clock();

    m_pulseChannel[0].Clock();
    m_pulseChannel[1].Clock();

    float pulseSample = 95.88F / ((8128.0F / (m_pulseChannel[0].Sample() + m_pulseChannel[1].Sample())) + 100.0F);
    m_sampleBuffer.push_back(pulseSample * AudioConstants::MASTER_VOLUME);
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
        m_pulseChannel[0].SetDuty((data & 0xC0) >> 6);
        m_pulseChannel[0].SetInfinitePlayFlag(data & 0x20);
        m_pulseChannel[0].SetConstantVolumeFlag(data & 0x10);
        m_pulseChannel[0].SetVolume(data & 0x0F);
        break;
    case 0x4001:
        break;
    case 0x4002:
        m_pulseChannel[0].SetTimerLow(data);
        break;
    case 0x4003:
        m_pulseChannel[0].SetTimerHigh(data & 0x07);
        break;
    case 0x4004:
        m_pulseChannel[1].SetDuty((data & 0xC0) >> 6);
        m_pulseChannel[1].SetInfinitePlayFlag(data & 0x20);
        m_pulseChannel[1].SetConstantVolumeFlag(data & 0x10);
        m_pulseChannel[1].SetVolume(data & 0x0F);
        break;
    case 0x4005:
        break;
    case 0x4006:
        m_pulseChannel[1].SetTimerLow(data);
        break;
    case 0x4007:
        m_pulseChannel[1].SetTimerHigh(data & 0x07);
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
