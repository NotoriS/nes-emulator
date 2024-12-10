#include "cpu.h"

void CPU::Break()
{
    m_microInstructionQueue.push([this]() { reg_pc++; });
    m_microInstructionQueue.push([this]()
        {
            StackPush(reg_pc >> 8);
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
    m_microInstructionQueue.push([this]()
        {
            reg_pc = Read(0xFFFE);
            SetFlag(Flag::I, true);
        });
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

void CPU::ReturnFromSubroutine()
{
    m_microInstructionQueue.push([]() { /* Skip cycle */ });
    m_microInstructionQueue.push([this]() { reg_s++; });
    m_microInstructionQueue.push([this]()
        {
            reg_pc = StackPop();
            reg_s++;
        });
    m_microInstructionQueue.push([this]() { reg_pc |= StackPop(); });
    m_microInstructionQueue.push([this]() { reg_pc++; });
}

void CPU::PushA()
{
    m_microInstructionQueue.push([]() { /* Skip cycle */ });
    m_microInstructionQueue.push([this]()
        {
            StackPush(reg_a);
            reg_s--;
        });
}

void CPU::PushP()
{
    m_microInstructionQueue.push([]() { /* Skip cycle */ });
    m_microInstructionQueue.push([this]()
        {
            StackPush(reg_p | static_cast<uint8_t>(Flag::B));
            reg_s--;
        });
}

void CPU::PullA()
{
    m_microInstructionQueue.push([]() { /* Skip cycle */ });
    m_microInstructionQueue.push([this]() { reg_s++; });
    m_microInstructionQueue.push([this]()
        {
            reg_a = StackPop();
            SetFlag(Flag::Z, reg_a == 0);
            SetFlag(Flag::N, reg_a & 0x80);
        });
}

void CPU::PullP()
{
    m_microInstructionQueue.push([]() { /* Skip cycle */ });
    m_microInstructionQueue.push([this]() { reg_s++; });
    m_microInstructionQueue.push([this]()
        {
            reg_p = StackPop();
        });
}

void CPU::JumpToSubroutine()
{
    m_microInstructionQueue.push([this]() { m_targetAddress = Read(reg_pc++); });
    m_microInstructionQueue.push([]() { /* Skip cycle */ });
    m_microInstructionQueue.push([this]()
        {
            StackPush(reg_pc >> 8);
            reg_s--;
        });
    m_microInstructionQueue.push([this]()
        {
            StackPush(reg_pc & 0x00FF);
            reg_s--;
        });
    m_microInstructionQueue.push([this]()
        {
            m_targetAddress |= Read(reg_pc) << 8;
            reg_pc = m_targetAddress;
        });
}

void CPU::AbsoluteReadOnly(std::function<void()> operation)
{
    m_microInstructionQueue.push([this]() { m_targetAddress = Read(reg_pc++); });
    m_microInstructionQueue.push([this]() { m_targetAddress |= Read(reg_pc++) << 8; });
    m_microInstructionQueue.push([this, operation]()
        {
            m_operand = Read(m_targetAddress);
            operation();
        });
}

void CPU::AbsoluteReadModifyWrite(std::function<void()> operation)
{
    m_microInstructionQueue.push([this]() { m_targetAddress = Read(reg_pc++); });
    m_microInstructionQueue.push([this]() { m_targetAddress |= Read(reg_pc++) << 8; });
    m_microInstructionQueue.push([this]() { m_operand = Read(m_targetAddress); });
    m_microInstructionQueue.push([this, operation]() { operation(); });
    m_microInstructionQueue.push([this]() { Write(m_targetAddress, m_operand); });
}

void CPU::AbsoluteWriteOnly(std::function<void()> operation)
{
    m_microInstructionQueue.push([this]() { m_targetAddress = Read(reg_pc++); });
    m_microInstructionQueue.push([this]() { m_targetAddress |= Read(reg_pc++) << 8; });
    m_microInstructionQueue.push([this, operation]() { operation(); });
}

void CPU::ZeroPageReadOnly(std::function<void()> operation)
{
    m_microInstructionQueue.push([this]() { m_targetAddress = Read(reg_pc++); });
    m_microInstructionQueue.push([this, operation]()
        {
            m_operand = Read(m_targetAddress);
            operation();
        });
}
