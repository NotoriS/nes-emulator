#include "mapper-000.h"

uint8_t Mapper000::CpuRead(uint16_t address)
{
    if (address >= 0x8000 && address <= 0xFFFF) {
        uint16_t offset = address - 0x8000;
        return m_prgRom[offset % m_prgRom.size()];
    }

    Logger::GetInstance().Warn("CPU tried to read from the cartridge at an unexpected address " + Logger::DecmialToHex(address));
    return 0;
}

void Mapper000::CpuWrite(uint16_t address, uint8_t data)
{
    Logger::GetInstance().Warn("CPU tried to write to the cartridge at an unexpected address " + Logger::DecmialToHex(address));
}

uint8_t Mapper000::PpuRead(uint16_t address)
{
    if (address >= 0x0000 && address <= 0x1FFF)
        return m_chrRom[address];

    Logger::GetInstance().Warn("PPU tried to read from the cartridge at an unexpected address " + Logger::DecmialToHex(address));
    return 0;
}

void Mapper000::PpuWrite(uint16_t address, uint8_t data)
{
    if (address >= 0x0000 && address <= 0x1FFF)
    {
        m_chrRom[address] = data;
        return;
    }

    Logger::GetInstance().Warn("PPU tried to write to the cartridge at an unexpected address " + Logger::DecmialToHex(address));
}
