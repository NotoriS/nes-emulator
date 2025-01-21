#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include <memory>
#include <iostream>

#include "mappers/mapper.h"
#include "mappers/mapper-000.h"

class Cartridge
{
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

public:
    // TODO: Add other mirror modes
    enum class MirrorModes : uint8_t
    {
        Horizontal = 0,
        Vertical = 1
    };

    Cartridge();
    ~Cartridge();

    void LoadROM(const std::string& filename);

    uint8_t CpuRead(uint16_t address) { return m_mapper->CpuRead(address); }
    void CpuWrite(uint16_t address, uint8_t data) { m_mapper->CpuWrite(address, data); }

    uint8_t PpuRead(uint16_t address) { return m_mapper->PpuRead(address); }
    void PpuWrite(uint16_t address, uint8_t data) { m_mapper->PpuWrite(address, data); }

    MirrorModes GetMirrorMode() { return m_mirrorMode; }

private:
    RomHeader m_header{};

    std::unique_ptr<Mapper> m_mapper;

    std::vector<uint8_t> m_prgRom;  // PRG ROM data
    std::vector<uint8_t> m_chrRom;  // CHR ROM data

    MirrorModes m_mirrorMode = MirrorModes::Horizontal;

    void CreateMapper(uint8_t mapperID);
};