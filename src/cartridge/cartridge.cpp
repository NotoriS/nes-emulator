#include <fstream>
#include <iostream>

#include "cartridge.h"

Cartridge::Cartridge()
{
}

Cartridge::~Cartridge()
{
}

void Cartridge::LoadROM(const std::string& filename)
{
    std::ifstream file;
    file.open(filename, std::ifstream::binary);
    if (!file.is_open()) 
    {
        file.close();
        throw std::runtime_error("Failed to open file: " + filename);
    }

    // Read and validate the header
    file.read(reinterpret_cast<char*>(&m_header), sizeof(RomHeader));
    if (m_header.signature[0] != 'N' || m_header.signature[1] != 'E' ||
        m_header.signature[2] != 'S' || m_header.signature[3] != 0x1A) 
    {
        file.close();
        throw std::runtime_error("Invalid NES file format: " + filename);
    }

    // Discard trainer if present
    if (m_header.flags6 & 0x04)
        file.seekg(512, std::ios_base::cur);

    // Load PRG ROM
    size_t prgSize = m_header.prgRomSize * 16 * 1024;
    m_prgRom.resize(prgSize);
    file.read(reinterpret_cast<char*>(m_prgRom.data()), prgSize);

    // Load CHR ROM (if present)
    size_t chrSize = m_header.chrRomSize * 8 * 1024;
    m_chrRom.resize(chrSize);
    file.read(reinterpret_cast<char*>(m_chrRom.data()), chrSize);

    file.close();

    // Fetch the mapper ID and create the mapper
    uint8_t mapperID = (m_header.flags6 & 0xF0) & (m_header.flags7 >> 4);
    CreateMapper(mapperID);

    std::cout << "Loaded " << prgSize << " bytes of PRG ROM and "
        << chrSize << " bytes of CHR ROM from " << filename << std::endl;
}

void Cartridge::CreateMapper(uint8_t mapperID)
{
    switch (mapperID)
    {
        case 0:
            m_mapper = std::make_unique<Mapper000>(m_prgRom, m_chrRom);
            break;
        default:
            std::cerr << "The mapper required by this ROM is unsuported." << std::endl;
            break;
    }
}
