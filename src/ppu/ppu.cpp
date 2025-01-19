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
    uint8_t result = 0x00;

    switch (address)
    {
        case 0x2002: // PPUSTATUS
            // TODO: pickup data from the open bus in bytes 4-0
            result = m_status.byte;
            m_status.vblank = 0;
            m_firstWrite = true;
            return result;
        case 0x2004: // OAMDATA
            // TODO
            return 0;
        case 0x2007: // PPUDATA
            // TODO
            return 0;
        default:
            std::cout << "Warning: The CPU attempted to read from the PPU at an invalid address (0x"
                << std::hex << static_cast<int>(address) << ")" << std::endl;
            return 0;
    }
}

void PPU::Write(uint16_t address, uint8_t data)
{
    switch (address)
    {
    case 0x2000: // PPUCTRL
        m_control.byte = data;
        m_tempVramAddress.nametableX = m_control.nametableX;
        m_tempVramAddress.nametableY = m_control.nametableY;
        break;
    case 0x2001: // PPUMASK
        m_mask.byte = data;
        break;
    case 0x2003: // OAMADDR
        // TODO
    case 0x2004: // OAMDATA
        // TODO
    case 0x2005: // PPUSCROLL
        if (m_firstWrite)
        {
            m_fineXScroll = data & 0x07;
            m_tempVramAddress.coarseXScroll = data >> 3;
        }
        else
        {
            m_tempVramAddress.fineYScroll = data & 0x07;
            m_tempVramAddress.coarseYScroll = data >> 3;
        }
        m_firstWrite = !m_firstWrite;
        break;
    case 0x2006: // PPUADDR
        if (m_firstWrite)
        {
            m_tempVramAddress.address &= 0x00FF;
            m_tempVramAddress.address |= (data & 0x3F) << 8;
        }
        else
        {
            m_tempVramAddress.address &= 0xFF00;
            m_tempVramAddress.address |= data;
            m_currVramAddress.address = m_tempVramAddress.address;
        }
        m_firstWrite = !m_firstWrite;
        break;
    case 0x2007: // PPUDATA
        // TODO
    default:
        std::cout << "Warning: The CPU attempted to write to the PPU at an invalid address (0x"
            << std::hex << static_cast<int>(address) << ")" << std::endl;
        break;
    }
}
