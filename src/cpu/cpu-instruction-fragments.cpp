#include "cpu.h"

void CPU::Break()
{
    m_microInstructionQueue.push([this]() { reg_pc++; });
    m_microInstructionQueue.push([this]()
        {
            StackPush(reg_pc >> 2);
            reg_s--;
        });
    m_microInstructionQueue.push([this]()
        {
            StackPush(reg_pc & 0x00FF);
            reg_s--;
        });
    m_microInstructionQueue.push([this]()
        {
            StackPush(reg_p | static_cast<uint8_t>(Flag::B));
            reg_s--;
        });
    m_microInstructionQueue.push([this]() { reg_pc = Read(0xFFFE); });
    m_microInstructionQueue.push([this]() { reg_pc |= Read(0xFFFF); });
}

void CPU::ReturnFromInterrupt()
{
    m_microInstructionQueue.push([]() { /* Skip cycle */ });
    m_microInstructionQueue.push([this]() { reg_s++; });
    m_microInstructionQueue.push([this]()
        {
            reg_p = StackPop();
            reg_s++;
        });
    m_microInstructionQueue.push([this]()
        {
            reg_pc = StackPop();
            reg_s++;
        });
    m_microInstructionQueue.push([this]() { reg_pc |= StackPop(); });
}
