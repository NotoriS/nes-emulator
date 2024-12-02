#pragma once

#include <cstdint>
#include <string>
#include <vector>

class Cartridge
{
public:
    Cartridge();
    ~Cartridge();

    void LoadROM(const std::string& filename);

private:
    struct RomHeader {
        char signature[4];      // Should be "NES\x1A"
        uint8_t prgRomSize;     // PRG ROM size in 16KB units
        uint8_t chrRomSize;     // CHR ROM size in 8KB units
        uint8_t flags6;         // Mapper, mirroring, battery, trainer
        uint8_t flags7;         // Mapper, VS/Playchoice, NES 2.0
        uint8_t prgRamSize;     // PRG RAM size in 8KB units (often 0)
        uint8_t flags9;         // TV system (rarely used extension)
        uint8_t flags10;        // TV system, PRG-RAM presence (unofficial, rarely used extension)
        uint8_t unused[5];      // Unused padding
    };

    RomHeader m_header{};

    bool m_romLoadedSuccessfully;
    uint8_t m_mapperID;

    std::vector<uint8_t> m_prgRom;  // PRG ROM data
    std::vector<uint8_t> m_chrRom;  // CHR ROM data

    uint8_t CpuRead(uint16_t address);
    void CpuWrite(uint16_t address, uint8_t data);

    uint8_t PpuRead(uint16_t address);
    void PpuWrite(uint16_t address, uint8_t data);
};