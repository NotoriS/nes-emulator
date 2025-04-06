#include "mapper-001.h"

Mapper001::Mapper001(std::vector<uint8_t>& prgRom, std::vector<uint8_t>& chrRom) : Mapper(prgRom, chrRom)
{
}

uint8_t Mapper001::CpuRead(uint16_t address)
{
    return 0;
}

void Mapper001::CpuWrite(uint16_t address, uint8_t data)
{
}

uint8_t Mapper001::PpuRead(uint16_t address)
{
    return 0;
}

void Mapper001::PpuWrite(uint16_t address, uint8_t data)
{
}

std::optional<MirrorMode> Mapper001::GetMirrorMode()
{
    return {};
}
