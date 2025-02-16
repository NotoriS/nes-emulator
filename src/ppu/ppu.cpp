#include "ppu.h"

PPU::PPU(std::shared_ptr<IBus> bus) : m_bus(bus)
{
    // Set pixel buffer to black
    for (int i = 0; i < DISPLAY_HEIGHT * DISPLAY_WIDTH; i++) m_pixelBuffer[i] = 0;
}

PPU::~PPU()
{
}

void PPU::Clock()
{
    PerformTickLogic();

    if (m_dot > 0 && m_dot <= DISPLAY_WIDTH && m_scanline >= 0 && m_scanline < DISPLAY_HEIGHT)
        m_pixelBuffer[m_scanline * DISPLAY_WIDTH + m_dot - 1] = DeterminePixelColour();

    m_dot++;
    if (m_dot > 340)
    {
        m_dot = 0;
        m_scanline++;
        if (m_scanline > 260)
        {
            m_scanline = -1;
            m_frameCompleted = true;
        }
    }
}

void PPU::Reset()
{
    // TODO
}

bool PPU::NmiInterruptWasRaised()
{
    if (m_nmiInterruptRaised)
    {
        m_nmiInterruptRaised = false;
        return true;
    }
    else
    {
        return false;
    }
}

uint8_t PPU::Read(uint16_t address)
{
    uint8_t result = 0x00;

    switch (address)
    {
    case 0x2002: // PPUSTATUS
        m_status.ppuOpenBus = m_readBuffer & 0x1F;
        result = m_status.byte;
        m_status.vblank = 0;
        m_firstWrite = true;
        return result;
    case 0x2004: // OAMDATA
        return ReadByteFromOAM(m_OAMAddress);
    case 0x2007: // PPUDATA
        result = m_readBuffer;
        m_readBuffer = ReadFromBus(m_currVramAddress.address);
        if (m_currVramAddress.address >= 0x3F00) result = m_readBuffer; // Ignore buffer for palette RAM reads
        IncrementVramAddress();
        return result;
    default:
        Logger::GetInstance().Warn("the CPU attempted to read from the PPU at an invalid address (" + Logger::DecmialToHex(address) + ")");
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
        m_OAMAddress = data;
        break;
    case 0x2004: // OAMDATA
        WriteByteToOAM(m_OAMAddress++, data);
        break;
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
        WriteToBus(m_currVramAddress.address, data);
        IncrementVramAddress();
        break;
    default:
        Logger::GetInstance().Warn("the CPU attempted to write to the PPU at an invalid address (" + Logger::DecmialToHex(address) + ")");
        break;
    }
}

void PPU::WriteByteToOAM(uint8_t address, uint8_t data)
{
    uint8_t translatedAddress = address / 4;
    uint8_t property = address % 4;

    switch (property)
    {
    case 0:
        m_OAM[translatedAddress].yPosition = data;
        return;
    case 1:
        m_OAM[translatedAddress].tileIndex = data;
        return;
    case 2:
        m_OAM[translatedAddress].attributes = data;
        return;
    case 3:
        m_OAM[translatedAddress].xPosition = data;
        return;
    }
}

void PPU::WriteByteToSecondaryOAM(uint8_t address, uint8_t data)
{
    if (address >= 32)
    {
        Logger::GetInstance().Error("address out of bounds for PPU::WriteByteToSecondaryOAM(uint8_t address, uint8_t data)");
        return;
    }

    uint8_t translatedAddress = address / 4;
    uint8_t property = address % 4;

    switch (property)
    {
    case 0:
        m_secondaryOAM[translatedAddress].yPosition = data;
        return;
    case 1:
        m_secondaryOAM[translatedAddress].tileIndex = data;
        return;
    case 2:
        m_secondaryOAM[translatedAddress].attributes = data;
        return;
    case 3:
        m_secondaryOAM[translatedAddress].xPosition = data;
        return;
    }
}

uint8_t PPU::ReadByteFromOAM(uint8_t address) const
{
    uint8_t translatedAddress = address / 4;
    uint8_t property = address % 4;

    switch (property)
    {
    case 0:
        return m_OAM[translatedAddress].yPosition;
    case 1:
        return m_OAM[translatedAddress].tileIndex;
    case 2:
        return m_OAM[translatedAddress].attributes;
    case 3:
        return m_OAM[translatedAddress].xPosition;
    default:
        Logger::GetInstance().Error("unexpected path reached in PPU::ReadByteFromOAM(uint8_t address).");
        return 0;
    }
}

uint8_t PPU::ReadByteFromSecondaryOAM(uint8_t address) const
{
    if (address >= 32)
    {
        Logger::GetInstance().Error("address out of bounds for PPU::ReadByteFromSecondaryOAM(uint8_t address)");
        return 0;
    }

    uint8_t translatedAddress = address / 4;
    uint8_t property = address % 4;

    switch (property)
    {
    case 0:
        return m_secondaryOAM[translatedAddress].yPosition;
    case 1:
        return m_secondaryOAM[translatedAddress].tileIndex;
    case 2:
        return m_secondaryOAM[translatedAddress].attributes;
    case 3:
        return m_secondaryOAM[translatedAddress].xPosition;
    default:
        Logger::GetInstance().Error("unexpected path reached in PPU::ReadByteFromSecondaryOAM(uint8_t address).");
        return 0;
    }
}

void PPU::PerformTickLogic()
{
    if (m_scanline >= -1 && m_scanline < DISPLAY_HEIGHT) // Rendering scanlines
    {
        if (m_dot > 0 && m_dot <= DISPLAY_WIDTH || m_dot >= 321 && m_dot <= 336) // Rendering Cycles
        {
            ShiftShifters();
            switch (m_dot % 8)
            {
            case 1:
                LoadShiftersLowByte();
                break;
            case 2:
                FetchFromNametable();
                break;
            case 4:
                FetchFromAttributeTable();
                break;
            case 6:
                FetchPatternLeastSignificantBits();
                break;
            case 0:
                FetchPatternMostSignificantBits();
                IncrementHorizontalPointer();
                break;
            }
        }
        if (m_dot == 256) IncrementVerticalPointer();
        if (m_dot == 257)
        {
            LoadShiftersLowByte();
            TransferHoriontalPointer();
        }
        if (m_dot == 338 || m_dot == 340) FetchFromNametable();
    }

    if (m_scanline == -1)
    {
        if (m_dot == 1)
        {
            m_status.vblank = 0;
            m_status.spriteZeroHit = 0;
            m_status.spriteOverflow = 0;
        }
        if (m_dot >= 280 && m_dot <= 304) TransferVerticalPointer();
    }

    if (m_scanline == 241 && m_dot == 1)
    {
        m_status.vblank = 1;
        if (m_control.vblankNmi)
            m_nmiInterruptRaised = true;
    }
}

uint32_t PPU::DeterminePixelColour()
{
    if (!m_mask.enableBackground) 
        return COLOUR_PALETTE[ReadFromBus(0x3F00) & 0x3F];

    uint16_t selectedBit = 0x8000 >> m_fineXScroll;

    uint8_t pixelLow = (m_patternLowShifter & selectedBit) != 0;
    uint8_t pixelHigh = (m_patternHighShifter & selectedBit) != 0;
    uint8_t backgroundPixel = (pixelHigh << 1) | pixelLow;

    uint8_t paletteLow = (m_attributeLowShifter & selectedBit) != 0;
    uint8_t paletteHigh = (m_attributeHighShifter & selectedBit) != 0;
    uint8_t backgroundPalette = (paletteHigh << 1) | paletteLow;

    uint16_t paletteIndexAddress = 0x3F00 + (backgroundPalette << 2) + backgroundPixel;
    return COLOUR_PALETTE[ReadFromBus(paletteIndexAddress) & 0x3F];
}

void PPU::FetchFromNametable()
{
    m_nametableByte = ReadFromBus(
        NAMETABLE_BASE
        | (m_currVramAddress.address & 0x0FFF)
    );
}

void PPU::FetchFromAttributeTable()
{
    m_tileAttribute = ReadFromBus(
        NAMETABLE_BASE | ATTRIBUTE_TABLE_OFFSET
        | (m_currVramAddress.nametableY << 11)
        | (m_currVramAddress.nametableX << 10)
        | ((m_currVramAddress.coarseYScroll >> 2) << 3)
        | (m_currVramAddress.coarseXScroll >> 2)
    );

    // Shift the 4 tile attribute byte to isolate the current tile (2 bits)
    if (m_currVramAddress.coarseYScroll & 2) m_tileAttribute >>= 4;
    if (m_currVramAddress.coarseXScroll & 2) m_tileAttribute >>= 2;
    m_tileAttribute &= 0b11;
}

void PPU::FetchPatternLeastSignificantBits()
{
    m_patternTableTileLow = ReadFromBus(
        (m_control.backgroundPatternTable << 12)
        | (m_nametableByte << 4)
        | m_currVramAddress.fineYScroll
    );
}

void PPU::FetchPatternMostSignificantBits()
{
    m_patternTableTileHigh = ReadFromBus(
        (m_control.backgroundPatternTable << 12)
        | (m_nametableByte << 4)
        | m_currVramAddress.fineYScroll + 8
    );
}

void PPU::IncrementVramAddress()
{
    if (m_control.vramIncrement)
        m_currVramAddress.address += 32;
    else
        m_currVramAddress.address++;
}

void PPU::IncrementHorizontalPointer()
{
    if (!m_mask.enableBackground && !m_mask.enableSprites) return;

    m_currVramAddress.coarseXScroll++;

    // Flip nametable selector when a coarse X scroll overflow is detected
    if (m_currVramAddress.coarseXScroll == 0)
        m_currVramAddress.nametableX = ~m_currVramAddress.nametableX;
}

void PPU::IncrementVerticalPointer()
{
    if (!m_mask.enableBackground && !m_mask.enableSprites) return;

    // Only update fine Y scroll when possible (no overflow)
    m_currVramAddress.fineYScroll++;
    if (m_currVramAddress.fineYScroll != 0) return;

    if (m_currVramAddress.coarseYScroll == 29)
    {
        // Switch to the next nametable before running in to the attribute table
        m_currVramAddress.coarseYScroll = 0;
        m_currVramAddress.nametableY = ~m_currVramAddress.nametableY;
    }
    else
    {
        m_currVramAddress.coarseYScroll++;
    }
}

void PPU::TransferHoriontalPointer()
{
    if (!m_mask.enableBackground && !m_mask.enableSprites) return;

    m_currVramAddress.nametableX = m_tempVramAddress.nametableX;
    m_currVramAddress.coarseXScroll = m_tempVramAddress.coarseXScroll;
}

void PPU::TransferVerticalPointer()
{
    if (!m_mask.enableBackground && !m_mask.enableSprites) return;

    m_currVramAddress.nametableY = m_tempVramAddress.nametableY;
    m_currVramAddress.coarseYScroll = m_tempVramAddress.coarseYScroll;
    m_currVramAddress.fineYScroll = m_tempVramAddress.fineYScroll;
}

void PPU::LoadShiftersLowByte()
{
    m_patternLowShifter &= 0xFF00;
    m_patternLowShifter |= m_patternTableTileLow;

    m_patternHighShifter &= 0xFF00;
    m_patternHighShifter |= m_patternTableTileHigh;

    m_attributeLowShifter &= 0xFF00;
    m_attributeLowShifter |= (m_tileAttribute & 1) ? 0xFF : 0;

    m_attributeHighShifter &= 0xFF00;
    m_attributeHighShifter |= (m_tileAttribute & 2) ? 0xFF : 0;

}

void PPU::ShiftShifters()
{
    if (!m_mask.enableBackground) return;

    m_patternLowShifter <<= 1;
    m_patternHighShifter <<= 1;
    m_attributeLowShifter <<= 1;
    m_attributeHighShifter <<= 1;
}
