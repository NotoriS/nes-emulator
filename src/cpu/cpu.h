#pragma once

#include <queue>
#include <functional>
#include <cstdint>
#include <iostream>

#include "../interfaces/i-bus.h"

class CPU
{
public:
    CPU();
    ~CPU();

    void ConnectBus(IBus* bus) { m_bus = bus; }

    void Clock();

private:
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

    enum class AddressIndex : uint8_t
    {
        None = 0,
        X = 1,
        Y = 2,
    };

    IBus* m_bus = nullptr;

    // Contains "per cycle" instructions that will execute when clock is called.
    std::queue<std::function<void()>> m_microInstructionQueue; 

    uint8_t  reg_a = 0x00;      // Accumulator Register
    uint8_t  reg_x = 0x00;      // X Register
    uint8_t  reg_y = 0x00;      // Y Register
    uint16_t reg_pc = 0xFFFC;   // Program Counter
    uint8_t  reg_s = 0xFD;      // Stack Pointer
    uint8_t  reg_p = 0x20;      // Status Register

    uint16_t m_targetAddress = 0x0000;
    uint8_t m_operand = 0x00;

    bool m_skipNextCycle = false;

    void QueueNextInstuction();

    void Write(uint16_t address, uint8_t data);
    uint8_t Read(uint16_t address);

    uint8_t GetFlag(Flag flag);
    void SetFlag(Flag flag, bool value);

    void StackPush(uint8_t value);
    uint8_t StackPop();

    // Instruction Queuing Fragments

    void Break();
    void ReturnFromInterrupt();
    void ReturnFromSubroutine();
    void PushA();
    void PushP();
    void PullA();
    void PullP();
    void JumpToSubroutine();

    void ImmediateReadOnly(std::function<void()> operation);

    void AbsoluteReadOnly(std::function<void()> operation, AddressIndex indexType);
    void AbsoluteReadModifyWrite(std::function<void()> operation, AddressIndex indexType);
    void AbsoluteWriteOnly(std::function<void()> operation, AddressIndex indexType);

    void ZeroPageReadOnly(std::function<void()> operation, AddressIndex indexType);
    void ZeroPageReadModifyWrite(std::function<void()> operation, AddressIndex indexType);
    void ZeroPageWriteOnly(std::function<void()> operation, AddressIndex indexType);

    void IndexedIndirectReadOnly(std::function<void()> operation); // (Indirect,X)
    void IndexedIndirectReadModifyWrite(std::function<void()> operation); // (Indirect,X)
    void IndexedIndirectWriteOnly(std::function<void()> operation); // (Indirect,X)
};