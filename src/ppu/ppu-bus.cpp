#include "ppu-bus.h"

PpuBus::PpuBus()
{
    m_paletteMemory.fill(0);
    for (int i = 0; i < 2; i++)
    {
        for (int j = 0; j < 0x400; j++)
        {
            m_nameTables[i][j] = 0;
        }
    }
}

PpuBus::~PpuBus()
{
}

uint8_t PpuBus::Read(uint16_t address)
{
    if (address < 0x2000) // Read from cartridge
    {
        return m_cartridge->PpuRead(address);
    }
    else if (address < 0x3F00) // Read from nametable
    {
        address &= 0x2FFF; // Convert the 0x3000 range to the 0x2000 range

        switch (m_cartridge->GetMirrorMode())
        {
        case MirrorMode::Vertical:
            if (address < 0x2400)
                return m_nameTables[0][address & 0x03FF];
            else if (address < 0x2800)
                return m_nameTables[1][address & 0x03FF];
            else if (address < 0x2C00)
                return m_nameTables[0][address & 0x03FF];
            else
                return m_nameTables[1][address & 0x03FF];
        case MirrorMode::Horizontal:
            if (address < 0x2400)
                return m_nameTables[0][address & 0x03FF];
            else if (address < 0x2800)
                return m_nameTables[0][address & 0x03FF];
            else if (address < 0x2C00)
                return m_nameTables[1][address & 0x03FF];
            else
                return m_nameTables[1][address & 0x03FF];
        default:
            throw std::runtime_error("unsupported mirror mode encoutered while reading from the name table");
        }
    }
    else if (address < 0x4000) // Read from palette RAM
    {
        address &= 0x1F;
        if ((address & 0x10) && address % 4 == 0) address &= 0xF;
        return m_paletteMemory[address];
    }

    Logger::GetInstance().Warn("failed to read from PPU bus at address " + Logger::DecmialToHex(address));
    return 0;
}

void PpuBus::Write(uint16_t address, uint8_t data)
{
    if (address < 0x2000) // Write to cartridge
    {
        m_cartridge->PpuWrite(address, data);
        return;
    }
    else if (address < 0x3F00) // Read from nametable
    {
        address &= 0x2FFF; // Convert the 0x3000 range to the 0x2000 range

        switch (m_cartridge->GetMirrorMode())
        {
        case MirrorMode::Vertical:
            if (address < 0x2400)
                m_nameTables[0][address & 0x03FF] = data;
            else if (address < 0x2800)
                m_nameTables[1][address & 0x03FF] = data;
            else if (address < 0x2C00)
                m_nameTables[0][address & 0x03FF] = data;
            else
                m_nameTables[1][address & 0x03FF] = data;
            break;
        case MirrorMode::Horizontal:
            if (address < 0x2400)
                m_nameTables[0][address & 0x03FF] = data;
            else if (address < 0x2800)
                m_nameTables[0][address & 0x03FF] = data;
            else if (address < 0x2C00)
                m_nameTables[1][address & 0x03FF] = data;
            else
                m_nameTables[1][address & 0x03FF] = data;
            break;
        default:
            throw std::runtime_error("unsupported mirror mode encoutered while reading from the name table");
        }
        return;
    }
    else if (address < 0x4000) // Write to palette RAM
    {
        address &= 0x1F;
        if ((address & 0x10) && address % 4 == 0) address &= 0xF;
        m_paletteMemory[address] = data;
        return;
    }

    Logger::GetInstance().Warn("failed to write to PPU bus at address " + Logger::DecmialToHex(address));
}
