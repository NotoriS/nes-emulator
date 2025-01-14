#pragma once

#include <cstdint>

class PPU
{
public:
    PPU();
    ~PPU();

    void Clock();
    void Reset();

    // Used externally to read and write to the PPU from the CPU bus
    uint8_t Read(uint16_t address);
    void Write(uint16_t address, uint8_t data);

private:
    // Used internally to read and write to the PPU's bus
    uint8_t ReadFromBus(uint16_t address);
    void WriteToBus(uint16_t address, uint8_t data);
};