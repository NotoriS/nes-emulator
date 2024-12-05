#include "cpu.h"

void CPU::Break()
{
    m_microInstructionQueue.push([this]() { reg_pc++; });
    m_microInstructionQueue.push([this]() { StackPush(reg_pc >> 2); });
    m_microInstructionQueue.push([this]() { StackPush(reg_pc & 0x00FF); });
    m_microInstructionQueue.push([this]() { StackPush(reg_p | static_cast<uint8_t>(Flag::B)); });
    m_microInstructionQueue.push([this]() { reg_pc = Read(0xFFFE); });
    m_microInstructionQueue.push([this]() { reg_pc |= Read(0xFFFF); });
}