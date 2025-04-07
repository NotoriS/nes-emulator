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
            m_oddFrame = !m_oddFrame;
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
        if (m_scanline < DISPLAY_HEIGHT && m_dot > 0 && m_dot <= 64) return 0xFF;
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
        throw std::runtime_error("address out of bounds for PPU::WriteByteToSecondaryOAM(uint8_t address, uint8_t data)");
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
        throw std::runtime_error("unexpected path reached in PPU::ReadByteFromOAM(uint8_t address).");
        return 0;
    }
}

uint8_t PPU::ReadByteFromSecondaryOAM(uint8_t address) const
{
    if (address >= 32)
    {
        throw std::runtime_error("address out of bounds for PPU::ReadByteFromSecondaryOAM(uint8_t address)");
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
        throw std::runtime_error("unexpected path reached in PPU::ReadByteFromSecondaryOAM(uint8_t address).");
        return 0;
    }
}

void PPU::PerformTickLogic()
{
    bool oddFrameCycleSkipped = m_oddFrame && m_scanline == -1 && m_dot == 340
        && (m_mask.enableBackground || m_mask.enableSprites);
    if (oddFrameCycleSkipped) m_scanline = m_dot = 0;

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

        // Garbage Nametable Reads
        if (m_dot > DISPLAY_WIDTH && m_dot <= 320)
        {
            if (m_dot % 8 == 2 || m_dot % 8 == 4)
            {
                FetchFromNametable();
            }
        }
        if (m_dot == 338 || m_dot == 340 || oddFrameCycleSkipped) FetchFromNametable();
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

    // Sprite Preparation
    if (m_dot == 0)
    {
        m_spritesOnCurrentScanline = m_spritesFoundDuringEval;
        m_spriteZeroIsInSpriteFragments = m_spriteZeroIsInSecondaryOAM;
    }
    if (m_scanline >= 0 && m_scanline < DISPLAY_HEIGHT)
    {
        // Secondary OAM Clear
        if (m_dot > 0 && m_dot <= 64 && (m_dot % 2 == 1))
        {
            WriteByteToSecondaryOAM(m_dot / 2, 0xFF);
        }

        if (m_dot > 64 && m_dot <= DISPLAY_WIDTH)
        {
            TickSpriteEvaluation();
        }
    }
    if (m_scanline < DISPLAY_HEIGHT && m_dot > DISPLAY_WIDTH && m_dot <= 320)
    {
        TickSpriteFetches();
    }

    if (m_scanline >= 0 && m_scanline < DISPLAY_HEIGHT && m_dot > 1 && m_dot < DISPLAY_WIDTH)
    {
        ShiftSprites();
    }
}

uint32_t PPU::DeterminePixelColour()
{
    // Determine the background pixel in this position

    uint8_t backgroundPixel = 0;
    uint8_t backgroundPalette = 0;

    if (m_mask.enableBackground)
    {
        uint16_t selectedBit = 0x8000 >> m_fineXScroll;

        uint8_t pixelLow = (m_patternLowShifter & selectedBit) != 0;
        uint8_t pixelHigh = (m_patternHighShifter & selectedBit) != 0;
        backgroundPixel = (pixelHigh << 1) | pixelLow;

        uint8_t paletteLow = (m_attributeLowShifter & selectedBit) != 0;
        uint8_t paletteHigh = (m_attributeHighShifter & selectedBit) != 0;
        backgroundPalette = (paletteHigh << 1) | paletteLow;
    }

    // Determine the foreground pixel in this position

    uint8_t foregroundPixel = 0;
    uint8_t foregroundPalette = 0;
    bool foregroundPriority = 0;
    bool spriteZeroIsRendering = false;

    if (m_mask.enableSprites)
    {
        for (char i = 0; i < m_spritesOnCurrentScanline; i++)
        {
            if (m_spriteFragments[i].xPosition != 0) continue;

            uint8_t pixelLow = (m_spriteFragments[i].patternLowShifter & 0x80) != 0;
            uint8_t pixelHigh = (m_spriteFragments[i].patternHighShifter & 0x80) != 0;
            foregroundPixel = (pixelHigh << 1) | pixelLow;

            foregroundPalette = (m_spriteFragments[i].attributes & 0x03) | 0x04;
            foregroundPriority = (m_spriteFragments[i].attributes & 0x20) == 0;

            if (foregroundPixel != 0)
            {
                if (m_spriteZeroIsInSpriteFragments && i == 0) spriteZeroIsRendering = true;
                break;
            }
        }
    }

    // Determine whether the background or foreground should be rendered

    uint8_t pixel = 0;
    uint8_t palette = 0;

    if (backgroundPixel != 0 && foregroundPixel != 0)
    {
        bool spriteZeroHit = spriteZeroIsRendering
            && m_dot != 255
            && !((!m_mask.showLeftmostSprites || !m_mask.showLeftmostBackground)
                && m_dot >= 0 && m_dot <= 7);

        if (spriteZeroHit) m_status.spriteZeroHit = 1;

        if (foregroundPriority) backgroundPixel = 0;
        else foregroundPixel = 0;
    }
    
    if (backgroundPixel != 0 && foregroundPixel == 0)
    {
        pixel = backgroundPixel;
        palette = backgroundPalette;
    }
    else if (backgroundPixel == 0 && foregroundPixel != 0)
    {
        pixel = foregroundPixel;
        palette = foregroundPalette;
    }

    uint16_t paletteIndexAddress = 0x3F00 + (palette << 2) + pixel;
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

void PPU::ShiftSprites()
{
    if (!m_mask.enableSprites) return;

    for (char i = 0; i < m_spritesOnCurrentScanline; i++)
    {
        if (m_spriteFragments[i].xPosition > 0)
        {
            m_spriteFragments[i].xPosition--;
        }
        else
        {
            m_spriteFragments[i].patternLowShifter <<= 1;
            m_spriteFragments[i].patternHighShifter <<= 1;
        }
    }
}

void PPU::TickSpriteEvaluation()
{
    if (m_dot == 65)
    {
        if (!m_spriteEvalCompleted)
        {
            Logger::GetInstance().Warn(std::format("sprites passed {} were not evaluated on scanline {}", 
                static_cast<int>(m_spriteEvalOAMIndex), static_cast<int>(m_scanline)));
        }
        m_spriteEvalCompleted = false;
        m_spriteEvalState = SpriteEvaluationState::ReadY;
        m_spritesFoundDuringEval = 0;
        m_spriteEvalOAMIndex = 0;
        m_spriteZeroIsInSecondaryOAM = false;
    }

    switch (m_spriteEvalState)
    {
    case SpriteEvaluationState::ReadY:
        if (m_dot % 2 == 0) break;
        m_spriteEvalByteBuffer = m_OAM[m_spriteEvalOAMIndex].yPosition;
        m_spriteEvalState = SpriteEvaluationState::WriteY;
        break;
    case SpriteEvaluationState::WriteY:
        if (m_spritesFoundDuringEval >= 8)
        {
            m_spriteEvalState = SpriteEvaluationState::IncrementOAMIndex;
            break;
        }
        m_secondaryOAM[m_spritesFoundDuringEval].yPosition = m_spriteEvalByteBuffer;
        if (SpriteInRangeOfNextScanline(m_spriteEvalByteBuffer))
            m_spriteEvalState = SpriteEvaluationState::ReadTileIndex;
        else
            m_spriteEvalState = SpriteEvaluationState::IncrementOAMIndex;
        break;
    case SpriteEvaluationState::ReadTileIndex:
        m_spriteEvalByteBuffer = m_OAM[m_spriteEvalOAMIndex].tileIndex;
        m_spriteEvalState = SpriteEvaluationState::WriteTileIndex;
        break;
    case SpriteEvaluationState::WriteTileIndex:
        m_secondaryOAM[m_spritesFoundDuringEval].tileIndex = m_spriteEvalByteBuffer;
        m_spriteEvalState = SpriteEvaluationState::ReadAttributes;
        break;
    case SpriteEvaluationState::ReadAttributes:
        m_spriteEvalByteBuffer = m_OAM[m_spriteEvalOAMIndex].attributes;
        m_spriteEvalState = SpriteEvaluationState::WriteAttributes;
        break;
    case SpriteEvaluationState::WriteAttributes:
        m_secondaryOAM[m_spritesFoundDuringEval].attributes = m_spriteEvalByteBuffer;
        m_spriteEvalState = SpriteEvaluationState::ReadX;
        break;
    case SpriteEvaluationState::ReadX:
        m_spriteEvalByteBuffer = m_OAM[m_spriteEvalOAMIndex].xPosition;
        m_spriteEvalState = SpriteEvaluationState::WriteX;
        break;
    case SpriteEvaluationState::WriteX:
        m_secondaryOAM[m_spritesFoundDuringEval].xPosition = m_spriteEvalByteBuffer;
        m_spritesFoundDuringEval++;
        if (m_spriteEvalOAMIndex == 0) m_spriteZeroIsInSecondaryOAM = true;
        m_spriteEvalState = SpriteEvaluationState::IncrementOAMIndex;
        break;
    case SpriteEvaluationState::StartCheckForSpriteOverflow:
        m_spriteOverflowPointer = m_spriteEvalOAMIndex * 4;
        m_spriteEvalState = SpriteEvaluationState::CheckForSpriteOverflow;
        [[fallthrough]];
    case SpriteEvaluationState::CheckForSpriteOverflow:
        // This section is intentionally incorrect to match the buggy behaviour of the NES
        if (m_dot % 2 == 0) break;
        if (SpriteInRangeOfNextScanline(ReadByteFromOAM(m_spriteOverflowPointer)))
        {
            m_status.spriteOverflow = 1;
            m_spriteOverflowPointer += 4;
            if (m_spriteOverflowPointer < 4) m_spriteEvalState = SpriteEvaluationState::ReadY;
        }
        else
        {
            m_spriteOverflowPointer += 5;
            if (m_spriteOverflowPointer < 5) m_spriteEvalState = SpriteEvaluationState::ReadY;
        }
        m_spriteEvalOAMIndex = m_spriteOverflowPointer / 4;
        break;
    }

    if (m_spriteEvalState == SpriteEvaluationState::IncrementOAMIndex)
    {
        m_spriteEvalOAMIndex++;
        if (m_spriteEvalOAMIndex == 64)
        {
            m_spriteEvalOAMIndex = 0;
            m_spriteEvalCompleted = true;
            m_spriteEvalState = SpriteEvaluationState::ReadY;
        }
        else if (m_spritesFoundDuringEval < 8)
        {
            m_spriteEvalState = SpriteEvaluationState::ReadY;
        }
        else
        {
            m_spriteEvalCompleted = true;
            m_spriteEvalState = SpriteEvaluationState::StartCheckForSpriteOverflow;
        }
    }
}

bool PPU::SpriteInRangeOfNextScanline(uint8_t yPosition)
{
    short diff = m_scanline - static_cast<short>(m_spriteEvalByteBuffer);
    return diff >= 0 && diff < (m_control.spriteSize ? 16 : 8);
}

void PPU::TickSpriteFetches()
{
    char spriteIndex = (m_dot - DISPLAY_WIDTH) / 8;
    char fetchCycle = (m_dot - DISPLAY_WIDTH) % 8;

    if (spriteIndex >= m_spritesFoundDuringEval) return;

    uint16_t spritePatternAddress;
    if (fetchCycle == 5) // Fetch least significant bits
    {
        spritePatternAddress = GetSpritePatternAddress(spriteIndex);
        m_spritePatternAddressBuffer = spritePatternAddress;

        m_spriteFragments[spriteIndex].patternLowShifter = ReadFromBus(spritePatternAddress);
        if (m_secondaryOAM[spriteIndex].attributes & 0x40) 
            HorizontallyFlipByte(m_spriteFragments[spriteIndex].patternLowShifter);

    }
    else if (fetchCycle == 7) // Fetch most significant bits
    {
        spritePatternAddress = m_spritePatternAddressBuffer + 8;

        m_spriteFragments[spriteIndex].patternHighShifter = ReadFromBus(spritePatternAddress);
        if (m_secondaryOAM[spriteIndex].attributes & 0x40)
            HorizontallyFlipByte(m_spriteFragments[spriteIndex].patternHighShifter);

        m_spriteFragments[spriteIndex].attributes = m_secondaryOAM[spriteIndex].attributes;
        m_spriteFragments[spriteIndex].xPosition = m_secondaryOAM[spriteIndex].xPosition;
    }
}

uint16_t PPU::GetSpritePatternAddress(const char& spriteIndex)
{
    bool verticalFlip = m_secondaryOAM[spriteIndex].attributes & 0x80;
    char diff = m_scanline - m_secondaryOAM[spriteIndex].yPosition;

    uint8_t patternTable;
    uint8_t cell;
    uint8_t row;

    if (m_control.spriteSize)
    {
        patternTable = m_secondaryOAM[spriteIndex].tileIndex & 0x01;
        cell = (m_secondaryOAM[spriteIndex].tileIndex & 0xFE) + ((diff < 8) ^ verticalFlip ? 0 : 1);
    }
    else
    {
        patternTable = m_control.spritePatternTable;
        cell = m_secondaryOAM[spriteIndex].tileIndex;
    }

    if (verticalFlip)
        row = (7 - diff) & 0x07;
    else 
        row = diff & 0x07;

    return (patternTable << 12) | (cell << 4) | row;
}

void PPU::HorizontallyFlipByte(uint8_t& byte)
{
    byte = ((byte * 0x0802LU & 0x22110LU) | (byte * 0x8020LU & 0x88440LU)) * 0x10101LU >> 16;
}
