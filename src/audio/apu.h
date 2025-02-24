#pragma once

#include <cstdint>

class APU
{
public:
    APU();
    ~APU();

    void Clock();

    uint8_t Read(uint16_t address);
    void Write(uint16_t address, uint8_t data);

    int16_t Sample();
};