#include "cpu.h"

void CPU::QueueNextInstuction()
{
    uint8_t opcode = Read(reg_pc);
    reg_pc++;

    switch (opcode)
    {
        // Opcodes 0x00 to 0x0F
        case 0x00: // BRK
            BRK();
            break;
        case 0x01: // ORA (Indirect,X)
            IndexedIndirectReadOnly([this]() { ORA(); });
            break;
        case 0x05: // ORA Zero Page
            ZeroPageReadOnly([this]() { ORA(); }, IndexType::None);
            break;
        case 0x06: // ASL Zero Page
            ZeroPageReadModifyWrite([this]() { ASL(); }, IndexType::None);
            break;
        case 0x08: // PHP
            PHP();
            break;
        case 0x09: // ORA Immediate
            ImmediateReadOnly([this]() { ORA(); });
            break;
        case 0x0A: // ASL Accumulator
            AccumulatorReadModifyWrite([this]() { ASL(); });
            break;
        case 0x0D: // ORA Absolute
            AbsoluteReadOnly([this]() { ORA(); }, IndexType::None);
            break;
        case 0x0E: // ASL Absolute
            AbsoluteReadModifyWrite([this]() { ASL(); }, IndexType::None);
            break;

        // Opcodes 0x10 to 0x1F
        case 0x10: // BPL
            BranchInstruction([this]() { return GetFlag(Flag::N) == 0; });
            break;
        case 0x11: // ORA (Indirect),Y
            IndirectIndexedReadOnly([this]() { ORA(); });
            break;
        case 0x15: // ORA Zero Page,X
            ZeroPageReadOnly([this]() { ORA(); }, IndexType::X);
            break;
        case 0x16: // ASL Zero Page,X
            ZeroPageReadModifyWrite([this]() { ASL(); }, IndexType::X);
            break;
        case 0x18: // CLC
            CLC();
            break;
        case 0x19: // ORA Absolute,Y
            AbsoluteReadOnly([this]() { ORA(); }, IndexType::Y);
            break;
        case 0x1D: // ORA Absolute,X
            AbsoluteReadOnly([this]() { ORA(); }, IndexType::X);
            break;
        case 0x1E: // ASL Absolute,X
            AbsoluteReadModifyWrite([this]() { ASL(); }, IndexType::X);
            break;

        // Opcodes 0x20 to 0x2F
        case 0x20: // JSR
            JSR();
            break;
        case 0x21: // AND (Indirect,X)
            IndexedIndirectReadOnly([this]() { AND(); });
            break;
        case 0x24: // BIT Zero Page
            ZeroPageReadOnly([this]() { BIT(); }, IndexType::None);
            break;
        case 0x25: // AND Zero Page
            ZeroPageReadOnly([this]() { AND(); }, IndexType::None);
            break;
        case 0x26: // ROL Zero Page
            ZeroPageReadModifyWrite([this]() { ROL(); }, IndexType::None);
            break;
        case 0x28: // PLP
            PLP();
            break;
        case 0x29: // AND Immediate
            ImmediateReadOnly([this]() { AND(); });
            break;
        case 0x2A: // ROL Accumulator
            AccumulatorReadModifyWrite([this]() { ROL(); });
            break;
        case 0x2C: // BIT Absolute
            AbsoluteReadOnly([this]() { BIT(); }, IndexType::None);
            break;
        case 0x2D: // AND Absolute
            AbsoluteReadOnly([this]() { AND(); }, IndexType::None);
            break;
        case 0x2E: // ROL Absolute
            AbsoluteReadModifyWrite([this]() { ROL(); }, IndexType::None);
            break;

        // Opcodes 0x30 to 0x3F
        case 0x30: // BMI
            BranchInstruction([this]() { return GetFlag(Flag::N) == 1; });
            break;
        case 0x31: // AND (Indirect),Y
            IndirectIndexedReadOnly([this]() { AND(); });
            break;
        case 0x35: // AND Zero Page,X
            ZeroPageReadOnly([this]() { AND(); }, IndexType::X);
            break;
        case 0x36: // ROL Zero Page,X
            ZeroPageReadModifyWrite([this]() { ROL(); }, IndexType::X);
            break;
        case 0x38: // SEC
            SEC();
            break;
        case 0x39: // AND Absolute,Y
            AbsoluteReadOnly([this]() { AND(); }, IndexType::Y);
            break;
        case 0x3D: // AND Absolute,X
            AbsoluteReadOnly([this]() { AND(); }, IndexType::X);
            break;
        case 0x3E: // ROL Absolute,X
            AbsoluteReadModifyWrite([this]() { ROL(); }, IndexType::X);
            break;

        // Opcodes 0x40 to 0x4F
        case 0x40: // RTI
            RTI();
            break;
        case 0x41: // EOR (Indirect,X)
            IndexedIndirectReadOnly([this]() { EOR(); });
            break;
        case 0x45: // EOR Zero Page
            ZeroPageReadOnly([this]() { EOR(); }, IndexType::None);
            break;
        case 0x46: // LSR Zero Page
            ZeroPageReadModifyWrite([this]() { LSR(); }, IndexType::None);
            break;
        case 0x48: // PHA
            PHA();
            break;
        case 0x49: // EOR Immediate
            ImmediateReadOnly([this]() { EOR(); });
            break;
        case 0x4A: // LSR Accumulator
            AccumulatorReadModifyWrite([this]() { LSR(); });
            break;
        case 0x4C: // JMP Absolute
            JMP_Absolute();
            break;
        case 0x4D: // EOR Absolute
            AbsoluteReadOnly([this]() { EOR(); }, IndexType::None);
            break;
        case 0x4E: // LSR Absolute
            AbsoluteReadModifyWrite([this]() { LSR(); }, IndexType::None);
            break;

        // Opcodes 0x50 to 0x5F
        case 0x50: // BVC
            BranchInstruction([this]() { return GetFlag(Flag::V) == 0; });
            break;
        case 0x51: // EOR (Indirect),Y
            IndirectIndexedReadOnly([this]() { EOR(); });
            break;
        case 0x55: // EOR Zero Page,X
            ZeroPageReadOnly([this]() { EOR(); }, IndexType::X);
            break;
        case 0x56: // LSR Zero Page,X
            ZeroPageReadModifyWrite([this]() { LSR(); }, IndexType::X);
            break;
        case 0x58: // CLI
            CLI();
            break;
        case 0x59: // EOR Absolute,Y
            AbsoluteReadOnly([this]() { EOR(); }, IndexType::Y);
            break;
        case 0x5D: // EOR Absolute,X
            AbsoluteReadOnly([this]() { EOR(); }, IndexType::X);
            break;
        case 0x5E: // LSR Absolute,X
            AbsoluteReadModifyWrite([this]() { LSR(); }, IndexType::X);
            break;

        // Opcodes 0x60 to 0x6F
        case 0x60: // RTS
            RTS();
            break;
        case 0x61: // ADC (Indirect,X)
            IndexedIndirectReadOnly([this]() { ADC(); });
            break;
        case 0x65: // ADC Zero Page
            ZeroPageReadOnly([this]() { ADC(); }, IndexType::None);
            break;
        case 0x66: // ROR Zero Page
            ZeroPageReadModifyWrite([this]() { ROR(); }, IndexType::None);
            break;
        case 0x68: // PLA
            PLA();
            break;
        case 0x69: // ADC Immediate
            ImmediateReadOnly([this]() { ADC(); });
            break;
        case 0x6A: // ROR Accumulator
            AccumulatorReadModifyWrite([this]() { ROR(); });
            break;
        case 0x6C: // JMP (Indirect)
            JMP_Indirect();
            break;
        case 0x6D: // ADC Absolute
            AbsoluteReadOnly([this]() { ADC(); }, IndexType::None);
            break;
        case 0x6E: // ROR Absolute
            AbsoluteReadModifyWrite([this]() { ROR(); }, IndexType::None);
            break;

        // Opcodes 0x70 to 0x7F
        case 0x70: // BVS
            BranchInstruction([this]() { return GetFlag(Flag::V) == 1; });
            break;
        case 0x71: // ADC (Indirect),Y
            IndirectIndexedReadOnly([this]() { ADC(); });
            break;
        case 0x75: // ADC Zero Page,X
            ZeroPageReadOnly([this]() { ADC(); }, IndexType::X);
            break;
        case 0x76: // ROR Zero Page,X
            ZeroPageReadModifyWrite([this]() { ROR(); }, IndexType::X);
            break;
        case 0x78: // SEI
            SEI();
            break;
        case 0x79: // ADC Absolute,Y
            AbsoluteReadOnly([this]() { ADC(); }, IndexType::Y);
            break;
        case 0x7D: // ADC Absolute,X
            AbsoluteReadOnly([this]() { ADC(); }, IndexType::X);
            break;
        case 0x7E: // ROR Absolute,X
            AbsoluteReadModifyWrite([this]() { ROR(); }, IndexType::X);
            break;

        // Opcodes 0x80 to 0x8F
        case 0x81: // STA (Indirect,X)
            IndexedIndirectWriteOnly([this]() { STA(); });
            break;
        case 0x84: // STY Zero Page
            ZeroPageWriteOnly([this]() { STY(); }, IndexType::None);
            break;
        case 0x85: // STA Zero Page
            ZeroPageWriteOnly([this]() { STA(); }, IndexType::None);
            break;
        case 0x86: // STX Zero Page
            ZeroPageWriteOnly([this]() { STX(); }, IndexType::None);
            break;
        case 0x88: // DEY
            DEY();
            break;
        case 0x8A: // TXA
            TXA();
            break;
        case 0x8C: // STY Absolute
            AbsoluteWriteOnly([this]() { STY(); }, IndexType::None);
            break;
        case 0x8D: // STA Absolute
            AbsoluteWriteOnly([this]() { STA(); }, IndexType::None);
            break;
        case 0x8E: // STX Absolute
            AbsoluteWriteOnly([this]() { STX(); }, IndexType::None);
            break;

        // Opcodes 0x90 to 0x9F
        case 0x90: // BCC
            BranchInstruction([this]() { return GetFlag(Flag::C) == 0; });
            break;
        case 0x91: // STA (Indirect),Y
            IndirectIndexedWriteOnly([this]() { STA(); });
            break;
        case 0x94: // STY Zero Page,X
            ZeroPageWriteOnly([this]() { STY(); }, IndexType::X);
            break;
        case 0x95: // STA Zero Page,X
            ZeroPageWriteOnly([this]() { STA(); }, IndexType::X);
            break;
        case 0x96: // STX Zero Page,Y
            ZeroPageWriteOnly([this]() { STX(); }, IndexType::Y);
            break;
        case 0x98: // TYA
            TYA();
            break;
        case 0x99: // STA Absolute,Y
            AbsoluteWriteOnly([this]() { STA(); }, IndexType::Y);
            break;
        case 0x9A: // TXS
            TXS();
            break;
        case 0x9D: // STA Absolute,X
            AbsoluteWriteOnly([this]() { STA(); }, IndexType::X);
            break;

        // Opcodes 0xA0 to 0xAF
        case 0xA0: // LDY Immediate
            ImmediateReadOnly([this]() { LDY(); });
            break;
        case 0xA1: // LDA (Indirect,X)
            IndexedIndirectReadOnly([this]() { LDA(); });
            break;
        case 0xA2: // LDX Immediate
            ImmediateReadOnly([this]() { LDX(); });
            break;
        case 0xA4: // LDY Zero Page
            ZeroPageReadOnly([this]() { LDY(); }, IndexType::None);
            break;
        case 0xA5: // LDA Zero Page
            ZeroPageReadOnly([this]() { LDA(); }, IndexType::None);
            break;
        case 0xA6: // LDX Zero Page
            ZeroPageReadOnly([this]() { LDX(); }, IndexType::None);
            break;
        case 0xA8: // TAY
            TAY();
            break;
        case 0xA9: // LDA Immediate
            ImmediateReadOnly([this]() { LDA(); });
            break;
        case 0xAA: // TAX
            TAX();
            break;
        case 0xAC: // LDY Absolute
            AbsoluteReadOnly([this]() { LDY(); }, IndexType::None);
            break;
        case 0xAD: // LDA Absolute
            AbsoluteReadOnly([this]() { LDA(); }, IndexType::None);
            break;
        case 0xAE: // LDX Absolute
            AbsoluteReadOnly([this]() { LDX(); }, IndexType::None);
            break;

        // Opcodes 0xB0 to 0xBF
        case 0xB0: // BCS
            BranchInstruction([this]() { return GetFlag(Flag::C) == 1; });
            break;
        case 0xB1: // LDA (Indirect),Y
            IndirectIndexedReadOnly([this]() { LDA(); });
            break;
        case 0xB4: // LDY Zero Page,X
            ZeroPageReadOnly([this]() { LDY(); }, IndexType::X);
            break;
        case 0xB5: // LDA Zero Page,X
            ZeroPageReadOnly([this]() { LDA(); }, IndexType::X);
            break;
        case 0xB6: // LDX Zero Page,Y
            ZeroPageReadOnly([this]() { LDX(); }, IndexType::Y);
            break;
        case 0xB8: // CLV
            CLV();
            break;
        case 0xB9: // LDA Absolute,Y
            AbsoluteReadOnly([this]() { LDA(); }, IndexType::Y);
            break;
        case 0xBA: // TSX
            TSX();
            break;
        case 0xBC: // LDY Absolute,X
            AbsoluteReadOnly([this]() { LDY(); }, IndexType::X);
            break;
        case 0xBD: // LDA Absolute,X
            AbsoluteReadOnly([this]() { LDA(); }, IndexType::X);
            break;
        case 0xBE: // LDX Absolute,Y
            AbsoluteReadOnly([this]() { LDX(); }, IndexType::Y);
            break;

        // Opcodes 0xC0 to 0xCF
        case 0xC0: // CPY Immediate
            ImmediateReadOnly([this]() { CPY(); });
            break;
        case 0xC1: // CMP (Indirect,X)
            IndexedIndirectReadOnly([this]() { CMP(); });
            break;
        case 0xC4: // CPY Zero Page
            ZeroPageReadOnly([this]() { CPY(); }, IndexType::None);
            break;
        case 0xC5: // CMP Zero Page
            ZeroPageReadOnly([this]() { CMP(); }, IndexType::None);
            break;
        case 0xC6: // DEC Zero Page
            ZeroPageReadModifyWrite([this]() { DEC(); }, IndexType::None);
            break;
        case 0xC8: // INY
            INY();
            break;
        case 0xC9: // CMP Immediate
            ImmediateReadOnly([this]() { CMP(); });
            break;
        case 0xCA: // DEX
            DEX();
            break;
        case 0xCC: // CPY Absolute
            AbsoluteReadOnly([this]() { CPY(); }, IndexType::None);
            break;
        case 0xCD: // CMP Absolute
            AbsoluteReadOnly([this]() { CMP(); }, IndexType::None);
            break;
        case 0xCE: // DEC Absolute
            AbsoluteReadModifyWrite([this]() { DEC(); }, IndexType::None);
            break;

        // Opcodes 0xD0 to 0xDF
        case 0xD0: // BNE
            BranchInstruction([this]() { return GetFlag(Flag::Z) == 0; });
            break;
        case 0xD1: // CMP (Indirect),Y
            IndirectIndexedReadOnly([this]() { CMP(); });
            break;
        case 0xD5: // CMP Zero Page,X
            ZeroPageReadOnly([this]() { CMP(); }, IndexType::X);
            break;
        case 0xD6: // DEC Zero Page,X
            ZeroPageReadModifyWrite([this]() { DEC(); }, IndexType::X);
            break;
        case 0xD8: // CLD
            CLD();
            break;
        case 0xD9: // CMP Absolute,Y
            AbsoluteReadOnly([this]() { CMP(); }, IndexType::Y);
            break;
        case 0xDD: // CMP Absolute,X
            AbsoluteReadOnly([this]() { CMP(); }, IndexType::X);
            break;
        case 0xDE: // DEC Absolute,X
            AbsoluteReadModifyWrite([this]() { DEC(); }, IndexType::X);
            break;

        // Opcodes 0xE0 to 0xEF
        case 0xE0: // CPX Immediate
            ImmediateReadOnly([this]() { CPX(); });
            break;
        case 0xE1: // SBC (Indirect,X)
            IndexedIndirectReadOnly([this]() { SBC(); });
            break;
        case 0xE4: // CPX Zero Page
            ZeroPageReadOnly([this]() { CPX(); }, IndexType::None);
            break;
        case 0xE5: // SBC Zero Page
            ZeroPageReadOnly([this]() { SBC(); }, IndexType::None);
            break;
        case 0xE6: // INC Zero Page
            ZeroPageReadModifyWrite([this]() { INC(); }, IndexType::None);
            break;
        case 0xE8: // INX
            INX();
            break;
        case 0xE9: // SBC Immediate
            ImmediateReadOnly([this]() { SBC(); });
            break;
        case 0xEA: // NOP
            m_microInstructionQueue.push_back([]() {});
            break;
        case 0xEC: // CPX Absolute
            AbsoluteReadOnly([this]() { CPX(); }, IndexType::None);
            break;
        case 0xED: // SBC Absolute
            AbsoluteReadOnly([this]() { SBC(); }, IndexType::None);
            break;
        case 0xEE: // INC Absolute
            AbsoluteReadModifyWrite([this]() { INC(); }, IndexType::None);
            break;

        // Opcodes 0xF0 to 0xFF
        case 0xF0: // BEQ
            BranchInstruction([this]() { return GetFlag(Flag::Z) == 1; });
            break;
        case 0xF1: // SBC (Indirect),Y
            IndirectIndexedReadOnly([this]() { SBC(); });
            break;
        case 0xF5: // SBC Zero Page,X
            ZeroPageReadOnly([this]() { SBC(); }, IndexType::X);
            break;
        case 0xF6: // INC Zero Page,X
            ZeroPageReadModifyWrite([this]() { INC(); }, IndexType::X);
            break;
        case 0xF8: // SED
            SED();
            break;
        case 0xF9: // SBC Absolute,Y
            AbsoluteReadOnly([this]() { SBC(); }, IndexType::Y);
            break;
        case 0xFD: // SBC Absolute,X
            AbsoluteReadOnly([this]() { SBC(); }, IndexType::X);
            break;
        case 0xFE: // INC Absolute,X
            AbsoluteReadModifyWrite([this]() { INC(); }, IndexType::X);
            break;

        // Illegal opcodes
        default: 
            std::cout << "Warning: Unexpected opcode 0x" << std::hex << static_cast<int>(opcode) << std::endl;
            break;
    }
}