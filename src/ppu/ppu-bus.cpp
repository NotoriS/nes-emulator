#include "ppu-bus.h"

PpuBus::PpuBus()
{
    m_paletteMemory.fill(0);
}

PpuBus::~PpuBus()
{
}

uint8_t PpuBus::Read(uint16_t address)
{
    if (address >= 0x0000 && address <= 0x1FFF) // Read from cartridge
    {
        return m_cartridge->PpuRead(address);
    }
    else if (address >= 0x3F00 && address <= 0x3FFF) // Read from palette RAM
    {
        address &= 0x1F;
        if ((address & 0x10) && address % 4 == 0) address &= 0xF;
        return m_paletteMemory[address];
    }

    std::cerr << "Failed to read from PPU bus at address: 0x" << std::hex << address << std::endl;
    return 0;
}

void PpuBus::Write(uint16_t address, uint8_t data)
{
    if (address >= 0x0000 && address <= 0x1FFF) // Write to cartridge
    {
        m_cartridge->PpuWrite(address, data);
        return;
    }
    else if (address >= 0x3F00 && address <= 0x3FFF) // Write to palette RAM
    {
        address &= 0x1F;
        if ((address & 0x10) && address % 4 == 0) address &= 0xF;
        m_paletteMemory[address] = data;
        return;
    }

    std::cerr << "Failed to write to CPU bus at address: 0x" << std::hex << address << std::endl;
}
