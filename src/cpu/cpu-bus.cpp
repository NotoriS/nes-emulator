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
    if (address >= 0x0000 && address <= 0x1FFF)
    {
        return m_memory[address & 0x07FF];
    }
    else if (address >= 0x4020 && address <= 0xFFFF)
    {
        return m_cartridge->CpuRead(address);
    }

    std::cerr << "Failed to read from CPU bus at address: 0x" << std::hex << address << std::endl;
    return 0;
}

void CpuBus::Write(uint16_t address, uint8_t data)
{
    if (address >= 0x0000 && address <= 0x1FFF)
    {
        m_memory[address & 0x07FF] = data;
        return;
    }
    else if (address >= 0x4020 && address <= 0xFFFF)
    {
        m_cartridge->CpuWrite(address, data);
        return;
    }

    std::cerr << "Failed to write to CPU bus at address: 0x" << std::hex << address << std::endl;
}
