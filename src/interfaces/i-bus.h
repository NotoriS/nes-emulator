#pragma once

#include <cstdint>

class IBus
{
public:
    virtual ~IBus() {}
    virtual void Write(uint16_t address, uint8_t data) = 0;
    virtual uint8_t Read(uint16_t address) = 0;
};