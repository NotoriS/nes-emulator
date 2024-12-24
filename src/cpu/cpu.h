#pragma once

#include <queue>
#include <functional>
#include <cstdint>
#include <iostream>

#include "../interfaces/i-bus.h"

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

    CPU(IBus* bus);
    ~CPU();

    void Reset();
    void Clock();

    uint8_t GetFlag(Flag flag);

private:
    IBus* m_bus;

    // Contains "per cycle" instructions that will execute when clock is called.
    std::queue<std::function<void()>> m_microInstructionQueue; 

    uint8_t  reg_a = 0x00;      // Accumulator Register
    uint8_t  reg_x = 0x00;      // X Register
    uint8_t  reg_y = 0x00;      // Y Register
    uint16_t reg_pc = 0x0000;   // Program Counter
    uint8_t  reg_s = 0x00;      // Stack Pointer
    uint8_t  reg_p = 0x00;      // Status Register

    uint16_t m_targetAddress = 0x0000;
    uint8_t m_operand = 0x00;

    bool m_skipNextCycle = false;

    void QueueNextInstuction();

    void Write(uint16_t address, uint8_t data);
    uint8_t Read(uint16_t address);

    void SetFlag(Flag flag, bool value);

    void StackPush(uint8_t value);
    uint8_t StackPop();

#pragma region Instruction Queuing Function Addressing Mode Wrappers
    void ImmediateReadOnly(std::function<void()> operation);

    void AccumulatorReadModifyWrite(std::function<void()> operation);

    void AbsoluteReadOnly(std::function<void()> operation, IndexType indexType);
    void AbsoluteReadModifyWrite(std::function<void()> operation, IndexType indexType);
    void AbsoluteWriteOnly(std::function<void()> operation, IndexType indexType);

    void ZeroPageReadOnly(std::function<void()> operation, IndexType indexType);
    void ZeroPageReadModifyWrite(std::function<void()> operation, IndexType indexType);
    void ZeroPageWriteOnly(std::function<void()> operation, IndexType indexType);

    void IndexedIndirectReadOnly(std::function<void()> operation); // (Indirect,X)
    void IndexedIndirectReadModifyWrite(std::function<void()> operation); // (Indirect,X)
    void IndexedIndirectWriteOnly(std::function<void()> operation); // (Indirect,X)

    void IndirectIndexedReadOnly(std::function<void()> operation); // (Indirect),Y
    void IndirectIndexedReadModifyWrite(std::function<void()> operation); // (Indirect),Y
    void IndirectIndexedWriteOnly(std::function<void()> operation); // (Indirect),Y
#pragma endregion

#pragma region Wrapped Instruction Queuing Functions
    void ADC(); // Add with carry
    void SBC(); // Subtract with carry
    void INC(); // Increment memory
    void DEC(); // Decrement memory
    void AND(); // Bitwise AND
    void ASL(); // Arithmetic shift left
    void LSR(); // Logical shift right
    void ROL(); // Rotate left
    void LDA(); // Load A
    void LDX(); // Load X
    void LDY(); // Load Y
    void STA(); // Store A
    void STX(); // Store X
    void STY(); // Store Y
#pragma endregion

#pragma region Standalone Instruction Queuing Functions
    void BRK(); // Break
    void RTI(); // Return from interrupt
    void RTS(); // Return from subroutine
    void PHA(); // Push the accumulator register to the stack
    void PHP(); // Push the status register to the stack
    void PLA(); // Pull the accumulator register from the stack
    void PLP(); // Pull the status register from the stack
    void JSR(); // Jump to subroutine
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
#pragma endregion
};