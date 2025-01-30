#include "cpu.h"

void CPU::QueueNextInstuction()
{
    uint8_t opcode = Read(reg_pc);
    reg_pc++;

    switch (opcode)
    {
    // Opcodes 0x00 to 0x0F
    case 0x00: // BRK
        InternalInterrupt(InterruptType::BRK);
        break;
    case 0x01: // ORA (Indirect,X)
        m_operation = &CPU::ORA;
        IndexedIndirectReadOnly();
        break;
    case 0x05: // ORA Zero Page
        m_operation = &CPU::ORA;
        m_indexType = IndexType::None;
        ZeroPageReadOnly();
        break;
    case 0x06: // ASL Zero Page
        m_operation = &CPU::ASL;
        m_indexType = IndexType::None;
        ZeroPageReadModifyWrite();
        break;
    case 0x08: // PHP
        PHP();
        break;
    case 0x09: // ORA Immediate
        m_operation = &CPU::ORA;
        ImmediateReadOnly();
        break;
    case 0x0A: // ASL Accumulator
        m_operation = &CPU::ASL;
        AccumulatorReadModifyWrite();
        break;
    case 0x0D: // ORA Absolute
        m_operation = &CPU::ORA;
        m_indexType = IndexType::None;
        AbsoluteReadOnly();
        break;
    case 0x0E: // ASL Absolute
        m_operation = &CPU::ASL;
        m_indexType = IndexType::None;
        AbsoluteReadModifyWrite();
        break;

    // Opcodes 0x10 to 0x1F
    case 0x10: // BPL
        m_branchTest = &CPU::PlusTest;
        BranchInstruction();
        break;
    case 0x11: // ORA (Indirect),Y
        m_operation = &CPU::ORA;
        IndirectIndexedReadOnly();
        break;
    case 0x15: // ORA Zero Page,X
        m_operation = &CPU::ORA;
        m_indexType = IndexType::X;
        ZeroPageReadOnly();
        break;
    case 0x16: // ASL Zero Page,X
        m_operation = &CPU::ASL;
        m_indexType = IndexType::X;
        ZeroPageReadModifyWrite();
        break;
    case 0x18: // CLC
        m_microInstructionQueue.push_back(&CPU::CLC);
        break;
    case 0x19: // ORA Absolute,Y
        m_operation = &CPU::ORA;
        m_indexType = IndexType::Y;
        AbsoluteReadOnly();
        break;
    case 0x1D: // ORA Absolute,X
        m_operation = &CPU::ORA;
        m_indexType = IndexType::X;
        AbsoluteReadOnly();
        break;
    case 0x1E: // ASL Absolute,X
        m_operation = &CPU::ASL;
        m_indexType = IndexType::X;
        AbsoluteReadModifyWrite();
        break;

    // Opcodes 0x20 to 0x2F
    case 0x20: // JSR
        JSR();
        break;
    case 0x21: // AND (Indirect,X)
        m_operation = &CPU::AND;
        IndexedIndirectReadOnly();
        break;
    case 0x24: // BIT Zero Page
        m_operation = &CPU::BIT;
        m_indexType = IndexType::None;
        ZeroPageReadOnly();
        break;
    case 0x25: // AND Zero Page
        m_operation = &CPU::AND;
        m_indexType = IndexType::None;
        ZeroPageReadOnly();
        break;
    case 0x26: // ROL Zero Page
        m_operation = &CPU::ROL;
        m_indexType = IndexType::None;
        ZeroPageReadModifyWrite();
        break;
    case 0x28: // PLP
        PLP();
        break;
    case 0x29: // AND Immediate
        m_operation = &CPU::AND;
        ImmediateReadOnly();
        break;
    case 0x2A: // ROL Accumulator
        m_operation = &CPU::ROL;
        AccumulatorReadModifyWrite();
        break;
    case 0x2C: // BIT Absolute
        m_operation = &CPU::BIT;
        m_indexType = IndexType::None;
        AbsoluteReadOnly();
        break;
    case 0x2D: // AND Absolute
        m_operation = &CPU::AND;
        m_indexType = IndexType::None;
        AbsoluteReadOnly();
        break;
    case 0x2E: // ROL Absolute
        m_operation = &CPU::ROL;
        m_indexType = IndexType::None;
        AbsoluteReadModifyWrite();
        break;

    // Opcodes 0x30 to 0x3F
    case 0x30: // BMI
        m_branchTest = &CPU::MinusTest;
        BranchInstruction();
        break;
    case 0x31: // AND (Indirect),Y
        m_operation = &CPU::AND;
        IndirectIndexedReadOnly();
        break;
    case 0x35: // AND Zero Page,X
        m_operation = &CPU::AND;
        m_indexType = IndexType::X;
        ZeroPageReadOnly();
        break;
    case 0x36: // ROL Zero Page,X
        m_operation = &CPU::ROL;
        m_indexType = IndexType::X;
        ZeroPageReadModifyWrite();
        break;
    case 0x38: // SEC
        m_microInstructionQueue.push_back(&CPU::SEC);
        break;
    case 0x39: // AND Absolute,Y
        m_operation = &CPU::AND;
        m_indexType = IndexType::Y;
        AbsoluteReadOnly();
        break;
    case 0x3D: // AND Absolute,X
        m_operation = &CPU::AND;
        m_indexType = IndexType::X;
        AbsoluteReadOnly();
        break;
    case 0x3E: // ROL Absolute,X
        m_operation = &CPU::ROL;
        m_indexType = IndexType::X;
        AbsoluteReadModifyWrite();
        break;

    // Opcodes 0x40 to 0x4F
    case 0x40: // RTI
        RTI();
        break;
    case 0x41: // EOR (Indirect,X)
        m_operation = &CPU::EOR;
        IndexedIndirectReadOnly();
        break;
    case 0x45: // EOR Zero Page
        m_operation = &CPU::EOR;
        m_indexType = IndexType::None;
        ZeroPageReadOnly();
        break;
    case 0x46: // LSR Zero Page
        m_operation = &CPU::LSR;
        m_indexType = IndexType::None;
        ZeroPageReadModifyWrite();
        break;
    case 0x48: // PHA
        PHA();
        break;
    case 0x49: // EOR Immediate
        m_operation = &CPU::EOR;
        ImmediateReadOnly();
        break;
    case 0x4A: // LSR Accumulator
        m_operation = &CPU::LSR;
        AccumulatorReadModifyWrite();
        break;
    case 0x4C: // JMP Absolute
        JMP_Absolute();
        break;
    case 0x4D: // EOR Absolute
        m_operation = &CPU::EOR;
        m_indexType = IndexType::None;
        AbsoluteReadOnly();
        break;
    case 0x4E: // LSR Absolute
        m_operation = &CPU::LSR;
        m_indexType = IndexType::None;
        AbsoluteReadModifyWrite();
        break;

    // Opcodes 0x50 to 0x5F
    case 0x50: // BVC
        m_branchTest = &CPU::OverflowClearTest;
        BranchInstruction();
        break;
    case 0x51: // EOR (Indirect),Y
        m_operation = &CPU::EOR;
        IndirectIndexedReadOnly();
        break;
    case 0x55: // EOR Zero Page,X
        m_operation = &CPU::EOR;
        m_indexType = IndexType::X;
        ZeroPageReadOnly();
        break;
    case 0x56: // LSR Zero Page,X
        m_operation = &CPU::LSR;
        m_indexType = IndexType::X;
        ZeroPageReadModifyWrite();
        break;
    case 0x58: // CLI
        m_microInstructionQueue.push_back(&CPU::CLI);
        break;
    case 0x59: // EOR Absolute,Y
        m_operation = &CPU::EOR;
        m_indexType = IndexType::Y;
        AbsoluteReadOnly();
        break;
    case 0x5D: // EOR Absolute,X
        m_operation = &CPU::EOR;
        m_indexType = IndexType::X;
        AbsoluteReadOnly();
        break;
    case 0x5E: // LSR Absolute,X
        m_operation = &CPU::LSR;
        m_indexType = IndexType::X;
        AbsoluteReadModifyWrite();
        break;

    // Opcodes 0x60 to 0x6F
    case 0x60: // RTS
        RTS();
        break;
    case 0x61: // ADC (Indirect,X)
        m_operation = &CPU::ADC;
        IndexedIndirectReadOnly();
        break;
    case 0x65: // ADC Zero Page
        m_operation = &CPU::ADC;
        m_indexType = IndexType::None;
        ZeroPageReadOnly();
        break;
    case 0x66: // ROR Zero Page
        m_operation = &CPU::ROR;
        m_indexType = IndexType::None;
        ZeroPageReadModifyWrite();
        break;
    case 0x68: // PLA
        PLA();
        break;
    case 0x69: // ADC Immediate
        m_operation = &CPU::ADC;
        ImmediateReadOnly();
        break;
    case 0x6A: // ROR Accumulator
        m_operation = &CPU::ROR;
        AccumulatorReadModifyWrite();
        break;
    case 0x6C: // JMP (Indirect)
        JMP_Indirect();
        break;
    case 0x6D: // ADC Absolute
        m_operation = &CPU::ADC;
        m_indexType = IndexType::None;
        AbsoluteReadOnly();
        break;
    case 0x6E: // ROR Absolute
        m_operation = &CPU::ROR;
        m_indexType = IndexType::None;
        AbsoluteReadModifyWrite();
        break;

    // Opcodes 0x70 to 0x7F
    case 0x70: // BVS
        m_branchTest = &CPU::OverflowSetTest;
        BranchInstruction();
        break;
    case 0x71: // ADC (Indirect),Y
        m_operation = &CPU::ADC;
        IndirectIndexedReadOnly();
        break;
    case 0x75: // ADC Zero Page,X
        m_operation = &CPU::ADC;
        m_indexType = IndexType::X;
        ZeroPageReadOnly();
        break;
    case 0x76: // ROR Zero Page,X
        m_operation = &CPU::ROR;
        m_indexType = IndexType::X;
        ZeroPageReadModifyWrite();
        break;
    case 0x78: // SEI
        m_microInstructionQueue.push_back(&CPU::SEI);
        break;
    case 0x79: // ADC Absolute,Y
        m_operation = &CPU::ADC;
        m_indexType = IndexType::Y;
        AbsoluteReadOnly();
        break;
    case 0x7D: // ADC Absolute,X
        m_operation = &CPU::ADC;
        m_indexType = IndexType::X;
        AbsoluteReadOnly();
        break;
    case 0x7E: // ROR Absolute,X
        m_operation = &CPU::ROR;
        m_indexType = IndexType::X;
        AbsoluteReadModifyWrite();
        break;

    // Opcodes 0x80 to 0x8F
    case 0x81: // STA (Indirect,X)
        m_operation = &CPU::STA;
        IndexedIndirectWriteOnly();
        break;
    case 0x84: // STY Zero Page
        m_operation = &CPU::STY;
        m_indexType = IndexType::None;
        ZeroPageWriteOnly();
        break;
    case 0x85: // STA Zero Page
        m_operation = &CPU::STA;
        m_indexType = IndexType::None;
        ZeroPageWriteOnly();
        break;
    case 0x86: // STX Zero Page
        m_operation = &CPU::STX;
        m_indexType = IndexType::None;
        ZeroPageWriteOnly();
        break;
    case 0x88: // DEY
        m_microInstructionQueue.push_back(&CPU::DEY);
        break;
    case 0x8A: // TXA
        m_microInstructionQueue.push_back(&CPU::TXA);
        break;
    case 0x8C: // STY Absolute
        m_operation = &CPU::STY;
        m_indexType = IndexType::None;
        AbsoluteWriteOnly();
        break;
    case 0x8D: // STA Absolute
        m_operation = &CPU::STA;
        m_indexType = IndexType::None;
        AbsoluteWriteOnly();
        break;
    case 0x8E: // STX Absolute
        m_operation = &CPU::STX;
        m_indexType = IndexType::None;
        AbsoluteWriteOnly();
        break;

    // Opcodes 0x90 to 0x9F
    case 0x90: // BCC
        m_branchTest = &CPU::CarryClearTest;
        BranchInstruction();
        break;
    case 0x91: // STA (Indirect),Y
        m_operation = &CPU::STA;
        IndirectIndexedWriteOnly();
        break;
    case 0x94: // STY Zero Page,X
        m_operation = &CPU::STY;
        m_indexType = IndexType::X;
        ZeroPageWriteOnly();
        break;
    case 0x95: // STA Zero Page,X
        m_operation = &CPU::STA;
        m_indexType = IndexType::X;
        ZeroPageWriteOnly();
        break;
    case 0x96: // STX Zero Page,Y
        m_operation = &CPU::STX;
        m_indexType = IndexType::Y;
        ZeroPageWriteOnly();
        break;
    case 0x98: // TYA
        m_microInstructionQueue.push_back(&CPU::TYA);
        break;
    case 0x99: // STA Absolute,Y
        m_operation = &CPU::STA;
        m_indexType = IndexType::Y;
        AbsoluteWriteOnly();
        break;
    case 0x9A: // TXS
        m_microInstructionQueue.push_back(&CPU::TXS);
        break;
    case 0x9D: // STA Absolute,X
        m_operation = &CPU::STA;
        m_indexType = IndexType::X;
        AbsoluteWriteOnly();
        break;

    // Opcodes 0xA0 to 0xAF
    case 0xA0: // LDY Immediate
        m_operation = &CPU::LDY;
        ImmediateReadOnly();
        break;
    case 0xA1: // LDA (Indirect,X)
        m_operation = &CPU::LDA;
        IndexedIndirectReadOnly();
        break;
    case 0xA2: // LDX Immediate
        m_operation = &CPU::LDX;
        ImmediateReadOnly();
        break;
    case 0xA4: // LDY Zero Page
        m_operation = &CPU::LDY;
        m_indexType = IndexType::None;
        ZeroPageReadOnly();
        break;
    case 0xA5: // LDA Zero Page
        m_operation = &CPU::LDA;
        m_indexType = IndexType::None;
        ZeroPageReadOnly();
        break;
    case 0xA6: // LDX Zero Page
        m_operation = &CPU::LDX;
        m_indexType = IndexType::None;
        ZeroPageReadOnly();
        break;
    case 0xA8: // TAY
        m_microInstructionQueue.push_back(&CPU::TAY);
        break;
    case 0xA9: // LDA Immediate
        m_operation = &CPU::LDA;
        ImmediateReadOnly();
        break;
    case 0xAA: // TAX
        m_microInstructionQueue.push_back(&CPU::TAX);
        break;
    case 0xAC: // LDY Absolute
        m_operation = &CPU::LDY;
        m_indexType = IndexType::None;
        AbsoluteReadOnly();
        break;
    case 0xAD: // LDA Absolute
        m_operation = &CPU::LDA;
        m_indexType = IndexType::None;
        AbsoluteReadOnly();
        break;
    case 0xAE: // LDX Absolute
        m_operation = &CPU::LDX;
        m_indexType = IndexType::None;
        AbsoluteReadOnly();
        break;

    // Opcodes 0xB0 to 0xBF
    case 0xB0: // BCS
        m_branchTest = &CPU::CarrySetTest;
        BranchInstruction();
        break;
    case 0xB1: // LDA (Indirect),Y
        m_operation = &CPU::LDA;
        IndirectIndexedReadOnly();
        break;
    case 0xB4: // LDY Zero Page,X
        m_operation = &CPU::LDY;
        m_indexType = IndexType::X;
        ZeroPageReadOnly();
        break;
    case 0xB5: // LDA Zero Page,X
        m_operation = &CPU::LDA;
        m_indexType = IndexType::X;
        ZeroPageReadOnly();
        break;
    case 0xB6: // LDX Zero Page,Y
        m_operation = &CPU::LDX;
        m_indexType = IndexType::Y;
        ZeroPageReadOnly();
        break;
    case 0xB8: // CLV
        m_microInstructionQueue.push_back(&CPU::CLV);
        break;
    case 0xB9: // LDA Absolute,Y
        m_operation = &CPU::LDA;
        m_indexType = IndexType::Y;
        AbsoluteReadOnly();
        break;
    case 0xBA: // TSX
        m_microInstructionQueue.push_back(&CPU::TSX);
        break;
    case 0xBC: // LDY Absolute,X
        m_operation = &CPU::LDY;
        m_indexType = IndexType::X;
        AbsoluteReadOnly();
        break;
    case 0xBD: // LDA Absolute,X
        m_operation = &CPU::LDA;
        m_indexType = IndexType::X;
        AbsoluteReadOnly();
        break;
    case 0xBE: // LDX Absolute,Y
        m_operation = &CPU::LDX;
        m_indexType = IndexType::Y;
        AbsoluteReadOnly();
        break;

    // Opcodes 0xC0 to 0xCF
    case 0xC0: // CPY Immediate
        m_operation = &CPU::CPY;
        ImmediateReadOnly();
        break;
    case 0xC1: // CMP (Indirect,X)
        m_operation = &CPU::CMP;
        IndexedIndirectReadOnly();
        break;
    case 0xC4: // CPY Zero Page
        m_operation = &CPU::CPY;
        m_indexType = IndexType::None;
        ZeroPageReadOnly();
        break;
    case 0xC5: // CMP Zero Page
        m_operation = &CPU::CMP;
        m_indexType = IndexType::None;
        ZeroPageReadOnly();
        break;
    case 0xC6: // DEC Zero Page
        m_operation = &CPU::DEC;
        m_indexType = IndexType::None;
        ZeroPageReadModifyWrite();
        break;
    case 0xC8: // INY
        m_microInstructionQueue.push_back(&CPU::INY);
        break;
    case 0xC9: // CMP Immediate
        m_operation = &CPU::CMP;
        ImmediateReadOnly();
        break;
    case 0xCA: // DEX
        m_microInstructionQueue.push_back(&CPU::DEX);
        break;
    case 0xCC: // CPY Absolute
        m_operation = &CPU::CPY;
        m_indexType = IndexType::None;
        AbsoluteReadOnly();
        break;
    case 0xCD: // CMP Absolute
        m_operation = &CPU::CMP;
        m_indexType = IndexType::None;
        AbsoluteReadOnly();
        break;
    case 0xCE: // DEC Absolute
        m_operation = &CPU::DEC;
        m_indexType = IndexType::None;
        AbsoluteReadModifyWrite();
        break;

    // Opcodes 0xD0 to 0xDF
    case 0xD0: // BNE
        m_branchTest = &CPU::NotEqualTest;
        BranchInstruction();
        break;
    case 0xD1: // CMP (Indirect),Y
        m_operation = &CPU::CMP;
        IndirectIndexedReadOnly();
        break;
    case 0xD5: // CMP Zero Page,X
        m_operation = &CPU::CMP;
        m_indexType = IndexType::X;
        ZeroPageReadOnly();
        break;
    case 0xD6: // DEC Zero Page,X
        m_operation = &CPU::DEC;
        m_indexType = IndexType::X;
        ZeroPageReadModifyWrite();
        break;
    case 0xD8: // CLD
        m_microInstructionQueue.push_back(&CPU::CLD);
        break;
    case 0xD9: // CMP Absolute,Y
        m_operation = &CPU::CMP;
        m_indexType = IndexType::Y;
        AbsoluteReadOnly();
        break;
    case 0xDD: // CMP Absolute,X
        m_operation = &CPU::CMP;
        m_indexType = IndexType::X;
        AbsoluteReadOnly();
        break;
    case 0xDE: // DEC Absolute,X
        m_operation = &CPU::DEC;
        m_indexType = IndexType::X;
        AbsoluteReadModifyWrite();
        break;

    // Opcodes 0xE0 to 0xEF
    case 0xE0: // CPX Immediate
        m_operation = &CPU::CPX;
        ImmediateReadOnly();
        break;
    case 0xE1: // SBC (Indirect,X)
        m_operation = &CPU::SBC;
        IndexedIndirectReadOnly();
        break;
    case 0xE4: // CPX Zero Page
        m_operation = &CPU::CPX;
        m_indexType = IndexType::None;
        ZeroPageReadOnly();
        break;
    case 0xE5: // SBC Zero Page
        m_operation = &CPU::SBC;
        m_indexType = IndexType::None;
        ZeroPageReadOnly();
        break;
    case 0xE6: // INC Zero Page
        m_operation = &CPU::INC;
        m_indexType = IndexType::None;
        ZeroPageReadModifyWrite();
        break;
    case 0xE8: // INX
        m_microInstructionQueue.push_back(&CPU::INX);
        break;
    case 0xE9: // SBC Immediate
        m_operation = &CPU::SBC;
        ImmediateReadOnly();
        break;
    case 0xEA: // NOP
        m_microInstructionQueue.push_back(&CPU::BlankMicroInstruction);
        break;
    case 0xEC: // CPX Absolute
        m_operation = &CPU::CPX;
        m_indexType = IndexType::None;
        AbsoluteReadOnly();
        break;
    case 0xED: // SBC Absolute
        m_operation = &CPU::SBC;
        m_indexType = IndexType::None;
        AbsoluteReadOnly();
        break;
    case 0xEE: // INC Absolute
        m_operation = &CPU::INC;
        m_indexType = IndexType::None;
        AbsoluteReadModifyWrite();
        break;

    // Opcodes 0xF0 to 0xFF
    case 0xF0: // BEQ
        m_branchTest = &CPU::EqualTest;
        BranchInstruction();
        break;
    case 0xF1: // SBC (Indirect),Y
        m_operation = &CPU::SBC;
        IndirectIndexedReadOnly();
        break;
    case 0xF5: // SBC Zero Page,X
        m_operation = &CPU::SBC;
        m_indexType = IndexType::X;
        ZeroPageReadOnly();
        break;
    case 0xF6: // INC Zero Page,X
        m_operation = &CPU::INC;
        m_indexType = IndexType::X;
        ZeroPageReadModifyWrite();
        break;
    case 0xF8: // SED
        m_microInstructionQueue.push_back(&CPU::SED);
        break;
    case 0xF9: // SBC Absolute,Y
        m_operation = &CPU::SBC;
        m_indexType = IndexType::Y;
        AbsoluteReadOnly();
        break;
    case 0xFD: // SBC Absolute,X
        m_operation = &CPU::SBC;
        m_indexType = IndexType::X;
        AbsoluteReadOnly();
        break;
    case 0xFE: // INC Absolute,X
        m_operation = &CPU::INC;
        m_indexType = IndexType::X;
        AbsoluteReadModifyWrite();
        break;

    // Illegal opcodes
    default: 
        Logger::GetInstance().Warn("unexpected opcode " + Logger::DecmialToHex(opcode));
        break;
    }
}