#pragma once

#include <cstdint>
#include <memory>

#include "../debug/logger.h"
#include "../interfaces/i-bus.h"
#include "../common/circular-buffer.h"

class CPU
{
private:
    enum class IndexType : uint8_t
    {
        None = 0,
        X = 1,
        Y = 2,
    };

public:
    enum class Flag : uint8_t
    {
        C = 1,          // Carry
        Z = (1 << 1),   // Zero
        I = (1 << 2),   // Interrupt Disable
        D = (1 << 3),   // Decimal Mode
        B = (1 << 4),   // Break
        U = (1 << 5),   // Unused
        V = (1 << 6),   // Overflow
        N = (1 << 7),   // Negative
    };

    enum class InterruptType : uint8_t
    {
        NMI = 0,
        IRQ = 1,
        BRK = 2,
    };

    static constexpr uint16_t NMI_VECTOR = 0xFFFA;
    static constexpr uint16_t RESET_VECTOR = 0xFFFC;
    static constexpr uint16_t IRQ_VECTOR = 0xFFFE;

    static constexpr uint16_t STACK_BASE = 0x0100;

    CPU(std::shared_ptr<IBus> bus);
    ~CPU();

    void Reset();
    void Clock();
    void Interrupt(InterruptType type);

    uint8_t GetFlag(Flag flag) const;

private:
    std::shared_ptr<IBus> m_bus;

    // Contains "per cycle" instructions that will execute when clock is called.
    CircularBuffer<void (CPU::*)(), 20> m_microInstructionQueue;

    uint8_t  reg_a = 0;    // Accumulator Register
    uint8_t  reg_x = 0;    // X Register
    uint8_t  reg_y = 0;    // Y Register
    uint16_t reg_pc = 0;   // Program Counter
    uint8_t  reg_s = 0;    // Stack Pointer
    uint8_t  reg_p = 0;    // Status Register

    uint16_t m_targetAddress = 0;
    uint8_t m_operand = 0;

    void (CPU::*m_operation)() = NULL;
    bool (CPU::* m_branchTest)() const = NULL;
    IndexType m_indexType = IndexType::None;

    bool m_interruptInProgress = false;
    InterruptType m_inProgressInterruptType;
    bool m_pendingNMI = false;
    bool m_pendingIRQ = false;
    uint16_t m_mostRecentInterruptVector = 0;

    void QueueNextInstuction();

    inline void Write(uint16_t address, uint8_t data);
    inline uint8_t Read(uint16_t address);

    void SetFlag(Flag flag, bool value);

    void StackPush(uint8_t value);
    uint8_t StackPop();

    void PollInterrupts();
    void InternalInterrupt(InterruptType type);

#pragma region Instruction Queuing Function Addressing Mode Wrappers
    void ImmediateReadOnly();

    void AccumulatorReadModifyWrite();

    void AbsoluteReadOnly();
    void AbsoluteReadModifyWrite();
    void AbsoluteWriteOnly();

    void ZeroPageReadOnly();
    void ZeroPageReadModifyWrite();
    void ZeroPageWriteOnly();

    void IndexedIndirectReadOnly(); // (Indirect,X)
    void IndexedIndirectReadModifyWrite(); // (Indirect,X)
    void IndexedIndirectWriteOnly(); // (Indirect,X)

    void IndirectIndexedReadOnly(); // (Indirect),Y
    void IndirectIndexedReadModifyWrite(); // (Indirect),Y
    void IndirectIndexedWriteOnly(); // (Indirect),Y

    void BranchInstruction();
#pragma endregion

#pragma region Operations Used With Multiple Addressing Modes
    void ADC(); // Add with carry
    void SBC(); // Subtract with carry
    void INC(); // Increment memory
    void DEC(); // Decrement memory
    void AND(); // Bitwise AND
    void ORA(); // Bitwise OR
    void EOR(); // Bitwise exclusive OR
    void BIT(); // Bit test
    void ASL(); // Arithmetic shift left
    void LSR(); // Logical shift right
    void ROL(); // Rotate left
    void ROR(); // Rotate right
    void LDA(); // Load A
    void LDX(); // Load X
    void LDY(); // Load Y
    void STA(); // Store A
    void STX(); // Store X
    void STY(); // Store Y
    void CMP(); // Compare A
    void CPX(); // Compare X
    void CPY(); // Compare Y
#pragma endregion

#pragma region Standalone Instruction Queuing Functions
    void RTI(); // Return from interrupt
    void RTS(); // Return from subroutine
    void PHA(); // Push the accumulator register to the stack
    void PHP(); // Push the status register to the stack
    void PLA(); // Pull the accumulator register from the stack
    void PLP(); // Pull the status register from the stack
    void JSR(); // Jump to subroutine

    // ==== Two Cycle Instructions ====
    void TAX(); // Transfer A to X
    void TXA(); // Transfer X to A
    void TAY(); // Transfer A to Y
    void TYA(); // Transfer Y to A
    void TXS(); // Transfer X to Stack Pointer
    void TSX(); // Transfer Stack Pointer to X
    void INX(); // Increment X register
    void DEX(); // Decrement X register
    void INY(); // Increment Y register
    void DEY(); // Decrement Y register
    void SEC(); // Set carry
    void CLC(); // Clear carry
    void SEI(); // Set interrupt disable
    void CLI(); // Clear interrupt disable
    void SED(); // Set decimal
    void CLD(); // Clear decimal
    void CLV(); // Clear overflow

    // ==== JMP Instructions For All Addressing Modes ====
    void JMP_Absolute(); // Absolute addressed jump
    void JMP_Indirect(); // Indirect addressed jump
#pragma endregion

#pragma region Micro Instructions
    void ReadProgramCounter();
    void IncrementStackPointer();
    void SetTargetAddressLowByteUsingPC();
    void SetTargetAddressHighByteUsingPC();
    void SetTargetAddressHighByteUsingXIndexedPC();
    void SetTargetAddressHighByteUsingYIndexedPC();
    void AddRegisterXToZeroPageTargetAddress();
    void AddRegisterYToZeroPageTargetAddress();
    void ReadOperandAtPCAndPerformOperation();
    void PerformOperationOnRegA();
    void PerformInvalidTargetAddressRead();
    void FetchHighByteForAbsoluteReadOnly();
    void PerformOperationOnTargetAddress();
    void ReadOperandFromTargetAddress();
    void WriteOperandToTargetAddress();
    void ReadOperandAtPCAndIncrementPC();
    void AddRegisterXToOperand();
    void SetTargetAddressLowByteUsingOperand();
    void SetTargetAddressHighByteUsingOperand();
    void FetchHighByteForIndirectIndexedReadOnly();
    void SetTargetAddressHighByteUsingYIndexedOperand();
    void CheckBranchCondition();
    void PerformBranch();
    void BlankMicroInstruction();
    void PopStatusOffTheStackAndIncrementStackPointer();
    void PopPCLowByteOffTheStackAndIncrementStackPointer();
    void PopPCHighByteOffTheStack();
    void IncrementProgramCounter();
    void PopStatusOffTheStack();
    void PushAccumulatorToTheStack();
    void PushStatusToTheStackWithBSet();
    void PopAccumulatorFromTheStackAndSetFlags();
    void ReadFromTheStackPointer();
    void PushPCHighByteToTheStack();
    void PushPCLowByteToTheStack();
    void JumpToSubroutineFinal();
    void JumpAbsoluteFinal();
    void JumpIndirectFinal();
    void PushStatusAndDecideFinalInterruptVector();
    void SetPCLowByteAndSetInterruptFlag();
    void SetPCHighByteAndClearInterruptInProgress();
    void SetInterruptInProgress();
    void SetInterruptInProgressAndIncrementPC();

    // ==== Branch Tests ====
    bool PlusTest() const;
    bool MinusTest() const;
    bool OverflowClearTest() const;
    bool OverflowSetTest() const;
    bool CarryClearTest() const;
    bool CarrySetTest() const;
    bool NotEqualTest() const;
    bool EqualTest() const;
#pragma endregion
};