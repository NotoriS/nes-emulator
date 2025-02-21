#include "cpu.h"

CPU::CPU(std::shared_ptr<IBus> bus) : m_bus(bus)
{
    Reset();
}

CPU::~CPU() {}

void CPU::Reset()
{
    while (!m_microInstructionQueue.empty()) m_microInstructionQueue.pop_front();

    reg_pc = Read(RESET_VECTOR);
    reg_pc |= Read(RESET_VECTOR + 1) << 8;

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
        (this->*instruction)();
    }

    if (m_microInstructionQueue.size() == 1 && !m_interruptInProgress)
        PollInterrupts();
}

void CPU::Interrupt(InterruptType type)
{
    switch (type)
    {
    case InterruptType::NMI:
        m_pendingNMI = true;
        m_mostRecentInterruptVector = NMI_VECTOR;
        break;
    case InterruptType::IRQ:
        m_pendingIRQ = true;
        m_mostRecentInterruptVector = IRQ_VECTOR;
        break;
    default:
        break;
    }
}

uint8_t CPU::GetFlag(Flag flag) const
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
    Write(STACK_BASE | reg_s, value);
}

uint8_t CPU::StackPop()
{
    return Read(STACK_BASE | reg_s);
}

void CPU::PollInterrupts()
{
    if (m_pendingNMI)
    {
        InternalInterrupt(InterruptType::NMI);
        m_pendingNMI = false;
        m_pendingIRQ = false;
    }
    else if (m_pendingIRQ && !GetFlag(Flag::I))
    {
        InternalInterrupt(InterruptType::IRQ);
        m_pendingIRQ = false;
    }
}

void CPU::InternalInterrupt(InterruptType type)
{
    m_inProgressInterruptType = type;
    if (type != InterruptType::BRK)
    {
        m_microInstructionQueue.push_back(&CPU::SetInterruptInProgress);
    }
    else
    {
        m_mostRecentInterruptVector = IRQ_VECTOR;
        m_microInstructionQueue.push_back(&CPU::SetInterruptInProgressAndIncrementPC);
    }

    m_microInstructionQueue.push_back(&CPU::PushPCHighByteToTheStack);
    m_microInstructionQueue.push_back(&CPU::PushPCLowByteToTheStack);
    m_microInstructionQueue.push_back(&CPU::PushStatusAndDecideFinalInterruptVector);
    m_microInstructionQueue.push_back(&CPU::SetPCLowByteAndSetInterruptFlag);
    m_microInstructionQueue.push_back(&CPU::SetPCHighByteAndClearInterruptInProgress);
}
