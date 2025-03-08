#include "apu.h"

APU::APU()
{
    m_frameCounter = std::make_unique<FrameCounter>(m_pulseChannel, m_triangleChannel);
    m_pulseChannel[0].SetChannelNumber(1);
    m_pulseChannel[1].SetChannelNumber(2);
}

APU::~APU()
{
}

void APU::Clock()
{
    m_frameCounter->Clock();

    m_pulseChannel[0].Clock();
    m_pulseChannel[1].Clock();
    m_triangleChannel.Clock();

    float pulseSample = 95.88F / ((8128.0F / (m_pulseChannel[0].Sample() + m_pulseChannel[1].Sample())) + 100.0F);
    float tndSample = 159.79F / ((1.0F / ((m_triangleChannel.Sample() / 8227.0F) /* Add other channels here */)) + 100.0F);
    float mixedSample = pulseSample + tndSample;

    m_sampleBuffer.push_back(mixedSample * AudioConstants::MASTER_VOLUME);
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
        m_pulseChannel[0].UpdateSweepSettings(data);
        break;
    case 0x4002:
        m_pulseChannel[0].SetTimerLow(data);
        break;
    case 0x4003:
        m_pulseChannel[0].SetTimerHigh(data & 0x07);
        m_pulseChannel[0].SetLengthCounter(data >> 3);
        m_pulseChannel[0].RestartEnvelope();
        m_pulseChannel[0].RestartSequencer();
        break;
    case 0x4004:
        m_pulseChannel[1].SetDuty((data & 0xC0) >> 6);
        m_pulseChannel[1].SetInfinitePlayFlag(data & 0x20);
        m_pulseChannel[1].SetConstantVolumeFlag(data & 0x10);
        m_pulseChannel[1].SetVolume(data & 0x0F);
        break;
    case 0x4005:
        m_pulseChannel[1].UpdateSweepSettings(data);
        break;
    case 0x4006:
        m_pulseChannel[1].SetTimerLow(data);
        break;
    case 0x4007:
        m_pulseChannel[1].SetTimerHigh(data & 0x07);
        m_pulseChannel[1].SetLengthCounter(data >> 3);
        m_pulseChannel[1].RestartEnvelope();
        m_pulseChannel[1].RestartSequencer();
        break;
    case 0x4008:
        m_triangleChannel.SetControlFlag(data & 0x80);
        m_triangleChannel.SetLinearCounterReloadValue(data & 0x7F);
        break;
    case 0x4009:
        // Unused but assigned to the triangle channel
        break;
    case 0x400A:
        m_triangleChannel.SetTimerLow(data);
        break;
    case 0x400B:
        m_triangleChannel.SetTimerHigh(data & 0x07);
        m_triangleChannel.SetLengthCounter(data >> 3);
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
        m_pulseChannel[0].SetEnabled(data & 0x01);
        m_pulseChannel[1].SetEnabled(data & 0x02);
        m_triangleChannel.SetEnabled(data & 0x04);
        break;
    case 0x4017:
        m_frameCounter->SetFiveStepMode(data & 0x80);
        break;
    default:
        Logger::GetInstance().Warn(std::format("unexpected write to APU at address {}", static_cast<int>(address)));
        break;
    }
}
