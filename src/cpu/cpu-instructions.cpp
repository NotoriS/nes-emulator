#include "cpu.h"

void CPU::ImmediateReadOnly(std::function<void()> operation)
{
    m_microInstructionQueue.push_back([this, operation]()
        {
            m_operand = Read(reg_pc++);
            operation();
        });
}

void CPU::AccumulatorReadModifyWrite(std::function<void()> operation)
{
    m_microInstructionQueue.push_back([this, operation]()
        {
            m_operand = reg_a;
            operation();
            reg_a = m_operand;
        });
}

void CPU::AbsoluteReadOnly(std::function<void()> operation, IndexType indexType)
{
    m_microInstructionQueue.push_back([this]() { m_targetAddress = Read(reg_pc++); });
    m_microInstructionQueue.push_back([this, indexType]()
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
                // Force an invalid read cycle
                m_microInstructionQueue.push_front([this] { Read(m_targetAddress - 0x100); });
            }
        });
    m_microInstructionQueue.push_back([this, operation]()
        {
            m_operand = Read(m_targetAddress);
            operation();
        });
}

void CPU::AbsoluteReadModifyWrite(std::function<void()> operation, IndexType indexType)
{
    m_microInstructionQueue.push_back([this]() { m_targetAddress = Read(reg_pc++); });

    switch (indexType)
    {
        case IndexType::None:
            m_microInstructionQueue.push_back([this]() { m_targetAddress |= Read(reg_pc++) << 8; });
            break;
        case IndexType::X:
            m_microInstructionQueue.push_back([this]()
                {
                    uint8_t originalPage = Read(reg_pc++);
                    m_targetAddress |= originalPage << 8;
                    m_targetAddress += reg_x;

                    // Force a read at a possibly invalid address
                    m_microInstructionQueue.push_front([this, originalPage]
                        {
                            if (originalPage == m_targetAddress >> 8)
                                Read(m_targetAddress);
                            else
                                Read(m_targetAddress - 0x100);
                        });
                });
            break;
        case IndexType::Y: // Unexpected
        default:
            throw std::runtime_error("Unexpected address index type.");
    }

    m_microInstructionQueue.push_back([this]() { m_operand = Read(m_targetAddress); });
    m_microInstructionQueue.push_back(operation);
    m_microInstructionQueue.push_back([this]() { Write(m_targetAddress, m_operand); });
}

void CPU::AbsoluteWriteOnly(std::function<void()> operation, IndexType indexType)
{
    m_microInstructionQueue.push_back([this]() { m_targetAddress = Read(reg_pc++); });
    
    switch (indexType)
    {
        case IndexType::None:
            m_microInstructionQueue.push_back([this]() { m_targetAddress |= Read(reg_pc++) << 8; });
            break;
        case IndexType::X:
            m_microInstructionQueue.push_back([this]()
                {
                    uint8_t originalPage = Read(reg_pc++);
                    m_targetAddress |= originalPage << 8;
                    m_targetAddress += reg_x;

                    // Force a read at a possibly invalid address
                    m_microInstructionQueue.push_front([this, originalPage]
                        {
                            if (originalPage == m_targetAddress >> 8)
                                Read(m_targetAddress);
                            else
                                Read(m_targetAddress - 0x100);
                        });
                });
            break;
        case IndexType::Y:
            m_microInstructionQueue.push_back([this]()
                {
                    uint8_t originalPage = Read(reg_pc++);
                    m_targetAddress |= originalPage << 8;
                    m_targetAddress += reg_y;

                    // Force a read at a possibly invalid address
                    m_microInstructionQueue.push_front([this, originalPage]
                        {
                            if (originalPage == m_targetAddress >> 8)
                                Read(m_targetAddress);
                            else
                                Read(m_targetAddress - 0x100);
                        });
                });
            break;
        default:
            throw std::runtime_error("Unexpected address index type.");
    }

    m_microInstructionQueue.push_back(operation);
}

void CPU::ZeroPageReadOnly(std::function<void()> operation, IndexType indexType)
{
    m_microInstructionQueue.push_back([this]() { m_targetAddress = Read(reg_pc++); });

    switch (indexType)
    {
        case IndexType::None:
            // Do nothing
            break;
        case IndexType::X:
            m_microInstructionQueue.push_back([this]() { m_targetAddress += reg_x; });
            break;
        case IndexType::Y:
            m_microInstructionQueue.push_back([this]() { m_targetAddress += reg_y; });
            break;
        default:
            throw std::runtime_error("Unexpected address index type.");
    }

    m_microInstructionQueue.push_back([this, operation]()
        {
            m_operand = Read(m_targetAddress);
            operation();
        });
}

void CPU::ZeroPageReadModifyWrite(std::function<void()> operation, IndexType indexType)
{
    m_microInstructionQueue.push_back([this]() { m_targetAddress = Read(reg_pc++); });

    switch (indexType)
    {
        case IndexType::None:
            // Do nothing
            break;
        case IndexType::X:
            m_microInstructionQueue.push_back([this]() { m_targetAddress += reg_x; });
            break;
        case IndexType::Y: // Unexpected
        default:
            throw std::runtime_error("Unexpected address index type.");
    }

    m_microInstructionQueue.push_back([this]() { m_operand = Read(m_targetAddress); });
    m_microInstructionQueue.push_back(operation);
    m_microInstructionQueue.push_back([this]() { Write(m_targetAddress, m_operand); });
}

void CPU::ZeroPageWriteOnly(std::function<void()> operation, IndexType indexType)
{
    m_microInstructionQueue.push_back([this]() { m_targetAddress = Read(reg_pc++); });

    switch (indexType)
    {
        case IndexType::None:
            // Do nothing
            break;
        case IndexType::X:
            m_microInstructionQueue.push_back([this]() { m_targetAddress += reg_x; });
            break;
        case IndexType::Y:
            m_microInstructionQueue.push_back([this]() { m_targetAddress += reg_y; });
            break;
        default:
            throw std::runtime_error("Unexpected address index type.");
    }

    m_microInstructionQueue.push_back(operation);
}

void CPU::IndexedIndirectReadOnly(std::function<void()> operation)
{
    // Temporarily uses the operand variable to store a pointer
    m_microInstructionQueue.push_back([this]() { m_operand = Read(reg_pc++); });
    m_microInstructionQueue.push_back([this]() { m_operand += reg_x; });
    m_microInstructionQueue.push_back([this]() { m_targetAddress = Read(m_operand++); });
    m_microInstructionQueue.push_back([this]() { m_targetAddress |= Read(m_operand) << 8; });
    m_microInstructionQueue.push_back([this, operation]()
        {
            m_operand = Read(m_targetAddress);
            operation();
        });
}

void CPU::IndexedIndirectReadModifyWrite(std::function<void()> operation)
{
    // Temporarily uses the operand variable to store a pointer
    m_microInstructionQueue.push_back([this]() { m_operand = Read(reg_pc++); });
    m_microInstructionQueue.push_back([this]() { m_operand += reg_x; });
    m_microInstructionQueue.push_back([this]() { m_targetAddress = Read(m_operand++); });
    m_microInstructionQueue.push_back([this]() { m_targetAddress |= Read(m_operand) << 8; });
    m_microInstructionQueue.push_back([this]() { m_operand = Read(m_targetAddress); });
    m_microInstructionQueue.push_back(operation);
    m_microInstructionQueue.push_back([this]() { Write(m_targetAddress, m_operand); });
}

void CPU::IndexedIndirectWriteOnly(std::function<void()> operation)
{
    // Temporarily uses the operand variable to store a pointer
    m_microInstructionQueue.push_back([this]() { m_operand = Read(reg_pc++); });
    m_microInstructionQueue.push_back([this]() { m_operand += reg_x; });
    m_microInstructionQueue.push_back([this]() { m_targetAddress = Read(m_operand++); });
    m_microInstructionQueue.push_back([this]() { m_targetAddress |= Read(m_operand) << 8; });
    m_microInstructionQueue.push_back(operation);
}

void CPU::IndirectIndexedReadOnly(std::function<void()> operation)
{
    // Temporarily uses the operand variable to store a pointer
    m_microInstructionQueue.push_back([this]() { m_operand = Read(reg_pc++); });
    m_microInstructionQueue.push_back([this]() { m_targetAddress = Read(m_operand++); });
    m_microInstructionQueue.push_back([this]()
        {
            uint8_t originalPage = Read(m_operand);

            m_targetAddress |= originalPage << 8;
            m_targetAddress += reg_y;

            uint8_t newPage = m_targetAddress >> 8;
            if (originalPage != newPage)
            {
                // Force an invalid read cycle
                m_microInstructionQueue.push_front([this] { Read(m_targetAddress - 0x100); });
            }
        });
    m_microInstructionQueue.push_back([this, operation]()
        {
            m_operand = Read(m_targetAddress);
            operation();
        });
}

void CPU::IndirectIndexedReadModifyWrite(std::function<void()> operation)
{
    // Temporarily uses the operand variable to store a pointer
    m_microInstructionQueue.push_back([this]() { m_operand = Read(reg_pc++); });
    m_microInstructionQueue.push_back([this]() { m_targetAddress = Read(m_operand++); });
    m_microInstructionQueue.push_back([this]()
        {
            uint8_t originalPage = Read(m_operand);
            m_targetAddress |= originalPage << 8;
            m_targetAddress += reg_y;

            // Force a read at a possibly invalid address
            m_microInstructionQueue.push_front([this, originalPage]
                {
                    if (originalPage == m_targetAddress >> 8)
                        Read(m_targetAddress);
                    else
                        Read(m_targetAddress - 0x100);
                });
        });
    m_microInstructionQueue.push_back([this]() { m_operand = Read(m_targetAddress); });
    m_microInstructionQueue.push_back(operation);
    m_microInstructionQueue.push_back([this]() { Write(m_targetAddress, m_operand); });
}

void CPU::IndirectIndexedWriteOnly(std::function<void()> operation)
{
    // Temporarily uses the operand variable to store a pointer
    m_microInstructionQueue.push_back([this]() { m_operand = Read(reg_pc++); });
    m_microInstructionQueue.push_back([this]() { m_targetAddress = Read(m_operand++); });
    m_microInstructionQueue.push_back([this]()
        {
            uint8_t originalPage = Read(m_operand);
            m_targetAddress |= originalPage << 8;
            m_targetAddress += reg_y;

            // Force a read at a possibly invalid address
            m_microInstructionQueue.push_front([this, originalPage]
                {
                    if (originalPage == m_targetAddress >> 8)
                        Read(m_targetAddress);
                    else
                        Read(m_targetAddress - 0x100);
                });
        });
    m_microInstructionQueue.push_back(operation);
}

void CPU::BranchInstruction(std::function<bool()> test)
{
    m_microInstructionQueue.push_back([this, test]()
        {
            m_operand = Read(reg_pc++);
            if (!test()) return;

            m_microInstructionQueue.push_front([this]()
                {
                    uint8_t originalPage = reg_pc >> 8;
                    reg_pc = reg_pc + static_cast<int8_t>(m_operand);

                    // Add a blank cycle when a page is crossed
                    if (originalPage != (reg_pc >> 8))
                        m_microInstructionQueue.push_front([]() {});
                });
        });
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
    m_microInstructionQueue.push_back([this] { Read(reg_pc); });
    m_microInstructionQueue.push_back([this]() { reg_s++; });
    m_microInstructionQueue.push_back([this]()
        {
            reg_p = StackPop();
            reg_s++;
        });
    m_microInstructionQueue.push_back([this]()
        {
            reg_pc = StackPop();
            reg_s++;
        });
    m_microInstructionQueue.push_back([this]() { reg_pc |= StackPop(); });
}

void CPU::RTS()
{
    m_microInstructionQueue.push_back([this] { Read(reg_pc); });
    m_microInstructionQueue.push_back([this]() { reg_s++; });
    m_microInstructionQueue.push_back([this]()
        {
            reg_pc = StackPop();
            reg_s++;
        });
    m_microInstructionQueue.push_back([this]() { reg_pc |= StackPop() << 8; });
    m_microInstructionQueue.push_back([this]() { reg_pc++; });
}

void CPU::PHA()
{
    m_microInstructionQueue.push_back([this] { Read(reg_pc); });
    m_microInstructionQueue.push_back([this]()
        {
            StackPush(reg_a);
            reg_s--;
        });
}

void CPU::PHP()
{
    m_microInstructionQueue.push_back([this] { Read(reg_pc); });
    m_microInstructionQueue.push_back([this]()
        {
            StackPush(reg_p | static_cast<uint8_t>(Flag::B));
            reg_s--;
        });
}

void CPU::PLA()
{
    m_microInstructionQueue.push_back([this] { Read(reg_pc); });
    m_microInstructionQueue.push_back([this]() { reg_s++; });
    m_microInstructionQueue.push_back([this]()
        {
            reg_a = StackPop();
            SetFlag(Flag::Z, reg_a == 0);
            SetFlag(Flag::N, reg_a & 0x80);
        });
}

void CPU::PLP()
{
    m_microInstructionQueue.push_back([this] { Read(reg_pc); });
    m_microInstructionQueue.push_back([this]() { reg_s++; });
    m_microInstructionQueue.push_back([this]()
        {
            reg_p = StackPop();
        });
}

void CPU::JSR()
{
    m_microInstructionQueue.push_back([this]() { m_targetAddress = Read(reg_pc++); });
    m_microInstructionQueue.push_back([this] { Read(0x100 + reg_s); });
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
    m_microInstructionQueue.push_back([this]()
        {
            m_targetAddress |= Read(reg_pc) << 8;
            reg_pc = m_targetAddress;
        });
}

void CPU::TAX()
{
    m_microInstructionQueue.push_back([this]()
        {
            reg_x = reg_a;

            SetFlag(Flag::Z, reg_x == 0);
            SetFlag(Flag::N, reg_x & 0x80);
        });
}

void CPU::TXA()
{
    m_microInstructionQueue.push_back([this]()
        {
            reg_a = reg_x;

            SetFlag(Flag::Z, reg_a == 0);
            SetFlag(Flag::N, reg_a & 0x80);
        });
}

void CPU::TAY()
{
    m_microInstructionQueue.push_back([this]()
        {
            reg_y = reg_a;

            SetFlag(Flag::Z, reg_y == 0);
            SetFlag(Flag::N, reg_y & 0x80);
        });
}

void CPU::TYA()
{
    m_microInstructionQueue.push_back([this]()
        {
            reg_a = reg_y;

            SetFlag(Flag::Z, reg_a == 0);
            SetFlag(Flag::N, reg_a & 0x80);
        });
}

void CPU::TXS()
{
    m_microInstructionQueue.push_back([this]()
        {
            reg_s = reg_x;
        });
}

void CPU::TSX()
{
    m_microInstructionQueue.push_back([this]()
        {
            reg_x = reg_s;

            SetFlag(Flag::Z, reg_x == 0);
            SetFlag(Flag::N, reg_x & 0x80);
        });
}

void CPU::INX()
{
    m_microInstructionQueue.push_back([this]()
        {
            reg_x++;

            SetFlag(Flag::Z, reg_x == 0);
            SetFlag(Flag::N, reg_x & 0x80);
        });
}

void CPU::DEX()
{
    m_microInstructionQueue.push_back([this]()
        {
            reg_x--;

            SetFlag(Flag::Z, reg_x == 0);
            SetFlag(Flag::N, reg_x & 0x80);
        });
}

void CPU::INY()
{
    m_microInstructionQueue.push_back([this]()
        {
            reg_y++;

            SetFlag(Flag::Z, reg_y == 0);
            SetFlag(Flag::N, reg_y & 0x80);
        });
}

void CPU::DEY()
{
    m_microInstructionQueue.push_back([this]()
        {
            reg_y--;

            SetFlag(Flag::Z, reg_y == 0);
            SetFlag(Flag::N, reg_y & 0x80);
        });
}

void CPU::SEC()
{
    m_microInstructionQueue.push_back([this]() { SetFlag(Flag::C, true); });
}

void CPU::CLC()
{
    m_microInstructionQueue.push_back([this]() { SetFlag(Flag::C, false); });
}

void CPU::SEI()
{
    m_microInstructionQueue.push_back([this]() { SetFlag(Flag::I, true); });
}

void CPU::CLI()
{
    m_microInstructionQueue.push_back([this]() { SetFlag(Flag::I, false); });
}

void CPU::SED()
{
    m_microInstructionQueue.push_back([this]() { SetFlag(Flag::D, true); });
}

void CPU::CLD()
{
    m_microInstructionQueue.push_back([this]() { SetFlag(Flag::D, false); });
}

void CPU::CLV()
{
    m_microInstructionQueue.push_back([this]() { SetFlag(Flag::V, false); });
}

void CPU::JMP_Absolute()
{
    m_microInstructionQueue.push_back([this]() { m_operand = Read(reg_pc++); });
    m_microInstructionQueue.push_back([this]()
        {
            uint8_t pch = Read(reg_pc);
            reg_pc = m_operand;
            reg_pc |= pch << 8;
        });
}

void CPU::JMP_Indirect()
{
    m_microInstructionQueue.push_back([this]() { m_targetAddress = Read(reg_pc++); });
    m_microInstructionQueue.push_back([this]() { m_targetAddress |= Read(reg_pc++) << 8; });
    m_microInstructionQueue.push_back([this]() { m_operand = Read(m_targetAddress); });
    m_microInstructionQueue.push_back([this]()
        {
            uint16_t pchAddress;
            if (static_cast<uint8_t>(m_targetAddress) == 0xFF)
                pchAddress = m_targetAddress & 0xFF00;
            else
                pchAddress = m_targetAddress + 1;

            reg_pc = m_operand;
            reg_pc |= Read(pchAddress) << 8;
        });
}
