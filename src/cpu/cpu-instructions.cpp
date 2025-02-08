#include "cpu.h"

void CPU::ImmediateReadOnly()
{
    m_microInstructionQueue.push_back(&CPU::ReadOperandAtPCAndPerformOperation);
}

void CPU::AccumulatorReadModifyWrite()
{
    m_microInstructionQueue.push_back(&CPU::PerformOperationOnRegA);
}

void CPU::AbsoluteReadOnly()
{
    m_microInstructionQueue.push_back(&CPU::SetTargetAddressLowByteUsingPC);
    m_microInstructionQueue.push_back(&CPU::FetchHighByteForAbsoluteReadOnly);
    m_microInstructionQueue.push_back(&CPU::PerformOperationOnTargetAddress);
}

void CPU::AbsoluteReadModifyWrite()
{
    m_microInstructionQueue.push_back(&CPU::SetTargetAddressLowByteUsingPC);

    switch (m_indexType)
    {
    case IndexType::None:
        m_microInstructionQueue.push_back(&CPU::SetTargetAddressHighByteUsingPC);
        break;
    case IndexType::X:
        m_microInstructionQueue.push_back(&CPU::SetTargetAddressHighByteUsingXIndexedPC);
        break;
    case IndexType::Y: // Unexpected
    default:
        throw std::runtime_error("Unexpected address index type.");
    }

    m_microInstructionQueue.push_back(&CPU::ReadOperandFromTargetAddress);
    m_microInstructionQueue.push_back(m_operation);
    m_microInstructionQueue.push_back(&CPU::WriteOperandToTargetAddress);
}

void CPU::AbsoluteWriteOnly()
{
    m_microInstructionQueue.push_back(&CPU::SetTargetAddressLowByteUsingPC);
    
    switch (m_indexType)
    {
    case IndexType::None:
        m_microInstructionQueue.push_back(&CPU::SetTargetAddressHighByteUsingPC);
        break;
    case IndexType::X:
        m_microInstructionQueue.push_back(&CPU::SetTargetAddressHighByteUsingXIndexedPC);
        break;
    case IndexType::Y:
        m_microInstructionQueue.push_back(&CPU::SetTargetAddressHighByteUsingYIndexedPC);
        break;
    default:
        throw std::runtime_error("Unexpected address index type.");
    }

    m_microInstructionQueue.push_back(m_operation);
}

void CPU::ZeroPageReadOnly()
{
    m_microInstructionQueue.push_back(&CPU::SetTargetAddressLowByteUsingPC);

    switch (m_indexType)
    {
    case IndexType::None:
        // Do nothing
        break;
    case IndexType::X:
        m_microInstructionQueue.push_back(&CPU::AddRegisterXToZeroPageTargetAddress);
        break;
    case IndexType::Y:
        m_microInstructionQueue.push_back(&CPU::AddRegisterYToZeroPageTargetAddress);
        break;
    default:
        throw std::runtime_error("Unexpected address index type.");
    }

    m_microInstructionQueue.push_back(&CPU::PerformOperationOnTargetAddress);
}

void CPU::ZeroPageReadModifyWrite()
{
    m_microInstructionQueue.push_back(&CPU::SetTargetAddressLowByteUsingPC);

    switch (m_indexType)
    {
    case IndexType::None:
        // Do nothing
        break;
    case IndexType::X:
        m_microInstructionQueue.push_back(&CPU::AddRegisterXToZeroPageTargetAddress);
        break;
    case IndexType::Y: // Unexpected
    default:
        throw std::runtime_error("Unexpected address index type.");
    }

    m_microInstructionQueue.push_back(&CPU::ReadOperandFromTargetAddress);
    m_microInstructionQueue.push_back(m_operation);
    m_microInstructionQueue.push_back(&CPU::WriteOperandToTargetAddress);
}

void CPU::ZeroPageWriteOnly()
{
    m_microInstructionQueue.push_back(&CPU::SetTargetAddressLowByteUsingPC);

    switch (m_indexType)
    {
    case IndexType::None:
        // Do nothing
        break;
    case IndexType::X:
        m_microInstructionQueue.push_back(&CPU::AddRegisterXToZeroPageTargetAddress);
        break;
    case IndexType::Y:
        m_microInstructionQueue.push_back(&CPU::AddRegisterYToZeroPageTargetAddress);
        break;
    default:
        throw std::runtime_error("Unexpected address index type.");
    }

    m_microInstructionQueue.push_back(m_operation);
}

void CPU::IndexedIndirectReadOnly()
{
    // Temporarily uses the operand variable to store a pointer
    m_microInstructionQueue.push_back(&CPU::ReadOperandAtPCAndIncrementPC);
    m_microInstructionQueue.push_back(&CPU::AddRegisterXToOperand);
    m_microInstructionQueue.push_back(&CPU::SetTargetAddressLowByteUsingOperand);
    m_microInstructionQueue.push_back(&CPU::SetTargetAddressHighByteUsingOperand);
    m_microInstructionQueue.push_back(&CPU::PerformOperationOnTargetAddress);
}

void CPU::IndexedIndirectReadModifyWrite()
{
    // Temporarily uses the operand variable to store a pointer
    m_microInstructionQueue.push_back(&CPU::ReadOperandAtPCAndIncrementPC);
    m_microInstructionQueue.push_back(&CPU::AddRegisterXToOperand);
    m_microInstructionQueue.push_back(&CPU::SetTargetAddressLowByteUsingOperand);
    m_microInstructionQueue.push_back(&CPU::SetTargetAddressHighByteUsingOperand);
    m_microInstructionQueue.push_back(&CPU::ReadOperandFromTargetAddress);
    m_microInstructionQueue.push_back(m_operation);
    m_microInstructionQueue.push_back(&CPU::WriteOperandToTargetAddress);
}

void CPU::IndexedIndirectWriteOnly()
{
    // Temporarily uses the operand variable to store a pointer
    m_microInstructionQueue.push_back(&CPU::ReadOperandAtPCAndIncrementPC);
    m_microInstructionQueue.push_back(&CPU::AddRegisterXToOperand);
    m_microInstructionQueue.push_back(&CPU::SetTargetAddressLowByteUsingOperand);
    m_microInstructionQueue.push_back(&CPU::SetTargetAddressHighByteUsingOperand);
    m_microInstructionQueue.push_back(m_operation);
}

void CPU::IndirectIndexedReadOnly()
{
    // Temporarily uses the operand variable to store a pointer
    m_microInstructionQueue.push_back(&CPU::ReadOperandAtPCAndIncrementPC);
    m_microInstructionQueue.push_back(&CPU::SetTargetAddressLowByteUsingOperand);
    m_microInstructionQueue.push_back(&CPU::FetchHighByteForIndirectIndexedReadOnly);
    m_microInstructionQueue.push_back(&CPU::PerformOperationOnTargetAddress);
}

void CPU::IndirectIndexedReadModifyWrite()
{
    // Temporarily uses the operand variable to store a pointer
    m_microInstructionQueue.push_back(&CPU::ReadOperandAtPCAndIncrementPC);
    m_microInstructionQueue.push_back(&CPU::SetTargetAddressLowByteUsingOperand);
    m_microInstructionQueue.push_back(&CPU::SetTargetAddressHighByteUsingYIndexedOperand);
    m_microInstructionQueue.push_back(&CPU::ReadOperandFromTargetAddress);
    m_microInstructionQueue.push_back(m_operation);
    m_microInstructionQueue.push_back(&CPU::WriteOperandToTargetAddress);
}

void CPU::IndirectIndexedWriteOnly()
{
    // Temporarily uses the operand variable to store a pointer
    m_microInstructionQueue.push_back(&CPU::ReadOperandAtPCAndIncrementPC);
    m_microInstructionQueue.push_back(&CPU::SetTargetAddressLowByteUsingOperand);
    m_microInstructionQueue.push_back(&CPU::SetTargetAddressHighByteUsingYIndexedOperand);
    m_microInstructionQueue.push_back(m_operation);
}

void CPU::BranchInstruction()
{
    m_microInstructionQueue.push_back(&CPU::CheckBranchCondition);
}

void CPU::ADC()
{
    uint16_t result = reg_a + m_operand + GetFlag(Flag::C);

    SetFlag(Flag::C, result > 0xFF);
    SetFlag(Flag::Z, (result & 0x00FF) == 0);
    SetFlag(Flag::V, (result ^ reg_a) & (result ^ m_operand) & 0x80);
    SetFlag(Flag::N, result & 0x80);

    reg_a = static_cast<uint8_t>(result);
}

void CPU::SBC()
{
    uint16_t result = reg_a + ~m_operand + GetFlag(Flag::C);

    SetFlag(Flag::C, !(result > 0xFF));
    SetFlag(Flag::Z, (result & 0x00FF) == 0);
    SetFlag(Flag::V, (result ^ reg_a) & (result ^ ~m_operand) & 0x80);
    SetFlag(Flag::N, result & 0x80);

    reg_a = static_cast<uint8_t>(result);
}

void CPU::INC()
{
    m_operand++;

    SetFlag(Flag::Z, m_operand == 0);
    SetFlag(Flag::N, m_operand & 0x80);
}

void CPU::DEC()
{
    m_operand--;

    SetFlag(Flag::Z, m_operand == 0);
    SetFlag(Flag::N, m_operand & 0x80);
}

void CPU::AND()
{
    reg_a &= m_operand;

    SetFlag(Flag::Z, reg_a == 0);
    SetFlag(Flag::N, reg_a & 0x80);
}

void CPU::ORA()
{
    reg_a |= m_operand;

    SetFlag(Flag::Z, reg_a == 0);
    SetFlag(Flag::N, reg_a & 0x80);
}

void CPU::EOR()
{
    reg_a ^= m_operand;

    SetFlag(Flag::Z, reg_a == 0);
    SetFlag(Flag::N, reg_a & 0x80);
}

void CPU::BIT()
{
    SetFlag(Flag::Z, (reg_a & m_operand) == 0);
    SetFlag(Flag::V, m_operand & 0x40);
    SetFlag(Flag::N, m_operand & 0x80);
}

void CPU::ASL()
{
    SetFlag(Flag::C, m_operand & 0x80);

    m_operand = m_operand << 1;

    SetFlag(Flag::Z, m_operand == 0);
    SetFlag(Flag::N, m_operand & 0x80);
}

void CPU::LSR()
{
    SetFlag(Flag::C, m_operand & 0x01);

    m_operand = m_operand >> 1;

    SetFlag(Flag::Z, m_operand == 0);
    SetFlag(Flag::N, m_operand & 0x80);
}

void CPU::ROL()
{
    uint8_t originalC = GetFlag(Flag::C);
    SetFlag(Flag::C, m_operand & 0x80);

    m_operand = (m_operand << 1) | originalC;

    SetFlag(Flag::Z, m_operand == 0);
    SetFlag(Flag::N, m_operand & 0x80);
}

void CPU::ROR()
{
    uint8_t originalC = GetFlag(Flag::C);
    SetFlag(Flag::C, m_operand & 0x01);

    m_operand = (m_operand >> 1) | (originalC << 7);

    SetFlag(Flag::Z, m_operand == 0);
    SetFlag(Flag::N, m_operand & 0x80);
}

void CPU::LDA()
{
    reg_a = m_operand;

    SetFlag(Flag::Z, reg_a == 0);
    SetFlag(Flag::N, reg_a & 0x80);
}

void CPU::LDX()
{
    reg_x = m_operand;

    SetFlag(Flag::Z, reg_x == 0);
    SetFlag(Flag::N, reg_x & 0x80);
}

void CPU::LDY()
{
    reg_y = m_operand;

    SetFlag(Flag::Z, reg_y == 0);
    SetFlag(Flag::N, reg_y & 0x80);
}

void CPU::STA()
{
    Write(m_targetAddress, reg_a);
}

void CPU::STX()
{
    Write(m_targetAddress, reg_x);
}

void CPU::STY()
{
    Write(m_targetAddress, reg_y);
}

void CPU::CMP()
{
    SetFlag(Flag::C, reg_a >= m_operand);
    SetFlag(Flag::Z, reg_a == m_operand);
    SetFlag(Flag::N, (reg_a - m_operand) & 0x80);
}

void CPU::CPX()
{
    SetFlag(Flag::C, reg_x >= m_operand);
    SetFlag(Flag::Z, reg_x == m_operand);
    SetFlag(Flag::N, (reg_x - m_operand) & 0x80);
}

void CPU::CPY()
{
    SetFlag(Flag::C, reg_y >= m_operand);
    SetFlag(Flag::Z, reg_y == m_operand);
    SetFlag(Flag::N, (reg_y - m_operand) & 0x80);
}

void CPU::RTI()
{
    m_microInstructionQueue.push_back(&CPU::ReadProgramCounter);
    m_microInstructionQueue.push_back(&CPU::IncrementStackPointer);
    m_microInstructionQueue.push_back(&CPU::PopStatusOffTheStackAndIncrementStackPointer);
    m_microInstructionQueue.push_back(&CPU::PopPCLowByteOffTheStackAndIncrementStackPointer);
    m_microInstructionQueue.push_back(&CPU::PopPCHighByteOffTheStack);
}

void CPU::RTS()
{
    m_microInstructionQueue.push_back(&CPU::ReadProgramCounter);
    m_microInstructionQueue.push_back(&CPU::IncrementStackPointer);
    m_microInstructionQueue.push_back(&CPU::PopPCLowByteOffTheStackAndIncrementStackPointer);
    m_microInstructionQueue.push_back(&CPU::PopPCHighByteOffTheStack);
    m_microInstructionQueue.push_back(&CPU::IncrementProgramCounter);
}

void CPU::PHA()
{
    m_microInstructionQueue.push_back(&CPU::ReadProgramCounter);
    m_microInstructionQueue.push_back(&CPU::PushAccumulatorToTheStack);
}

void CPU::PHP()
{
    m_microInstructionQueue.push_back(&CPU::ReadProgramCounter);
    m_microInstructionQueue.push_back(&CPU::PushStatusToTheStackWithBSet);
}

void CPU::PLA()
{
    m_microInstructionQueue.push_back(&CPU::ReadProgramCounter);
    m_microInstructionQueue.push_back(&CPU::IncrementStackPointer);
    m_microInstructionQueue.push_back(&CPU::PopAccumulatorFromTheStackAndSetFlags);
}

void CPU::PLP()
{
    m_microInstructionQueue.push_back(&CPU::ReadProgramCounter);
    m_microInstructionQueue.push_back(&CPU::IncrementStackPointer);
    m_microInstructionQueue.push_back(&CPU::PopStatusOffTheStack);
}

void CPU::JSR()
{
    m_microInstructionQueue.push_back(&CPU::SetTargetAddressLowByteUsingPC);
    m_microInstructionQueue.push_back(&CPU::ReadFromTheStackPointer);
    m_microInstructionQueue.push_back(&CPU::PushPCHighByteToTheStack);
    m_microInstructionQueue.push_back(&CPU::PushPCLowByteToTheStack);
    m_microInstructionQueue.push_back(&CPU::JumpToSubroutineFinal);
}

void CPU::TAX()
{
    reg_x = reg_a;

    SetFlag(Flag::Z, reg_x == 0);
    SetFlag(Flag::N, reg_x & 0x80);
}

void CPU::TXA()
{
    reg_a = reg_x;

    SetFlag(Flag::Z, reg_a == 0);
    SetFlag(Flag::N, reg_a & 0x80);
}

void CPU::TAY()
{
    reg_y = reg_a;

    SetFlag(Flag::Z, reg_y == 0);
    SetFlag(Flag::N, reg_y & 0x80);
}

void CPU::TYA()
{
    reg_a = reg_y;

    SetFlag(Flag::Z, reg_a == 0);
    SetFlag(Flag::N, reg_a & 0x80);
}

void CPU::TXS()
{
    reg_s = reg_x;
}

void CPU::TSX()
{
    reg_x = reg_s;

    SetFlag(Flag::Z, reg_x == 0);
    SetFlag(Flag::N, reg_x & 0x80);
}

void CPU::INX()
{
    reg_x++;

    SetFlag(Flag::Z, reg_x == 0);
    SetFlag(Flag::N, reg_x & 0x80);
}

void CPU::DEX()
{
    reg_x--;

    SetFlag(Flag::Z, reg_x == 0);
    SetFlag(Flag::N, reg_x & 0x80);
}

void CPU::INY()
{
    reg_y++;

    SetFlag(Flag::Z, reg_y == 0);
    SetFlag(Flag::N, reg_y & 0x80);
}

void CPU::DEY()
{
    reg_y--;

    SetFlag(Flag::Z, reg_y == 0);
    SetFlag(Flag::N, reg_y & 0x80);
}

void CPU::SEC()
{
    SetFlag(Flag::C, true);
}

void CPU::CLC()
{
    SetFlag(Flag::C, false);
}

void CPU::SEI()
{
    SetFlag(Flag::I, true);
}

void CPU::CLI()
{
    SetFlag(Flag::I, false);
}

void CPU::SED()
{
    SetFlag(Flag::D, true);
}

void CPU::CLD()
{
    SetFlag(Flag::D, false);
}

void CPU::CLV()
{
    SetFlag(Flag::V, false);
}

void CPU::JMP_Absolute()
{
    m_microInstructionQueue.push_back(&CPU::ReadOperandAtPCAndIncrementPC);
    m_microInstructionQueue.push_back(&CPU::JumpAbsoluteFinal);
}

void CPU::JMP_Indirect()
{
    m_microInstructionQueue.push_back(&CPU::SetTargetAddressLowByteUsingPC);
    m_microInstructionQueue.push_back(&CPU::SetTargetAddressHighByteUsingPC);
    m_microInstructionQueue.push_back(&CPU::ReadOperandFromTargetAddress);
    m_microInstructionQueue.push_back(&CPU::JumpIndirectFinal);
}
