#include "cpu.h"

void CPU::ReadProgramCounter()
{
    Read(reg_pc);
}

void CPU::IncrementStackPointer()
{
    reg_s++;
}

void CPU::SetTargetAddressLowByteUsingPC()
{
    m_targetAddress = Read(reg_pc++);
}

void CPU::SetTargetAddressHighByteUsingPC()
{
    m_targetAddress |= Read(reg_pc++) << 8;
}

void CPU::SetTargetAddressHighByteUsingXIndexedPC()
{
    uint8_t originalPage = Read(reg_pc++);
    m_targetAddress |= originalPage << 8;
    m_targetAddress += reg_x;

    // Force a read at a possibly invalid address
    if (originalPage == m_targetAddress >> 8)
        m_microInstructionQueue.push_front(&CPU::ReadOperandFromTargetAddress);
    else
        m_microInstructionQueue.push_front(&CPU::PerformInvalidTargetAddressRead);
}

void CPU::SetTargetAddressHighByteUsingYIndexedPC()
{
    uint8_t originalPage = Read(reg_pc++);
    m_targetAddress |= originalPage << 8;
    m_targetAddress += reg_y;

    // Force a read at a possibly invalid address
    if (originalPage == m_targetAddress >> 8)
        m_microInstructionQueue.push_front(&CPU::ReadOperandFromTargetAddress);
    else
        m_microInstructionQueue.push_front(&CPU::PerformInvalidTargetAddressRead);
}

void CPU::AddRegisterXToZeroPageTargetAddress()
{
    m_targetAddress += reg_x;
    m_targetAddress &= 0x00FF;
}

void CPU::AddRegisterYToZeroPageTargetAddress()
{
    m_targetAddress += reg_y;
    m_targetAddress &= 0x00FF;
}

void CPU::ReadOperandAtPCAndPerformOperation()
{
    m_operand = Read(reg_pc++);
    (this->*m_operation)();
}

void CPU::PerformOperationOnRegA()
{
    m_operand = reg_a;
    (this->*m_operation)();
    reg_a = m_operand;
}

void CPU::PerformInvalidTargetAddressRead()
{
    Read(m_targetAddress - 0x100);
}

void CPU::FetchHighByteForAbsoluteReadOnly()
{
    uint8_t originalPage = Read(reg_pc++);
    m_targetAddress |= originalPage << 8;

    switch (m_indexType)
    {
    case IndexType::None:
        return; // No need to continue
    case IndexType::X:
        m_targetAddress += reg_x;
        break;
    case IndexType::Y:
        m_targetAddress += reg_y;
        break;
    default:
        throw std::runtime_error("Unexpected address index type.");
    }

    uint8_t newPage = m_targetAddress >> 8;
    if (originalPage != newPage)
    {
        m_microInstructionQueue.push_front(&CPU::PerformInvalidTargetAddressRead);
    }
}

void CPU::PerformOperationOnTargetAddress()
{
    m_operand = Read(m_targetAddress);
    (this->*m_operation)();
}

void CPU::ReadOperandFromTargetAddress()
{
    m_operand = Read(m_targetAddress);
}

void CPU::WriteOperandToTargetAddress()
{
    Write(m_targetAddress, m_operand);
}

void CPU::ReadOperandAtPCAndIncrementPC()
{
    m_operand = Read(reg_pc++);
}

void CPU::AddRegisterXToOperand()
{
    m_operand += reg_x;
}

void CPU::SetTargetAddressLowByteUsingOperand()
{
    m_targetAddress = Read(m_operand++);
}

void CPU::SetTargetAddressHighByteUsingOperand()
{
    m_targetAddress |= Read(m_operand) << 8;
}

void CPU::FetchHighByteForIndirectIndexedReadOnly()
{
    uint8_t originalPage = Read(m_operand);

    m_targetAddress |= originalPage << 8;
    m_targetAddress += reg_y;

    uint8_t newPage = m_targetAddress >> 8;
    if (originalPage != newPage)
    {
        m_microInstructionQueue.push_front(&CPU::PerformInvalidTargetAddressRead);
    }
}

void CPU::SetTargetAddressHighByteUsingYIndexedOperand()
{
    uint8_t originalPage = Read(m_operand);
    m_targetAddress |= originalPage << 8;
    m_targetAddress += reg_y;

    // Force a read at a possibly invalid address
    if (originalPage == m_targetAddress >> 8)
        m_microInstructionQueue.push_front(&CPU::ReadOperandFromTargetAddress);
    else
        m_microInstructionQueue.push_front(&CPU::PerformInvalidTargetAddressRead);
}

void CPU::CheckBranchCondition()
{
    m_operand = Read(reg_pc++);
    if (!(this->*m_branchTest)()) return;

    m_microInstructionQueue.push_front(&CPU::PerformBranch);
}

void CPU::PerformBranch()
{
    uint8_t originalPage = reg_pc >> 8;
    reg_pc = reg_pc + static_cast<int8_t>(m_operand);

    // Add a blank cycle when a page is crossed
    if (originalPage != (reg_pc >> 8))
        m_microInstructionQueue.push_front(&CPU::BlankMicroInstruction);
}

void CPU::BlankMicroInstruction() {}

void CPU::PopStatusOffTheStackAndIncrementStackPointer()
{
    reg_p = StackPop();
    reg_s++;
}

void CPU::PopPCLowByteOffTheStackAndIncrementStackPointer()
{
    reg_pc = StackPop();
    reg_s++;
}

void CPU::PopPCHighByteOffTheStack()
{
    reg_pc |= StackPop() << 8;
}

void CPU::IncrementProgramCounter()
{
    reg_pc++;
}

void CPU::PopStatusOffTheStack()
{
    reg_p = StackPop();
}

void CPU::PushAccumulatorToTheStack()
{
    StackPush(reg_a);
    reg_s--;
}

void CPU::PushStatusToTheStackWithBSet()
{
    StackPush(reg_p | static_cast<uint8_t>(Flag::B));
    reg_s--;
}

void CPU::PopAccumulatorFromTheStackAndSetFlags()
{
    reg_a = StackPop();
    SetFlag(Flag::Z, reg_a == 0);
    SetFlag(Flag::N, reg_a & 0x80);
}

void CPU::ReadFromTheStackPointer()
{
    Read(0x100 + reg_s);
}

void CPU::PushPCHighByteToTheStack()
{
    StackPush(reg_pc >> 8);
    reg_s--;
}

void CPU::PushPCLowByteToTheStack()
{
    StackPush(static_cast<uint8_t>(reg_pc));
    reg_s--;
}

void CPU::JumpToSubroutineFinal()
{
    m_targetAddress |= Read(reg_pc) << 8;
    reg_pc = m_targetAddress;
}

void CPU::JumpAbsoluteFinal()
{
    uint8_t pch = Read(reg_pc);
    reg_pc = m_operand;
    reg_pc |= pch << 8;
}

void CPU::JumpIndirectFinal()
{
    uint16_t pchAddress;
    if (static_cast<uint8_t>(m_targetAddress) == 0xFF)
        pchAddress = m_targetAddress & 0xFF00;
    else
        pchAddress = m_targetAddress + 1;

    reg_pc = m_operand;
    reg_pc |= Read(pchAddress) << 8;
}

void CPU::PushStatusAndDecideFinalInterruptVector()
{
    // Allows NMI interrupts to hijack the BRK/IRQ interrupt vector
    if (m_inProgressInterruptType == InterruptType::NMI) m_targetAddress = NMI_VECTOR;
    else m_targetAddress = m_mostRecentInterruptVector;

    if (m_inProgressInterruptType == InterruptType::BRK)
        StackPush(reg_p | static_cast<uint8_t>(Flag::B));
    else
        StackPush(reg_p & ~static_cast<uint8_t>(Flag::B));

    reg_s--;
}

void CPU::SetPCLowByteAndSetInterruptFlag()
{
    reg_pc = Read(m_targetAddress);
    SetFlag(Flag::I, true);
}

void CPU::SetPCHighByteAndClearInterruptInProgress()
{
    reg_pc |= Read(m_targetAddress + 1) << 8;
    m_interruptInProgress = false;
}

void CPU::SetInterruptInProgress()
{
    m_interruptInProgress = true;
}

void CPU::SetInterruptInProgressAndIncrementPC()
{
    m_interruptInProgress = true;
    reg_pc++;
}

bool CPU::PlusTest() const
{
    return GetFlag(Flag::N) == 0;
}

bool CPU::MinusTest() const
{
    return GetFlag(Flag::N) == 1;
}

bool CPU::OverflowClearTest() const
{
    return GetFlag(Flag::V) == 0;
}

bool CPU::OverflowSetTest() const
{
    return GetFlag(Flag::V) == 1;
}

bool CPU::CarryClearTest() const
{
    return GetFlag(Flag::C) == 0;
}

bool CPU::CarrySetTest() const
{
    return GetFlag(Flag::C) == 1;
}

bool CPU::NotEqualTest() const
{
    return GetFlag(Flag::Z) == 0;
}

bool CPU::EqualTest() const
{
    return GetFlag(Flag::Z) == 1;
}
