#include "cpu.h"

CPU::CPU(IBus* bus) : m_bus(bus)
{
    Reset();
}

CPU::~CPU() {}

void CPU::Reset()
{
    while (!m_microInstructionQueue.empty()) m_microInstructionQueue.pop_front();

    reg_pc = Read(0xFFFC);
    reg_pc |= Read(0xFFFD) << 8;

    reg_s -= 3;

    SetFlag(Flag::U, true);
    SetFlag(Flag::I, true);
}

void CPU::Clock()
{
    if (m_microInstructionQueue.empty())
    {
        QueueNextInstuction();
    }
    else
    {
        auto instruction = m_microInstructionQueue.front();
        m_microInstructionQueue.pop_front();
        instruction();
    }
}

void CPU::Write(uint16_t address, uint8_t data)
{
    if (m_bus != nullptr) m_bus->Write(address, data);
}

uint8_t CPU::Read(uint16_t address)
{
    if (m_bus != nullptr) return m_bus->Read(address);
    throw std::runtime_error("CPU tried to read from the bus before it was connected.");
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
