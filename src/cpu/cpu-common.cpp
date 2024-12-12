#include "cpu.h"

CPU::CPU() {}

CPU::~CPU() {}

void CPU::Clock()
{
    if (m_skipNextCycle)
    {
        m_skipNextCycle = false;
        return;
    }

    if (m_microInstructionQueue.empty())
    {
        QueueNextInstuction();
    }
    else
    {
        m_microInstructionQueue.front()();
        m_microInstructionQueue.pop();
    }
}

void CPU::Write(uint16_t address, uint8_t data)
{
    if (m_bus != nullptr) m_bus->Write(address, data);
}

uint8_t CPU::Read(uint16_t address)
{
    if (m_bus != nullptr) return m_bus->Read(address);
}

uint8_t CPU::GetFlag(Flag flag)
{
    return ((reg_p & static_cast<uint8_t>(flag)) > 0) ? 1 : 0;
}

void CPU::SetFlag(Flag flag, bool value)
{
    if (value) reg_p |= static_cast<uint8_t>(flag);
    else reg_p &= ~static_cast<uint8_t>(flag);
}

void CPU::StackPush(uint8_t value)
{
    Write(0x0100 | reg_s, value);
}

uint8_t CPU::StackPop()
{
    return Read(0x0100 | reg_s);
}
