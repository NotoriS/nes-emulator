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
    // TODO: Add support for PRG RAM
    if (address < 0x8000)
    {
        Logger::GetInstance().Warn("A write to PRG RAMs address range was made, but it is not currently supported.");
        return;
    }

    // Reset bit set
    if (data & 0x80)
    {
        m_loadReg = 0b10000;
        return;
    }

    // Load register is ready for copy
    if (m_loadReg & 1)
    {
        uint8_t* targetReg;
        if (address >= 0x8000 && address < 0xA000) targetReg = &m_controlReg;
        else if (address >= 0xA000 && address < 0xC000) targetReg = &m_chrBankZeroReg;
        else if (address >= 0xC000 && address < 0xE000) targetReg = &m_chrBankOneReg;
        else if (address >= 0xE000) targetReg = &m_prgBankReg;
        else throw std::runtime_error("unexpected path reached when copying MMC1's load register.");

        *targetReg = (m_loadReg >> 1) & 0b01111;
        *targetReg |= (data & 1) << 4;
        m_loadReg = 0b10000;
        return;
    }

    // shift and load the load register with bit 0
    m_loadReg >>= 1;
    m_loadReg |= (data & 1) << 4;
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
