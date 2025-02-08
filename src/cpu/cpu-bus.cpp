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
    else if (address == 0x4016) // Read controller 1 input
    {
        uint8_t result = (m_controllerOneShifter & 0x80) != 0;
        m_controllerOneShifter <<= 1;
        return result;
    }
    else if (address == 0x4017) // Read controller 2 input
    {
        uint8_t result = (m_controllerTwoShifter & 0x80) != 0;
        m_controllerTwoShifter <<= 1;
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
    else if (address == 0x4016) // Poll controller 1 state
    {
        m_controllerOneShifter = *m_controllerOneState;
        return;
    }
    else if (address == 0x4017) // Poll controller 2 state
    {
        m_controllerTwoShifter = *m_controllerTwoState;
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
