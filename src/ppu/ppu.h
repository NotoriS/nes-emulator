#pragma once

#include "../interfaces/i-bus.h"
#include "ppu-bus.h"

#include <cstdint>
#include <memory>

class PPU
{
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

    // Used internally to read and write to the PPU's bus
    uint8_t ReadFromBus(uint16_t address) { return m_bus->Read(address); }
    void WriteToBus(uint16_t address, uint8_t data) { m_bus->Write(address, data); }
};