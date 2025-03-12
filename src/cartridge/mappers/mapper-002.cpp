#include "mapper-002.h"

Mapper002::Mapper002(std::vector<uint8_t>& prgRom, std::vector<uint8_t>& chrRom) : Mapper(prgRom, chrRom)
{
    m_dynamicBank = 0;
    m_fixedBank = (prgRom.size() / 16 / 1024) - 1;
}

uint8_t Mapper002::CpuRead(uint16_t address)
{
    uint16_t offset;

    if (address >= 0x8000 && address < 0xC000)
    {
        offset = address - 0x8000;
        return m_prgRom[(m_dynamicBank * 16 * 1024) + offset];
    }
    else if (address >= 0xC000)
    {
        offset = address - 0xC000;
        return m_prgRom[(m_fixedBank * 16 * 1024) + offset];
    }

    Logger::GetInstance().Warn("CPU tried to read from the cartridge at an unexpected address " + Logger::DecmialToHex(address));
    return 0;
}

void Mapper002::CpuWrite(uint16_t address, uint8_t data)
{
    if (address >= 0x8000)
    {
        m_dynamicBank = data & 0x0F;
        return;
    }

    Logger::GetInstance().Warn("CPU tried to write to the cartridge at an unexpected address " + Logger::DecmialToHex(address));
}

uint8_t Mapper002::PpuRead(uint16_t address)
{
    if (address >= 0x0000 && address <= 0x1FFF)
        return m_chrRom[address];

    Logger::GetInstance().Warn("PPU tried to read from the cartridge at an unexpected address " + Logger::DecmialToHex(address));
    return 0;
}

void Mapper002::PpuWrite(uint16_t address, uint8_t data)
{
    if (address >= 0x0000 && address <= 0x1FFF)
    {
        m_chrRom[address] = data;
        return;
    }

    Logger::GetInstance().Warn("PPU tried to write to the cartridge at an unexpected address " + Logger::DecmialToHex(address));
}
