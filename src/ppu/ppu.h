#pragma once

#include "../debug/logger.h"
#include "../interfaces/i-bus.h"
#include "ppu-bus.h"
#include "colour-palette.h"

#include <iostream>
#include <iomanip>
#include <cstdint>
#include <memory>

class PPU
{
    union PPUCTRL
    {
        struct
        {
            uint8_t nametableX : 1;
            uint8_t nametableY : 1;
            uint8_t vramIncrement : 1;
            uint8_t spritePatternTable : 1;
            uint8_t backgroundPatternTable : 1;
            uint8_t spriteSize : 1;
            uint8_t slave : 1;
            uint8_t vblankNmi : 1;
        };
        uint8_t byte = 0x00;
    };

    union PPUMASK
    {
        struct
        {
            uint8_t greyscale : 1;
            uint8_t showLeftmostBackground : 1;
            uint8_t showLeftmostSprites : 1;
            uint8_t enableBackground : 1;
            uint8_t enableSprites : 1;
            uint8_t emphasizeRed : 1;
            uint8_t emphasizeGreen : 1;
            uint8_t emphasizeBlue : 1;
        };
        uint8_t byte = 0x00;
    };

    union PPUSTATUS
    {
        struct
        {
            uint8_t ppuOpenBus : 5;
            uint8_t spriteOverflow : 1;
            uint8_t spriteZeroHit : 1;
            uint8_t vblank : 1;
        };
        uint8_t byte = 0x00;
    };

    union AddressRegister
    {
        struct
        {
            uint16_t coarseXScroll : 5;
            uint16_t coarseYScroll : 5;
            uint16_t nametableX : 1;
            uint16_t nametableY : 1;
            uint16_t fineYScroll : 3;
            uint16_t unused : 1;
        };
        uint16_t address = 0x0000;
    };

    struct ObjectAttribute
    {
        uint8_t yPosition;
        uint8_t tileIndex;
        uint8_t attributes;
        uint8_t xPosition;
    };

    enum class SpriteEvaluationState
    {
        ReadY,
        WriteY,
        ReadTileIndex,
        WriteTileIndex,
        ReadAttributes,
        WriteAttributes,
        ReadX,
        WriteX,
        IncrementOAMIndex,
        CheckForSpriteOverflow
    };

public:
    static const uint16_t DISPLAY_WIDTH = 256;
    static const uint16_t DISPLAY_HEIGHT = 240;

    PPU(std::shared_ptr<IBus> bus);
    ~PPU();

    void Clock();
    void Reset();

    uint32_t* GetPixelBuffer() { m_frameCompleted = false; return m_pixelBuffer; }
    bool FrameIsComplete() const { return m_frameCompleted; }
    bool NmiInterruptWasRaised();

    // Used externally to read and write to the PPU from the CPU bus
    uint8_t Read(uint16_t address);
    void Write(uint16_t address, uint8_t data);

    void WriteByteToOAM(uint8_t address, uint8_t data);
    void WriteByteToSecondaryOAM(uint8_t address, uint8_t data);

private:
    static const uint16_t NAMETABLE_BASE = 0x2000;
    static const uint16_t ATTRIBUTE_TABLE_OFFSET = 0x03C0;

    std::shared_ptr<IBus> m_bus;

    // Exposed registers
    PPUCTRL m_control;
    PPUMASK m_mask;
    PPUSTATUS m_status;

    // Internal control registers
    AddressRegister m_currVramAddress;
    AddressRegister m_tempVramAddress;
    uint8_t m_fineXScroll = 0x00;
    bool m_firstWrite = true;
    uint8_t m_readBuffer = 0x00;

    // Internal rendering registers
    uint8_t m_nametableByte = 0x00;
    uint8_t m_tileAttribute = 0x00;
    uint8_t m_patternTableTileLow = 0x00;
    uint8_t m_patternTableTileHigh = 0x00;
    uint16_t m_patternLowShifter = 0x0000;
    uint16_t m_patternHighShifter = 0x0000;
    uint16_t m_attributeLowShifter = 0x0000;
    uint16_t m_attributeHighShifter = 0x0000;

    short m_scanline = 0;
    short m_dot = 0;

    uint32_t m_pixelBuffer[DISPLAY_HEIGHT * DISPLAY_WIDTH];
    bool m_frameCompleted = false;
    bool m_nmiInterruptRaised = false;

    ObjectAttribute m_OAM[64];
    ObjectAttribute m_secondaryOAM[8];
    uint8_t m_OAMAddress = 0x00;

    SpriteEvaluationState m_spriteEvalState = SpriteEvaluationState::ReadY;
    uint8_t m_spritesFound = 0;
    uint8_t m_spriteEvalOAMIndex = 0;
    uint8_t m_spriteEvalByteBuffer = 0;

    uint8_t ReadByteFromOAM(uint8_t address) const;
    uint8_t ReadByteFromSecondaryOAM(uint8_t address) const;

    // Used internally to read and write to the PPU's bus
    inline uint8_t ReadFromBus(uint16_t address) { return m_bus->Read(address); }
    inline void WriteToBus(uint16_t address, uint8_t data) { m_bus->Write(address, data); }

    void IncrementVramAddress();

    // Rendering helpers
    void PerformTickLogic();
    uint32_t DeterminePixelColour();
    void FetchFromNametable();
    void FetchFromAttributeTable();
    void FetchPatternLeastSignificantBits();
    void FetchPatternMostSignificantBits();
    void IncrementHorizontalPointer();
    void IncrementVerticalPointer();
    void TransferHoriontalPointer();
    void TransferVerticalPointer();
    void LoadShiftersLowByte();
    void ShiftShifters();
    void TickSpriteEvaluation();
};