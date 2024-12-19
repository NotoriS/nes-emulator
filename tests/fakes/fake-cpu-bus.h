#include <array>

#include "../../src/interfaces/i-bus.h"

class TestCpuBus : public IBus
{
public:
    TestCpuBus() { m_memory.fill(0); }
    ~TestCpuBus() override {}

    uint8_t Read(uint16_t address) override
    {
        return m_memory[address];
    }

    void Write(uint16_t address, uint8_t data) override 
    {
        m_memory[address] = data;
    }

private:
    std::array<uint8_t, 0x10000> m_memory;
};