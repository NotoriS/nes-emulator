#pragma once

#include "../interfaces/i-bus.h"
#include "ppu-bus.h"

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
            uint8_t coarseXScroll : 5;
            uint8_t coarseYScroll : 5;
            uint8_t nametableX : 1;
            uint8_t nametableY : 1;
            uint8_t fineYScroll : 3;
            uint8_t unused : 1;
        };
        uint16_t address = 0x0000;
    };

public:
    PPU(std::shared_ptr<IBus> bus);
    ~PPU();

    void Clock();
    void Reset();

    // Used externally to read and write to the PPU from the CPU bus
    uint8_t Read(uint16_t address);
    void Write(uint16_t address, uint8_t data);

private:
    std::shared_ptr<IBus> m_bus;

    // Exposed registers
    PPUCTRL m_control;
    PPUMASK m_mask;
    PPUSTATUS m_status;

    // Internal registers
    AddressRegister m_currVramAddress;
    AddressRegister m_tempVramAddress;
    uint8_t m_fineXScroll = 0x00;
    bool m_firstWrite = true;

    // Used internally to read and write to the PPU's bus
    uint8_t ReadFromBus(uint16_t address) { return m_bus->Read(address); }
    void WriteToBus(uint16_t address, uint8_t data) { m_bus->Write(address, data); }
};