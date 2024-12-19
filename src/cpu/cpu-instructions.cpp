#include "cpu.h"

void CPU::ImmediateReadOnly(std::function<void()> operation)
{
    m_microInstructionQueue.push([this, operation]()
        {
            m_operand = Read(reg_pc++);
            operation();
        });
}

void CPU::AccumulatorReadModifyWrite(std::function<void()> operation)
{
    m_microInstructionQueue.push([this, operation]()
        {
            m_operand = reg_a;
            operation();
            reg_a = m_operand;
        });
}

void CPU::AbsoluteReadOnly(std::function<void()> operation, IndexType indexType)
{
    m_microInstructionQueue.push([this]() { m_targetAddress = Read(reg_pc++); });
    m_microInstructionQueue.push([this, indexType]()
        {
            uint8_t originalPage = Read(reg_pc++);
            m_targetAddress |= originalPage << 8;

            switch (indexType)
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
                m_skipNextCycle = true;
            }
        });
    m_microInstructionQueue.push([this, operation]()
        {
            m_operand = Read(m_targetAddress);
            operation();
        });
}

void CPU::AbsoluteReadModifyWrite(std::function<void()> operation, IndexType indexType)
{
    m_microInstructionQueue.push([this]() { m_targetAddress = Read(reg_pc++); });

    switch (indexType)
    {
        case IndexType::None:
            m_microInstructionQueue.push([this]() { m_targetAddress |= Read(reg_pc++) << 8; });
            break;
        case IndexType::X:
            m_microInstructionQueue.push([this]()
                {
                    m_targetAddress |= Read(reg_pc++) << 8;
                    m_targetAddress += reg_x;
                    m_skipNextCycle = true;
                });
            break;
        case IndexType::Y: // Unexpected
        default:
            throw std::runtime_error("Unexpected address index type.");
    }

    m_microInstructionQueue.push([this]() { m_operand = Read(m_targetAddress); });
    m_microInstructionQueue.push(operation);
    m_microInstructionQueue.push([this]() { Write(m_targetAddress, m_operand); });
}

void CPU::AbsoluteWriteOnly(std::function<void()> operation, IndexType indexType)
{
    m_microInstructionQueue.push([this]() { m_targetAddress = Read(reg_pc++); });
    
    switch (indexType)
    {
        case IndexType::None:
            m_microInstructionQueue.push([this]() { m_targetAddress |= Read(reg_pc++) << 8; });
            break;
        case IndexType::X:
            m_microInstructionQueue.push([this]()
                {
                    m_targetAddress |= Read(reg_pc++) << 8;
                    m_targetAddress += reg_x;
                    m_skipNextCycle = true;
                });
            break;
        case IndexType::Y:
            m_microInstructionQueue.push([this]()
                {
                    m_targetAddress |= Read(reg_pc++) << 8;
                    m_targetAddress += reg_y;
                    m_skipNextCycle = true;
                });
            break;
        default:
            throw std::runtime_error("Unexpected address index type.");
    }

    m_microInstructionQueue.push(operation);
}

void CPU::ZeroPageReadOnly(std::function<void()> operation, IndexType indexType)
{
    m_microInstructionQueue.push([this]() { m_targetAddress = Read(reg_pc++); });

    switch (indexType)
    {
        case IndexType::None:
            // Do nothing
            break;
        case IndexType::X:
            m_microInstructionQueue.push([this]() { m_targetAddress += reg_x; });
            break;
        case IndexType::Y:
            m_microInstructionQueue.push([this]() { m_targetAddress += reg_y; });
            break;
        default:
            throw std::runtime_error("Unexpected address index type.");
    }

    m_microInstructionQueue.push([this, operation]()
        {
            m_operand = Read(m_targetAddress);
            operation();
        });
}

void CPU::ZeroPageReadModifyWrite(std::function<void()> operation, IndexType indexType)
{
    m_microInstructionQueue.push([this]() { m_targetAddress = Read(reg_pc++); });

    switch (indexType)
    {
        case IndexType::None:
            // Do nothing
            break;
        case IndexType::X:
            m_microInstructionQueue.push([this]() { m_targetAddress += reg_x; });
            break;
        case IndexType::Y: // Unexpected
        default:
            throw std::runtime_error("Unexpected address index type.");
    }

    m_microInstructionQueue.push([this]() { m_operand = Read(m_targetAddress); });
    m_microInstructionQueue.push(operation);
    m_microInstructionQueue.push([this]() { Write(m_targetAddress, m_operand); });
}

void CPU::ZeroPageWriteOnly(std::function<void()> operation, IndexType indexType)
{
    m_microInstructionQueue.push([this]() { m_targetAddress = Read(reg_pc++); });

    switch (indexType)
    {
        case IndexType::None:
            // Do nothing
            break;
        case IndexType::X:
            m_microInstructionQueue.push([this]() { m_targetAddress += reg_x; });
            break;
        case IndexType::Y:
            m_microInstructionQueue.push([this]() { m_targetAddress += reg_y; });
            break;
        default:
            throw std::runtime_error("Unexpected address index type.");
    }

    m_microInstructionQueue.push(operation);
}

void CPU::IndexedIndirectReadOnly(std::function<void()> operation)
{
    // Temporarily uses the operand variable to store a pointer
    m_microInstructionQueue.push([this]() { m_operand = Read(reg_pc++); });
    m_microInstructionQueue.push([this]() { m_operand += reg_x; });
    m_microInstructionQueue.push([this]() { m_targetAddress = Read(m_operand++); });
    m_microInstructionQueue.push([this]() { m_targetAddress |= Read(m_operand) << 8; });
    m_microInstructionQueue.push([this, operation]()
        {
            m_operand = Read(m_targetAddress);
            operation();
        });
}

void CPU::IndexedIndirectReadModifyWrite(std::function<void()> operation)
{
    // Temporarily uses the operand variable to store a pointer
    m_microInstructionQueue.push([this]() { m_operand = Read(reg_pc++); });
    m_microInstructionQueue.push([this]() { m_operand += reg_x; });
    m_microInstructionQueue.push([this]() { m_targetAddress = Read(m_operand++); });
    m_microInstructionQueue.push([this]() { m_targetAddress |= Read(m_operand) << 8; });
    m_microInstructionQueue.push([this]() { m_operand = Read(m_targetAddress); });
    m_microInstructionQueue.push(operation);
    m_microInstructionQueue.push([this]() { Write(m_targetAddress, m_operand); });
}

void CPU::IndexedIndirectWriteOnly(std::function<void()> operation)
{
    // Temporarily uses the operand variable to store a pointer
    m_microInstructionQueue.push([this]() { m_operand = Read(reg_pc++); });
    m_microInstructionQueue.push([this]() { m_operand += reg_x; });
    m_microInstructionQueue.push([this]() { m_targetAddress = Read(m_operand++); });
    m_microInstructionQueue.push([this]() { m_targetAddress |= Read(m_operand) << 8; });
    m_microInstructionQueue.push(operation);
}

void CPU::IndirectIndexedReadOnly(std::function<void()> operation)
{
    // Temporarily uses the operand variable to store a pointer
    m_microInstructionQueue.push([this]() { m_operand = Read(reg_pc++); });
    m_microInstructionQueue.push([this]() { m_targetAddress = Read(m_operand++); });
    m_microInstructionQueue.push([this]()
        {
            uint8_t originalPage = Read(m_operand);

            m_targetAddress |= originalPage << 8;
            m_targetAddress += reg_y;

            uint8_t newPage = m_targetAddress >> 8;
            if (originalPage != newPage)
            {
                m_skipNextCycle = true;
            }
        });
    m_microInstructionQueue.push([this, operation]()
        {
            m_operand = Read(m_targetAddress);
            operation();
        });
}

void CPU::IndirectIndexedReadModifyWrite(std::function<void()> operation)
{
    // Temporarily uses the operand variable to store a pointer
    m_microInstructionQueue.push([this]() { m_operand = Read(reg_pc++); });
    m_microInstructionQueue.push([this]() { m_targetAddress = Read(m_operand++); });
    m_microInstructionQueue.push([this]()
        {
            m_targetAddress |= Read(m_operand) << 8;
            m_targetAddress += reg_y;
            m_skipNextCycle = true;
        });
    m_microInstructionQueue.push([this]() { m_operand = Read(m_targetAddress); });
    m_microInstructionQueue.push(operation);
    m_microInstructionQueue.push([this]() { Write(m_targetAddress, m_operand); });
}

void CPU::IndirectIndexedWriteOnly(std::function<void()> operation)
{
    // Temporarily uses the operand variable to store a pointer
    m_microInstructionQueue.push([this]() { m_operand = Read(reg_pc++); });
    m_microInstructionQueue.push([this]() { m_targetAddress = Read(m_operand++); });
    m_microInstructionQueue.push([this]()
        {
            m_targetAddress |= Read(m_operand) << 8;
            m_targetAddress += reg_y;
            m_skipNextCycle = true;
        });
    m_microInstructionQueue.push(operation);
}

void CPU::ADC()
{
    uint16_t result = reg_a + m_operand + GetFlag(Flag::C);

    SetFlag(Flag::C, result > 0xFF);
    SetFlag(Flag::Z, result == 0);
    SetFlag(Flag::V, (result ^ reg_a) & (result ^ m_operand) & 0x80);
    SetFlag(Flag::N, result & 0x80);

    reg_a = static_cast<uint8_t>(result);
}

void CPU::AND()
{
    reg_a &= m_operand;

    SetFlag(Flag::Z, reg_a == 0);
    SetFlag(Flag::N, reg_a & 0x80);
}

void CPU::ASL()
{
    SetFlag(Flag::C, m_operand & 0x80);

    m_operand = m_operand << 1;

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

void CPU::BRK()
{
    m_microInstructionQueue.push([this]() { reg_pc++; });
    m_microInstructionQueue.push([this]()
        {
            StackPush(reg_pc >> 8);
            reg_s--;
        });
    m_microInstructionQueue.push([this]()
        {
            StackPush(static_cast<uint8_t>(reg_pc));
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

void CPU::RTI()
{
    m_skipNextCycle = true;
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

void CPU::RTS()
{
    m_skipNextCycle = true;
    m_microInstructionQueue.push([this]() { reg_s++; });
    m_microInstructionQueue.push([this]()
        {
            reg_pc = StackPop();
            reg_s++;
        });
    m_microInstructionQueue.push([this]() { reg_pc |= StackPop(); });
    m_microInstructionQueue.push([this]() { reg_pc++; });
}

void CPU::PHA()
{
    m_skipNextCycle = true;
    m_microInstructionQueue.push([this]()
        {
            StackPush(reg_a);
            reg_s--;
        });
}

void CPU::PHP()
{
    m_skipNextCycle = true;
    m_microInstructionQueue.push([this]()
        {
            StackPush(reg_p | static_cast<uint8_t>(Flag::B));
            reg_s--;
        });
}

void CPU::PLA()
{
    m_skipNextCycle = true;
    m_microInstructionQueue.push([this]() { reg_s++; });
    m_microInstructionQueue.push([this]()
        {
            reg_a = StackPop();
            SetFlag(Flag::Z, reg_a == 0);
            SetFlag(Flag::N, reg_a & 0x80);
        });
}

void CPU::PLP()
{
    m_skipNextCycle = true;
    m_microInstructionQueue.push([this]() { reg_s++; });
    m_microInstructionQueue.push([this]()
        {
            reg_p = StackPop();
        });
}

void CPU::JSR()
{
    m_microInstructionQueue.push([this]()
        {
            m_targetAddress = Read(reg_pc++);
            m_skipNextCycle = true;
        });
    m_microInstructionQueue.push([this]()
        {
            StackPush(reg_pc >> 8);
            reg_s--;
        });
    m_microInstructionQueue.push([this]()
        {
            StackPush(static_cast<uint8_t>(reg_pc));
            reg_s--;
        });
    m_microInstructionQueue.push([this]()
        {
            m_targetAddress |= Read(reg_pc) << 8;
            reg_pc = m_targetAddress;
        });
}
