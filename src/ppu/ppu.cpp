#include "ppu.h"

PPU::PPU(std::shared_ptr<IBus> bus) : m_bus(bus)
{
}

PPU::~PPU()
{
}

void PPU::Clock()
{
    // TODO
}

void PPU::Reset()
{
    // TODO
}

uint8_t PPU::Read(uint16_t address)
{
    // TODO
    return 0;
}

void PPU::Write(uint16_t address, uint8_t data)
{
    // TODO
}
