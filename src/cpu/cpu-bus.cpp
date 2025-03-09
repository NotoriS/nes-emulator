#include "cpu-bus.h"

CpuBus::CpuBus()
{
    m_memory.fill(0);
}

CpuBus::~CpuBus()
{
}

uint8_t CpuBus::Read(uint16_t address)
{
    if (address >= 0x0000 && address <= 0x1FFF) // Read from RAM
    {
        return m_memory[address & 0x07FF];
    }
    else if (address >= 0x2000 && address <= 0x3FFF) // Read from PPU registers
    {
        return m_ppu->Read(address & 0x2007);
    }
    else if (address == 0x4015) // Read from APU registers
    {
        return m_apu->Read(address);
    }
    else if (address == 0x4016) // Read controller 1 input
    {
        if (m_controllerPollingEnabled) PollControllerState();
        uint8_t result = (m_controllerOneShifter & 0x80) != 0;
        m_controllerOneShifter <<= 1;
        m_controllerOneShifter |= 0x01;
        return result;
    }
    else if (address == 0x4017) // Read controller 2 input
    {
        if (m_controllerPollingEnabled) PollControllerState();
        uint8_t result = (m_controllerTwoShifter & 0x80) != 0;
        m_controllerTwoShifter <<= 1;
        m_controllerTwoShifter |= 0x01;
        return result;
    }
    else if (address >= 0x4020 && address <= 0xFFFF) // Read from cartridge
    {
        return m_cartridge->CpuRead(address);
    }

    Logger::GetInstance().Warn("failed to read from CPU bus at address: " + Logger::DecmialToHex(address));
    return 0;
}

void CpuBus::Write(uint16_t address, uint8_t data)
{
    if (address >= 0x0000 && address <= 0x1FFF) // Write to RAM
    {
        m_memory[address & 0x07FF] = data;
        return;
    }
    else if (address >= 0x2000 && address <= 0x3FFF) // Write to PPU registers
    {
        m_ppu->Write(address & 0x2007, data);
        return;
    }
    else if ((address >= 0x4000 && address <= 0x4013) || address == 0x4015 || address == 0x4017) // Write to APU registers
    {
        m_apu->Write(address, data);
        return;
    }
    else if (address == 0x4014) // Trigger DMA
    {
        m_dmaPage = data;
        m_dmaReady = true;
        return;
    }
    else if (address == 0x4016) // Set controller polling mode
    {
        m_controllerPollingEnabled = data & 0x01;
        if (m_controllerPollingEnabled) PollControllerState();
        return;
    }
    else if (address >= 0x4020 && address <= 0xFFFF) // Write to cartridge
    {
        m_cartridge->CpuWrite(address, data);
        return;
    }

    Logger::GetInstance().Warn("failed to write to CPU bus at address: " + Logger::DecmialToHex(address));
}

void CpuBus::ConnectControllers(std::shared_ptr<uint8_t> controllerOneState, std::shared_ptr<uint8_t> controllerTwoState)
{
    m_controllerOneState = controllerOneState;
    m_controllerTwoState = controllerTwoState;
}

bool CpuBus::TryDirectMemoryAccess(bool cycleIsOdd)
{
    if (!m_dmaReady) return false;

    if (m_dmaInProgress)
    {
        if (cycleIsOdd)
        {
            m_ppu->WriteByteToOAM(m_dmaAddress, m_dmaDataBuffer);
            m_dmaAddress++;

            if (m_dmaAddress == 0)
            {
                m_dmaReady = false;
                m_dmaInProgress = false;
            }
        }
        else
        {
            m_dmaDataBuffer = Read((m_dmaPage << 8) | m_dmaAddress);
        }
    }
    else
    {
        if (cycleIsOdd) m_dmaInProgress = true;
    }

    return true;
}

void CpuBus::PollControllerState()
{
    m_controllerOneShifter = *m_controllerOneState;
    m_controllerTwoShifter = *m_controllerTwoState;

    // Prevent conflicting d-pad inputs (left + right or up + down)
    if ((m_controllerOneShifter & 0x03) == 0x03)
    {
        m_controllerOneShifter &= ~0x03;
    }
    if ((m_controllerOneShifter & 0x0C) == 0x0C)
    {
        m_controllerOneShifter &= ~0x0C;
    }
    if ((m_controllerTwoShifter & 0x03) == 0x03)
    {
        m_controllerTwoShifter &= ~0x03;
    }
    if ((m_controllerTwoShifter & 0x0C) == 0x0C)
    {
        m_controllerTwoShifter &= ~0x0C;
    }
}
