#include "mapper-001.h"

Mapper001::Mapper001(std::vector<uint8_t>& prgRom, std::vector<uint8_t>& chrRom) 
    : Mapper(prgRom, chrRom), m_prgRam(0x2000)
{
}

uint8_t Mapper001::CpuRead(uint16_t address)
{
    if (address < 0x6000)
    {
        Logger::GetInstance().Warn("unexpected CPU read in mapper 1 below 0x6000");
        return 0;
    }

    if (address >= 0x6000 && address < 0x8000) return m_prgRam[address & 0x1FFF];

    address -= 0x8000;
    const uint8_t prgRomBankMode = (m_controlReg >> 2) & 3;
    uint32_t translatedAddress;

    if (prgRomBankMode == 2) // Fix first 16 KB bank (0x8000 - 0xBFFF)
    {
        if (address < 0x4000) translatedAddress = address;
        else translatedAddress = (m_prgBankReg & 0b01111) * 0x4000 + (address & 0x3FFF);
    }
    else if (prgRomBankMode == 3) // Fix second 16 KB bank (0xC000 - 0xFFFF)
    {
        if (address >= 0x4000) translatedAddress = (m_prgRom.size() - 0x4000) + (address & 0x3FFF);
        else translatedAddress = (m_prgBankReg & 0b01111) * 0x4000 + address;
    }
    else // Use one 32 KB bank
    {
        translatedAddress = ((m_prgBankReg & 0b01111) >> 1) * 0x8000 + address;
    }

    return m_prgRom[translatedAddress];
}

void Mapper001::CpuWrite(uint16_t address, uint8_t data)
{
    if (address < 0x6000)
    {
        Logger::GetInstance().Warn("unexpected CPU write in mapper 1 below 0x6000");
        return;
    }

    if (address >= 0x6000 && address < 0x8000)
    {
        m_prgRam[address & 0x1FFF] = data;
        return;
    }

    // Reset bit set
    if (data & 0x80)
    {
        m_loadReg = 0b10000;
        m_controlReg |= 0b01100;
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
    if (address >= 0x2000)
    {
        Logger::GetInstance().Warn("PPU tried to write to the cartridge at an unexpected address " + Logger::DecmialToHex(address));
        return 0;
    }

    uint32_t translatedAddress;
    if (m_controlReg & 0b10000) // Split 4 KB bank mode
    {
        if (address < 0x1000) translatedAddress = m_chrBankZeroReg * 0x1000 + address;
        else translatedAddress = m_chrBankOneReg * 0x1000 + (address & 0x0FFF);
    }
    else // Single 8 KB bank mode
    {
        translatedAddress = (m_chrBankZeroReg >> 1) * 0x2000 + address;
    }

    return m_chrRom[translatedAddress];
}

void Mapper001::PpuWrite(uint16_t address, uint8_t data)
{
    if (address >= 0x0000 && address <= 0x1FFF)
    {
        m_chrRom[address] = data;
        return;
    }

    Logger::GetInstance().Warn("PPU tried to write to the cartridge at an unexpected address " + Logger::DecmialToHex(address));
}

std::optional<MirrorMode> Mapper001::GetMirrorMode()
{
    uint8_t nametableArrangement = m_controlReg & 0b00011;
    switch (nametableArrangement)
    {
    case 0:
        return MirrorMode::OneScreenLow;
    case 1:
        return MirrorMode::OneScreenHigh;
    case 2:
        return MirrorMode::Vertical;
    case 3:
        return MirrorMode::Horizontal;
    }

    throw std::runtime_error("no mirror mode returned by MMC1.");
}
