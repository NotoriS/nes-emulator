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
        instruction();
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
    if (type != InterruptType::BRK)
    {
        m_microInstructionQueue.push_back([this]() { m_interruptInProgress = true; });
    }
    else
    {
        m_mostRecentInterruptVector = IRQ_VECTOR;
        m_microInstructionQueue.push_back([this]()
            {
                m_interruptInProgress = true;
                reg_pc++;
            });
    }

    m_microInstructionQueue.push_back([this]()
        {
            StackPush(reg_pc >> 8);
            reg_s--;
        });
    m_microInstructionQueue.push_back([this]()
        {
            StackPush(static_cast<uint8_t>(reg_pc));
            reg_s--;
        });
    m_microInstructionQueue.push_back([this, type]()
        {
            // Allows NMI interrupts to hijack the BRK/IRQ interrupt vector
            if (type == InterruptType::NMI) m_targetAddress = NMI_VECTOR;
            else m_targetAddress = m_mostRecentInterruptVector;

            if (type == InterruptType::BRK)
                StackPush(reg_p | static_cast<uint8_t>(Flag::B));
            else
                StackPush(reg_p & ~static_cast<uint8_t>(Flag::B));

            reg_s--;
        });
    m_microInstructionQueue.push_back([this]()
        {
            reg_pc = Read(m_targetAddress);
            SetFlag(Flag::I, true);
        });
    m_microInstructionQueue.push_back([this]()
        {
            reg_pc |= Read(m_targetAddress + 1) << 8;
            m_interruptInProgress = false;
        });
}
