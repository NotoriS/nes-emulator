#include "ppu-bus.h"

PpuBus::PpuBus()
{
}

PpuBus::~PpuBus()
{
}

uint8_t PpuBus::Read(uint16_t address)
{
    if (address >= 0x0000 && address <= 0x1FFF)
    {
        return m_cartridge->PpuRead(address);
    }

    std::cerr << "Failed to read from PPU bus at address: 0x" << std::hex << address << std::endl;
    return 0;
}

void PpuBus::Write(uint16_t address, uint8_t data)
{
    if (address >= 0x0000 && address <= 0x1FFF)
    {
        m_cartridge->PpuWrite(address, data);
        return;
    }

    std::cerr << "Failed to write to CPU bus at address: 0x" << std::hex << address << std::endl;
}
