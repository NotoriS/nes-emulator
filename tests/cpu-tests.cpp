#include <gtest/gtest.h>
#include <memory>

#include "../src/cpu/cpu.h"
#include "fakes/fake-cpu-bus.h"

TEST(CpuTests, ZeroPage_ASL)
{
    auto bus = std::make_shared<TestCpuBus>();
    auto cpu = std::make_shared<CPU>(bus);

    // Write test data
    bus->Write(0x00F6, 2);

    // Add instruction to program memory
    bus->Write(0, 0x06);

    // Add target address to program memory
    bus->Write(1, 0xF6);

    for (int i = 0; i < 5; i++) { cpu->Clock(); }

    EXPECT_EQ(4, bus->Read(0x00F6));
}

TEST(CpuTests, Absolute_ASL)
{
    auto bus = std::make_shared<TestCpuBus>();
    auto cpu = std::make_shared<CPU>(bus);

    // Write test data
    bus->Write(0x0EF6, 2);

    // Add instruction to program memory
    bus->Write(0, 0x0E);

    // Add target address to program memory
    bus->Write(1, 0xF6);
    bus->Write(2, 0x0E);

    for (int i = 0; i < 6; i++) { cpu->Clock(); }

    EXPECT_EQ(4, bus->Read(0x0EF6));
}

TEST(CpuTests, AbsoluteX_ASL)
{
    auto bus = std::make_shared<TestCpuBus>();
    auto cpu = std::make_shared<CPU>(bus);

    // Write test data
    bus->Write(0x2F26, 2);

    // Add load X instruction and address to memory
    bus->Write(0, 0xA2);
    bus->Write(1, 3);

    // Add instruction to program memory
    bus->Write(2, 0x1E);

    // Add target address to program memory
    bus->Write(3, 0x23);
    bus->Write(4, 0x2F);

    for (int i = 0; i < 9; i++) { cpu->Clock(); }

    EXPECT_EQ(4, bus->Read(0x2F26));
}

TEST(CpuTests, ZeroPage_LSR)
{
    auto bus = std::make_shared<TestCpuBus>();
    auto cpu = std::make_shared<CPU>(bus);

    // Write test data
    bus->Write(0x0011, 9);

    // Add LSR instruction to program memory
    bus->Write(0, 0x46);
    bus->Write(1, 0x11);

    for (int i = 0; i < 5; i++) { cpu->Clock(); }

    EXPECT_EQ(4, bus->Read(0x0011)) << "Value should have been 4 after performing LSR on 9";
    EXPECT_EQ(1, cpu->GetFlag(CPU::Flag::C)) << "Carry flag was cleared after performing LSR on 9";
    EXPECT_EQ(0, cpu->GetFlag(CPU::Flag::Z)) << "Zero flag was set after performing LSR on 9";
    EXPECT_EQ(0, cpu->GetFlag(CPU::Flag::N)) << "Negative flag was set after performing LSR on 9";
}

TEST(CpuTests, Absolute_ROL)
{
    auto bus = std::make_shared<TestCpuBus>();
    auto cpu = std::make_shared<CPU>(bus);

    // Write test data
    bus->Write(0x0045, 0b01101010);

    for (int i = 0; i < 9; i++)
    {
        // Add instruction to program memory
        bus->Write(i * 2, 0x26);

        // Add target address to program memory
        bus->Write((i * 2) + 1, 0x45);
    }

    // Check value after 4 rotate left calls
    for (int i = 0; i < 20; i++) { cpu->Clock(); }
    EXPECT_EQ(0b10100011, bus->Read(0x0045));

    // Check value after 9 rotate left calls
    for (int i = 0; i < 25; i++) { cpu->Clock(); }
    EXPECT_EQ(0b01101010, bus->Read(0x0045));
}

TEST(CpuTests, Absolute_ROR)
{
    auto bus = std::make_shared<TestCpuBus>();
    auto cpu = std::make_shared<CPU>(bus);

    // Write test data
    bus->Write(0x0045, 0b01101010);

    for (int i = 0; i < 9; i++)
    {
        // Add instruction to program memory
        bus->Write(i * 2, 0x66);

        // Add target address to program memory
        bus->Write((i * 2) + 1, 0x45);
    }

    // Check value after 4 rotate right calls
    for (int i = 0; i < 20; i++) { cpu->Clock(); }
    EXPECT_EQ(0b01000110, bus->Read(0x0045));

    // Check value after 9 rotate right calls
    for (int i = 0; i < 25; i++) { cpu->Clock(); }
    EXPECT_EQ(0b01101010, bus->Read(0x0045));
}

TEST(CpuTests, Absolute_LDA_STA)
{
    auto bus = std::make_shared<TestCpuBus>();
    auto cpu = std::make_shared<CPU>(bus);

    // Write test data
    bus->Write(0x1EF6, 5);

    // Add load instruction and address to memory
    bus->Write(0, 0xAD);
    bus->Write(1, 0xF6);
    bus->Write(2, 0x1E);

    // Add store instruction and address to memory
    bus->Write(3, 0x8D);
    bus->Write(4, 0x45);
    bus->Write(5, 0x26);

    for (int i = 0; i < 8; i++) { cpu->Clock(); }

    EXPECT_EQ(5, bus->Read(0x2645));
}

TEST(CpuTests, Absolute_LDX_STX)
{
    auto bus = std::make_shared<TestCpuBus>();
    auto cpu = std::make_shared<CPU>(bus);

    // Write test data
    bus->Write(0x1EF6, 5);

    // Add load instruction and address to memory
    bus->Write(0, 0xAE);
    bus->Write(1, 0xF6);
    bus->Write(2, 0x1E);

    // Add store instruction and address to memory
    bus->Write(3, 0x8E);
    bus->Write(4, 0x45);
    bus->Write(5, 0x26);

    for (int i = 0; i < 8; i++) { cpu->Clock(); }

    EXPECT_EQ(5, bus->Read(0x2645));
}

TEST(CpuTests, Absolute_LDY_STY)
{
    auto bus = std::make_shared<TestCpuBus>();
    auto cpu = std::make_shared<CPU>(bus);

    // Write test data
    bus->Write(0x1EF6, 5);

    // Add load instruction and address to memory
    bus->Write(0, 0xAC);
    bus->Write(1, 0xF6);
    bus->Write(2, 0x1E);

    // Add store instruction and address to memory
    bus->Write(3, 0x8C);
    bus->Write(4, 0x45);
    bus->Write(5, 0x26);

    for (int i = 0; i < 8; i++) { cpu->Clock(); }

    EXPECT_EQ(5, bus->Read(0x2645));
}

TEST(CpuTests, TAX)
{
    auto bus = std::make_shared<TestCpuBus>();
    auto cpu = std::make_shared<CPU>(bus);

    // Add load A instruction and address to memory
    bus->Write(0, 0xA9);
    bus->Write(1, 5);

    // Add transfer instruction to memory
    bus->Write(2, 0xAA);

    // Add store X instruction and address to memory
    bus->Write(3, 0x8E);
    bus->Write(4, 0x45);
    bus->Write(5, 0x26);

    for (int i = 0; i < 8; i++) { cpu->Clock(); }

    EXPECT_EQ(5, bus->Read(0x2645));
}

TEST(CpuTests, TAY)
{
    auto bus = std::make_shared<TestCpuBus>();
    auto cpu = std::make_shared<CPU>(bus);

    // Add load A instruction and address to memory
    bus->Write(0, 0xA9);
    bus->Write(1, 5);

    // Add transfer instruction to memory
    bus->Write(2, 0xA8);

    // Add store Y instruction and address to memory
    bus->Write(3, 0x8C);
    bus->Write(4, 0x45);
    bus->Write(5, 0x26);

    for (int i = 0; i < 8; i++) { cpu->Clock(); }

    EXPECT_EQ(5, bus->Read(0x2645));
}

TEST(CpuTests, TSX)
{
    auto bus = std::make_shared<TestCpuBus>();
    auto cpu = std::make_shared<CPU>(bus);

    // Add transfer instruction to memory
    bus->Write(0, 0xBA);

    // Add store X instruction and address to memory
    bus->Write(1, 0x8E);
    bus->Write(2, 0x45);
    bus->Write(3, 0x26);

    for (int i = 0; i < 6; i++) { cpu->Clock(); }

    EXPECT_EQ(0xFD, bus->Read(0x2645));
}

TEST(CpuTests, TYA)
{
    auto bus = std::make_shared<TestCpuBus>();
    auto cpu = std::make_shared<CPU>(bus);

    // Add load Y instruction and address to memory
    bus->Write(0, 0xA0);
    bus->Write(1, 5);

    // Add transfer instruction to memory
    bus->Write(2, 0x98);

    // Add store A instruction and address to memory
    bus->Write(3, 0x8D);
    bus->Write(4, 0x45);
    bus->Write(5, 0x26);

    for (int i = 0; i < 8; i++) { cpu->Clock(); }

    EXPECT_EQ(5, bus->Read(0x2645));
}

TEST(CpuTests, TXA)
{
    auto bus = std::make_shared<TestCpuBus>();
    auto cpu = std::make_shared<CPU>(bus);

    // Add load X instruction and address to memory
    bus->Write(0, 0xA2);
    bus->Write(1, 5);

    // Add transfer instruction to memory
    bus->Write(2, 0x8A);

    // Add store A instruction and address to memory
    bus->Write(3, 0x8D);
    bus->Write(4, 0x45);
    bus->Write(5, 0x26);

    for (int i = 0; i < 8; i++) { cpu->Clock(); }

    EXPECT_EQ(5, bus->Read(0x2645));
}

TEST(CpuTests, TXS_TSX)
{
    auto bus = std::make_shared<TestCpuBus>();
    auto cpu = std::make_shared<CPU>(bus);

    // Add load X instruction to memory
    bus->Write(0, 0xA2);
    bus->Write(1, 5);

    // Add TXS instruction to memory
    bus->Write(2, 0x9A);

    // Set X to something else
    bus->Write(3, 0xA2);
    bus->Write(4, 7);

    // Add store X instruction and address to memory
    bus->Write(5, 0x8E);
    bus->Write(6, 0x45);
    bus->Write(7, 0x26);

    // Check that X was set properly
    for (int i = 0; i < 10; i++) { cpu->Clock(); }
    EXPECT_EQ(7, bus->Read(0x2645));

    // Add TSX instruction to memory
    bus->Write(8, 0xBA);

    // Add store X instruction and address to memory
    bus->Write(9, 0x8E);
    bus->Write(10, 0x45);
    bus->Write(11, 0x26);

    // Check that X was changed to the value of SP
    for (int i = 0; i < 6; i++) { cpu->Clock(); }
    EXPECT_EQ(5, bus->Read(0x2645));
}

TEST(CpuTests, Immediate_ADC)
{
    auto bus = std::make_shared<TestCpuBus>();
    auto cpu = std::make_shared<CPU>(bus);

    // Add ADC instruction to program memory
    bus->Write(0, 0x69);
    bus->Write(1, 130);

    // Add store A instruction to program memory
    bus->Write(2, 0x8D);
    bus->Write(3, 0x44);
    bus->Write(4, 0x44);

    for (int i = 0; i < 6; i++) { cpu->Clock(); }

    // Check state after subtraction with overflow and carry cleared
    EXPECT_EQ(0, cpu->GetFlag(CPU::Flag::C)) << "Carry flag is set after normal ADC";
    EXPECT_EQ(130, bus->Read(0x4444)) << "ADC did not add properly with the C flag cleared";

    // Add ADC instruction to program memory
    bus->Write(5, 0x69);
    bus->Write(6, 130);

    // Add store A instruction to program memory
    bus->Write(7, 0x8D);
    bus->Write(8, 0x55);
    bus->Write(9, 0x55);

    for (int i = 0; i < 6; i++) { cpu->Clock(); }

    // Check state after add with overflow and carry cleared
    EXPECT_EQ(1, cpu->GetFlag(CPU::Flag::C)) << "Carry flag is cleared after ADC with overflow";
    EXPECT_EQ(4, bus->Read(0x5555)) << "ADC did not overflow properly with the C flag cleared";

    // Add ADC instruction to program memory
    bus->Write(10, 0x69);
    bus->Write(11, 130);

    // Add store A instruction to program memory
    bus->Write(12, 0x8D);
    bus->Write(13, 0x44);
    bus->Write(14, 0x44);

    for (int i = 0; i < 6; i++) { cpu->Clock(); }

    // Check state after subtraction with overflow and carry cleared
    EXPECT_EQ(0, cpu->GetFlag(CPU::Flag::C)) << "Carry flag is set after normal ADC";
    EXPECT_EQ(135, bus->Read(0x4444)) << "ADC did not add properly with the C flag set";
}

TEST(CpuTests, Immediate_SBC)
{
    auto bus = std::make_shared<TestCpuBus>();
    auto cpu = std::make_shared<CPU>(bus);

    // Add SBC instruction to program memory
    bus->Write(0, 0xE9);
    bus->Write(1, 1);

    // Add store A instruction to program memory
    bus->Write(2, 0x8D);
    bus->Write(3, 0x44);
    bus->Write(4, 0x44);

    for (int i = 0; i < 6; i++) { cpu->Clock(); }

    // Check state after subtraction with overflow and carry cleared
    EXPECT_EQ(0, cpu->GetFlag(CPU::Flag::C)) << "Carry flag is set after SBC underflows";
    EXPECT_EQ(254, bus->Read(0x4444)) << "SBC did not subtract properly with the C flag cleared";

    // Add SBC instruction to program memory
    bus->Write(5, 0xE9);
    bus->Write(6, 14);

    // Add store A instruction to program memory
    bus->Write(7, 0x8D);
    bus->Write(8, 0x55);
    bus->Write(9, 0x55);

    for (int i = 0; i < 6; i++) { cpu->Clock(); }

    // Check state after subtraction with no overflow and carry cleared
    EXPECT_EQ(1, cpu->GetFlag(CPU::Flag::C)) << "Carry flag is cleared after normal SBC";
    EXPECT_EQ(239, bus->Read(0x5555)) << "SBC did not underflow properly with the C flag cleared";

    // Add SBC instruction to program memory
    bus->Write(10, 0xE9);
    bus->Write(11, 9);

    // Add store A instruction to program memory
    bus->Write(12, 0x8D);
    bus->Write(13, 0x66);
    bus->Write(14, 0x66);

    for (int i = 0; i < 6; i++) { cpu->Clock(); }

    // Check state after subtraction with no overflow and carry set
    EXPECT_EQ(1, cpu->GetFlag(CPU::Flag::C)) << "Carry flag is cleared after normal SBC";
    EXPECT_EQ(230, bus->Read(0x6666)) << "SBC did not subtract properly with the C flag set";
}

TEST(CpuTests, ZeroPage_INC)
{
    auto bus = std::make_shared<TestCpuBus>();
    auto cpu = std::make_shared<CPU>(bus);

    // Write value to increment
    bus->Write(0x0047, 0xFF);

    // Add INC instruction to program memory
    bus->Write(0, 0xE6);
    bus->Write(1, 0x47);

    for (int i = 0; i < 5; i++) { cpu->Clock(); }

    // Check state after overflowing INC
    EXPECT_EQ(0, bus->Read(0x0047)) << "Memory value does not equal zero after incrementing 0xFF";
    EXPECT_EQ(1, cpu->GetFlag(CPU::Flag::Z)) << "Zero flag is not set when INC results in 0";
}

TEST(CpuTests, ZeroPage_DEC)
{
    auto bus = std::make_shared<TestCpuBus>();
    auto cpu = std::make_shared<CPU>(bus);

    // Add DEC instruction to program memory
    bus->Write(0, 0xC6);
    bus->Write(1, 0x47);

    for (int i = 0; i < 5; i++) { cpu->Clock(); }

    // Check state after overflowing INC
    EXPECT_EQ(0xFF, bus->Read(0x0047)) << "Memory value does not equal zero after incrementing 0xFF";
    EXPECT_EQ(0, cpu->GetFlag(CPU::Flag::Z)) << "Zero flag is set when DEC results in 0xFF";
    EXPECT_EQ(1, cpu->GetFlag(CPU::Flag::N)) << "Negative flag is not set when DEC results in 0xFF";
}

TEST(CpuTests, INX)
{
    auto bus = std::make_shared<TestCpuBus>();
    auto cpu = std::make_shared<CPU>(bus);

    // Add INX to program memory
    bus->Write(0, 0xE8);

    // Add store X to program memory
    bus->Write(1, 0x86);
    bus->Write(2, 0x11);

    for (int i = 0; i < 5; i++) { cpu->Clock(); }

    // Check state after INX
    EXPECT_EQ(0x01, bus->Read(0x0011)) << "Memory value does not equal 0x01 after incrementing X = 0x00";
    EXPECT_EQ(0, cpu->GetFlag(CPU::Flag::Z)) << "Zero flag is set when INX results in 0x01";
    EXPECT_EQ(0, cpu->GetFlag(CPU::Flag::N)) << "Negative flag is set when INX results in 0x01";
}

TEST(CpuTests, DEX)
{
    auto bus = std::make_shared<TestCpuBus>();
    auto cpu = std::make_shared<CPU>(bus);

    // Add DEX to program memory
    bus->Write(0, 0xCA);

    // Add store X to program memory
    bus->Write(1, 0x86);
    bus->Write(2, 0x11);

    for (int i = 0; i < 5; i++) { cpu->Clock(); }

    // Check state after DEX
    EXPECT_EQ(0xFF, bus->Read(0x0011)) << "Memory value does not equal 0xFF after decrementing X = 0x00";
    EXPECT_EQ(0, cpu->GetFlag(CPU::Flag::Z)) << "Zero flag is set when DEX results in 0xFF";
    EXPECT_EQ(1, cpu->GetFlag(CPU::Flag::N)) << "Negative flag is not set when DEX results in 0xFF";
}

TEST(CpuTests, INY)
{
    auto bus = std::make_shared<TestCpuBus>();
    auto cpu = std::make_shared<CPU>(bus);

    // Add INY to program memory
    bus->Write(0, 0xC8);

    // Add store Y to program memory
    bus->Write(1, 0x84);
    bus->Write(2, 0x11);

    for (int i = 0; i < 5; i++) { cpu->Clock(); }

    // Check state after INY
    EXPECT_EQ(0x01, bus->Read(0x0011)) << "Memory value does not equal 0x01 after incrementing Y = 0x00";
    EXPECT_EQ(0, cpu->GetFlag(CPU::Flag::Z)) << "Zero flag is set when INY results in 0x01";
    EXPECT_EQ(0, cpu->GetFlag(CPU::Flag::N)) << "Negative flag is set when INY results in 0x01";
}

TEST(CpuTests, DEY)
{
    auto bus = std::make_shared<TestCpuBus>();
    auto cpu = std::make_shared<CPU>(bus);

    // Add DEY to program memory
    bus->Write(0, 0x88);

    // Add store Y to program memory
    bus->Write(1, 0x84);
    bus->Write(2, 0x11);

    for (int i = 0; i < 5; i++) { cpu->Clock(); }

    // Check state after DEY
    EXPECT_EQ(0xFF, bus->Read(0x0011)) << "Memory value does not equal 0xFF after decrementing Y = 0x00";
    EXPECT_EQ(0, cpu->GetFlag(CPU::Flag::Z)) << "Zero flag is set when DEY results in 0xFF";
    EXPECT_EQ(1, cpu->GetFlag(CPU::Flag::N)) << "Negative flag is not set when DEY results in 0xFF";
}

TEST(CpuTests, Immediate_AND)
{
    auto bus = std::make_shared<TestCpuBus>();
    auto cpu = std::make_shared<CPU>(bus);

    // Add load A to program memory
    bus->Write(0, 0xA9);
    bus->Write(1, 0b00101100);

    // Add AND to program memory
    bus->Write(2, 0x29);
    bus->Write(3, 0b00101011);

    // Add store A instruction to program memory
    bus->Write(4, 0x85);
    bus->Write(5, 0x11);

    for (int i = 0; i < 7; i++) { cpu->Clock(); }

    EXPECT_EQ(0b00101000, bus->Read(0x0011)) << "AND results in the wrong value in the accumulator";
    EXPECT_EQ(0, cpu->GetFlag(CPU::Flag::Z)) << "Z flag incorrectly set after AND";
    EXPECT_EQ(0, cpu->GetFlag(CPU::Flag::N)) << "N flag incorrectly set after AND";
}

TEST(CpuTests, Immediate_ORA)
{
    auto bus = std::make_shared<TestCpuBus>();
    auto cpu = std::make_shared<CPU>(bus);

    // Add load A to program memory
    bus->Write(0, 0xA9);
    bus->Write(1, 0b00101100);

    // Add ORA to program memory
    bus->Write(2, 0x09);
    bus->Write(3, 0b10100001);

    // Add store A instruction to program memory
    bus->Write(4, 0x85);
    bus->Write(5, 0x55);

    for (int i = 0; i < 7; i++) { cpu->Clock(); }

    EXPECT_EQ(0b10101101, bus->Read(0x0055)) << "ORA results in the wrong value in the accumulator";
    EXPECT_EQ(0, cpu->GetFlag(CPU::Flag::Z)) << "Z flag incorrectly set after ORA";
    EXPECT_EQ(1, cpu->GetFlag(CPU::Flag::N)) << "N flag incorrectly cleared after ORA";
}

TEST(CpuTests, Immediate_EOR)
{
    auto bus = std::make_shared<TestCpuBus>();
    auto cpu = std::make_shared<CPU>(bus);

    // Add load A to program memory
    bus->Write(0, 0xA9);
    bus->Write(1, 0b00101100);

    // Add EOR to program memory
    bus->Write(2, 0x49);
    bus->Write(3, 0b10001001);

    // Add store A instruction to program memory
    bus->Write(4, 0x85);
    bus->Write(5, 0x55);

    for (int i = 0; i < 7; i++) { cpu->Clock(); }

    EXPECT_EQ(0b10100101, bus->Read(0x0055)) << "EOR results in the wrong value in the accumulator";
    EXPECT_EQ(0, cpu->GetFlag(CPU::Flag::Z)) << "Z flag incorrectly set after EOR";
    EXPECT_EQ(1, cpu->GetFlag(CPU::Flag::N)) << "N flag incorrectly cleared after EOR";
}

TEST(CpuTests, ZeroPage_BIT)
{
    auto bus = std::make_shared<TestCpuBus>();
    auto cpu = std::make_shared<CPU>(bus);

    // Add load A to program memory
    bus->Write(0, 0xA9);
    bus->Write(1, 0b00101100);

    // Write test data
    bus->Write(0x0044, 0b11000011);

    // Add BIT to program memory
    bus->Write(2, 0x24);
    bus->Write(3, 0x44);

    // Add store A instruction to program memory
    bus->Write(4, 0x85);
    bus->Write(5, 0x55);

    for (int i = 0; i < 8; i++) { cpu->Clock(); }

    EXPECT_EQ(1, cpu->GetFlag(CPU::Flag::Z)) << "Z flag incorrectly cleared after BIT";
    EXPECT_EQ(1, cpu->GetFlag(CPU::Flag::V)) << "V flag incorrectly cleared after BIT";
    EXPECT_EQ(1, cpu->GetFlag(CPU::Flag::N)) << "N flag incorrectly cleared after BIT";
}

TEST(CpuTests, Immediate_CMP)
{
    auto bus = std::make_shared<TestCpuBus>();
    auto cpu = std::make_shared<CPU>(bus);

    // Add load A to program memory
    bus->Write(0, 0xA9);
    bus->Write(1, 0xAF);

    // Add CMP to program memory
    bus->Write(2, 0xC9);
    bus->Write(3, 0x23);

    for (int i = 0; i < 4; i++) { cpu->Clock(); }

    EXPECT_EQ(1, cpu->GetFlag(CPU::Flag::C)) << "C flag incorrectly cleared after CMP";
    EXPECT_EQ(0, cpu->GetFlag(CPU::Flag::Z)) << "Z flag incorrectly set after CMP";
    EXPECT_EQ(1, cpu->GetFlag(CPU::Flag::N)) << "N flag incorrectly cleared after CMP";
}

TEST(CpuTests, Immediate_CPX)
{
    auto bus = std::make_shared<TestCpuBus>();
    auto cpu = std::make_shared<CPU>(bus);

    // Add load X to program memory
    bus->Write(0, 0xA2);
    bus->Write(1, 0xAF);

    // Add CPX to program memory
    bus->Write(2, 0xE0);
    bus->Write(3, 0x23);

    for (int i = 0; i < 4; i++) { cpu->Clock(); }

    EXPECT_EQ(1, cpu->GetFlag(CPU::Flag::C)) << "C flag incorrectly cleared after CPX";
    EXPECT_EQ(0, cpu->GetFlag(CPU::Flag::Z)) << "Z flag incorrectly set after CPX";
    EXPECT_EQ(1, cpu->GetFlag(CPU::Flag::N)) << "N flag incorrectly cleared after CPX";
}

TEST(CpuTests, Immediate_CPY)
{
    auto bus = std::make_shared<TestCpuBus>();
    auto cpu = std::make_shared<CPU>(bus);

    // Add load Y to program memory
    bus->Write(0, 0xA0);
    bus->Write(1, 0xAF);

    // Add CPY to program memory
    bus->Write(2, 0xC0);
    bus->Write(3, 0x23);

    for (int i = 0; i < 4; i++) { cpu->Clock(); }

    EXPECT_EQ(1, cpu->GetFlag(CPU::Flag::C)) << "C flag incorrectly cleared after CPY";
    EXPECT_EQ(0, cpu->GetFlag(CPU::Flag::Z)) << "Z flag incorrectly set after CPY";
    EXPECT_EQ(1, cpu->GetFlag(CPU::Flag::N)) << "N flag incorrectly cleared after CPY";
}

TEST(CpuTests, SEC_CLC)
{
    auto bus = std::make_shared<TestCpuBus>();
    auto cpu = std::make_shared<CPU>(bus);

    // Add set and clear carry to program memory
    bus->Write(0, 0x38);
    bus->Write(1, 0x18);

    for (int i = 0; i < 2; i++) { cpu->Clock(); }
    EXPECT_EQ(1, cpu->GetFlag(CPU::Flag::C)) << "C flag incorrectly cleared after SEC";

    for (int i = 0; i < 2; i++) { cpu->Clock(); }
    EXPECT_EQ(0, cpu->GetFlag(CPU::Flag::C)) << "C flag incorrectly set after CLC";
}

TEST(CpuTests, SEI_CLI)
{
    auto bus = std::make_shared<TestCpuBus>();
    auto cpu = std::make_shared<CPU>(bus);

    // Add set and clear interrupt disable to program memory
    bus->Write(0, 0x78);
    bus->Write(1, 0x58);

    for (int i = 0; i < 2; i++) { cpu->Clock(); }
    EXPECT_EQ(1, cpu->GetFlag(CPU::Flag::I)) << "I flag incorrectly cleared after SEI";

    for (int i = 0; i < 2; i++) { cpu->Clock(); }
    EXPECT_EQ(0, cpu->GetFlag(CPU::Flag::I)) << "I flag incorrectly set after CLI";
}

TEST(CpuTests, SED_CLD)
{
    auto bus = std::make_shared<TestCpuBus>();
    auto cpu = std::make_shared<CPU>(bus);

    // Add set and clear decimal to program memory
    bus->Write(0, 0xF8);
    bus->Write(1, 0xD8);

    for (int i = 0; i < 2; i++) { cpu->Clock(); }
    EXPECT_EQ(1, cpu->GetFlag(CPU::Flag::D)) << "D flag incorrectly cleared after SED";

    for (int i = 0; i < 2; i++) { cpu->Clock(); }
    EXPECT_EQ(0, cpu->GetFlag(CPU::Flag::D)) << "D flag incorrectly set after CLD";
}

TEST(CpuTests, CLV)
{
    auto bus = std::make_shared<TestCpuBus>();
    auto cpu = std::make_shared<CPU>(bus);

    // Add overflowing ADC instructions to program memory
    bus->Write(0, 0x69);
    bus->Write(1, 100);
    bus->Write(2, 0x69);
    bus->Write(3, 100);

    for (int i = 0; i < 4; i++) { cpu->Clock(); }
    EXPECT_EQ(1, cpu->GetFlag(CPU::Flag::V)) << "V flag incorrectly cleared after overflowing ADC";

    // Add clear overflow to program memory
    bus->Write(4, 0xB8);

    for (int i = 0; i < 2; i++) { cpu->Clock(); }
    EXPECT_EQ(0, cpu->GetFlag(CPU::Flag::V)) << "V flag incorrectly set after CLV";
}

TEST(CpuTests, BCC_Positive_Offset)
{
    auto bus = std::make_shared<TestCpuBus>();
    auto cpu = std::make_shared<CPU>(bus);

    // Add a branch to byte 47 into program memory
    bus->Write(0, 0x90);
    bus->Write(1, 45);

    // Add a SED instruction at byte 47
    bus->Write(47, 0xF8);

    for (int i = 0; i < 5; i++) { cpu->Clock(); }
    EXPECT_EQ(1, cpu->GetFlag(CPU::Flag::D)) << "D flag incorrectly cleared after an attempted branch to SED";
}

TEST(CpuTests, BCC_Negative_Offset)
{
    auto bus = std::make_shared<TestCpuBus>();
    auto cpu = std::make_shared<CPU>(bus);

    // Add a branch to byte 0 into program memory
    bus->Write(0, 0x90);
    bus->Write(1, 0xFE);

    for (int i = 0; i < 3; i++) { cpu->Clock(); }

    // Add a SED instruction at byte 0
    bus->Write(0, 0xF8);

    for (int i = 0; i < 2; i++) { cpu->Clock(); }
    EXPECT_EQ(1, cpu->GetFlag(CPU::Flag::D)) << "D flag incorrectly cleared after an attempted branch to SED";
}

TEST(CpuTests, BCS_Positive_Offset)
{
    auto bus = std::make_shared<TestCpuBus>();
    auto cpu = std::make_shared<CPU>(bus);

    // Set the carry flag
    bus->Write(0, 0x038);
    for (int i = 0; i < 2; i++) { cpu->Clock(); }

    // Add a branch to byte 47 into program memory
    bus->Write(1, 0xB0);
    bus->Write(2, 44);

    // Add a SED instruction at byte 47
    bus->Write(47, 0xF8);

    for (int i = 0; i < 5; i++) { cpu->Clock(); }
    EXPECT_EQ(1, cpu->GetFlag(CPU::Flag::D)) << "D flag incorrectly cleared after an attempted branch to SED";
}

TEST(CpuTests, BCS_Negative_Offset)
{
    auto bus = std::make_shared<TestCpuBus>();
    auto cpu = std::make_shared<CPU>(bus);

    // Set the carry flag
    bus->Write(0, 0x038);
    for (int i = 0; i < 2; i++) { cpu->Clock(); }

    // Add a branch to byte 0 into program memory
    bus->Write(1, 0xB0);
    bus->Write(2, 0xFD);

    for (int i = 0; i < 3; i++) { cpu->Clock(); }

    // Add a SED instruction at byte 0
    bus->Write(0, 0xF8);

    for (int i = 0; i < 2; i++) { cpu->Clock(); }
    EXPECT_EQ(1, cpu->GetFlag(CPU::Flag::D)) << "D flag incorrectly cleared after an attempted branch to SED";
}

TEST(CpuTests, BCS_No_Branch)
{
    auto bus = std::make_shared<TestCpuBus>();
    auto cpu = std::make_shared<CPU>(bus);

    // Add a branch to byte 47 into program memory
    bus->Write(0, 0xB0);
    bus->Write(1, 45);

    // Add a SED instruction at byte 47
    bus->Write(47, 0xF8);

    for (int i = 0; i < 5; i++) { cpu->Clock(); }
    EXPECT_EQ(0, cpu->GetFlag(CPU::Flag::D)) << "D flag incorrectly set after branch to SED should fail";
}

TEST(CpuTests, BEQ_Positive_Offset)
{
    auto bus = std::make_shared<TestCpuBus>();
    auto cpu = std::make_shared<CPU>(bus);

    // Set the zero flag
    bus->Write(0, 0x69);
    bus->Write(1, 0);

    for (int i = 0; i < 2; i++) { cpu->Clock(); }

    // Add a branch to byte 47 into program memory
    bus->Write(2, 0xF0);
    bus->Write(3, 43);

    // Add a SED instruction at byte 47
    bus->Write(47, 0xF8);

    for (int i = 0; i < 5; i++) { cpu->Clock(); }
    EXPECT_EQ(1, cpu->GetFlag(CPU::Flag::D)) << "D flag incorrectly cleared after an attempted branch to SED";
}

TEST(CpuTests, BEQ_Negative_Offset)
{
    auto bus = std::make_shared<TestCpuBus>();
    auto cpu = std::make_shared<CPU>(bus);

    // Set the zero flag
    bus->Write(0, 0x69);
    bus->Write(1, 0);

    for (int i = 0; i < 2; i++) { cpu->Clock(); }

    // Add a branch to byte 0 into program memory
    bus->Write(2, 0xF0);
    bus->Write(3, 0xFC);

    for (int i = 0; i < 3; i++) { cpu->Clock(); }

    // Add a SED instruction at byte 0
    bus->Write(0, 0xF8);

    for (int i = 0; i < 2; i++) { cpu->Clock(); }
    EXPECT_EQ(1, cpu->GetFlag(CPU::Flag::D)) << "D flag incorrectly cleared after an attempted branch to SED";
}

TEST(CpuTests, BNE_Positive_Offset)
{
    auto bus = std::make_shared<TestCpuBus>();
    auto cpu = std::make_shared<CPU>(bus);

    // Add a branch to byte 47 into program memory
    bus->Write(0, 0xD0);
    bus->Write(1, 45);

    // Add a SED instruction at byte 47
    bus->Write(47, 0xF8);

    for (int i = 0; i < 5; i++) { cpu->Clock(); }
    EXPECT_EQ(1, cpu->GetFlag(CPU::Flag::D)) << "D flag incorrectly cleared after an attempted branch to SED";
}

TEST(CpuTests, BNE_Negative_Offset)
{
    auto bus = std::make_shared<TestCpuBus>();
    auto cpu = std::make_shared<CPU>(bus);

    // Add a branch to byte 0 into program memory
    bus->Write(0, 0xD0);
    bus->Write(1, 0xFE);

    for (int i = 0; i < 3; i++) { cpu->Clock(); }

    // Add a SED instruction at byte 0
    bus->Write(0, 0xF8);

    for (int i = 0; i < 2; i++) { cpu->Clock(); }
    EXPECT_EQ(1, cpu->GetFlag(CPU::Flag::D)) << "D flag incorrectly cleared after an attempted branch to SED";
}

TEST(CpuTests, BPL_Positive_Offset)
{
    auto bus = std::make_shared<TestCpuBus>();
    auto cpu = std::make_shared<CPU>(bus);

    // Add a branch to byte 47 into program memory
    bus->Write(0, 0x10);
    bus->Write(1, 45);

    // Add a SED instruction at byte 47
    bus->Write(47, 0xF8);

    for (int i = 0; i < 5; i++) { cpu->Clock(); }
    EXPECT_EQ(1, cpu->GetFlag(CPU::Flag::D)) << "D flag incorrectly cleared after an attempted branch to SED";
}

TEST(CpuTests, BPL_Negative_Offset)
{
    auto bus = std::make_shared<TestCpuBus>();
    auto cpu = std::make_shared<CPU>(bus);

    // Add a branch to byte 0 into program memory
    bus->Write(0, 0x10);
    bus->Write(1, 0xFE);

    for (int i = 0; i < 3; i++) { cpu->Clock(); }

    // Add a SED instruction at byte 0
    bus->Write(0, 0xF8);

    for (int i = 0; i < 2; i++) { cpu->Clock(); }
    EXPECT_EQ(1, cpu->GetFlag(CPU::Flag::D)) << "D flag incorrectly cleared after an attempted branch to SED";
}

TEST(CpuTests, BMI_Positive_Offset)
{
    auto bus = std::make_shared<TestCpuBus>();
    auto cpu = std::make_shared<CPU>(bus);

    // Set the negative flag
    bus->Write(0, 0x69);
    bus->Write(1, 0x80);

    for (int i = 0; i < 2; i++) { cpu->Clock(); }

    // Add a branch to byte 47 into program memory
    bus->Write(2, 0x30);
    bus->Write(3, 43);

    // Add a SED instruction at byte 47
    bus->Write(47, 0xF8);

    for (int i = 0; i < 5; i++) { cpu->Clock(); }
    EXPECT_EQ(1, cpu->GetFlag(CPU::Flag::D)) << "D flag incorrectly cleared after an attempted branch to SED";
}

TEST(CpuTests, BMI_Negative_Offset)
{
    auto bus = std::make_shared<TestCpuBus>();
    auto cpu = std::make_shared<CPU>(bus);

    // Set the negative flag
    bus->Write(0, 0x69);
    bus->Write(1, 0x80);

    for (int i = 0; i < 2; i++) { cpu->Clock(); }

    // Add a branch to byte 0 into program memory
    bus->Write(2, 0x30);
    bus->Write(3, 0xFC);

    for (int i = 0; i < 3; i++) { cpu->Clock(); }

    // Add a SED instruction at byte 0
    bus->Write(0, 0xF8);

    for (int i = 0; i < 2; i++) { cpu->Clock(); }
    EXPECT_EQ(1, cpu->GetFlag(CPU::Flag::D)) << "D flag incorrectly cleared after an attempted branch to SED";
}

TEST(CpuTests, BVC_Positive_Offset)
{
    auto bus = std::make_shared<TestCpuBus>();
    auto cpu = std::make_shared<CPU>(bus);

    // Add a branch to byte 47 into program memory
    bus->Write(0, 0x50);
    bus->Write(1, 45);

    // Add a SED instruction at byte 47
    bus->Write(47, 0xF8);

    for (int i = 0; i < 5; i++) { cpu->Clock(); }
    EXPECT_EQ(1, cpu->GetFlag(CPU::Flag::D)) << "D flag incorrectly cleared after an attempted branch to SED";
}

TEST(CpuTests, BVC_Negative_Offset)
{
    auto bus = std::make_shared<TestCpuBus>();
    auto cpu = std::make_shared<CPU>(bus);

    // Add a branch to byte 0 into program memory
    bus->Write(0, 0x50);
    bus->Write(1, 0xFE);

    for (int i = 0; i < 3; i++) { cpu->Clock(); }

    // Add a SED instruction at byte 0
    bus->Write(0, 0xF8);

    for (int i = 0; i < 2; i++) { cpu->Clock(); }
    EXPECT_EQ(1, cpu->GetFlag(CPU::Flag::D)) << "D flag incorrectly cleared after an attempted branch to SED";
}

TEST(CpuTests, BVS_Positive_Offset)
{
    auto bus = std::make_shared<TestCpuBus>();
    auto cpu = std::make_shared<CPU>(bus);

    // Set the overflow flag
    bus->Write(0, 0x69);
    bus->Write(1, 0x70);
    bus->Write(2, 0x69);
    bus->Write(3, 0x70);

    for (int i = 0; i < 4; i++) { cpu->Clock(); }

    // Add a branch to byte 47 into program memory
    bus->Write(4, 0x70);
    bus->Write(5, 41);

    // Add a SED instruction at byte 47
    bus->Write(47, 0xF8);

    for (int i = 0; i < 5; i++) { cpu->Clock(); }
    EXPECT_EQ(1, cpu->GetFlag(CPU::Flag::D)) << "D flag incorrectly cleared after an attempted branch to SED";
}

TEST(CpuTests, BVS_Negative_Offset)
{
    auto bus = std::make_shared<TestCpuBus>();
    auto cpu = std::make_shared<CPU>(bus);

    // Set the overflow flag
    bus->Write(0, 0x69);
    bus->Write(1, 0x70);
    bus->Write(2, 0x69);
    bus->Write(3, 0x70);

    for (int i = 0; i < 4; i++) { cpu->Clock(); }

    // Add a branch to byte 0 into program memory
    bus->Write(4, 0x70);
    bus->Write(5, 0xFA);

    for (int i = 0; i < 3; i++) { cpu->Clock(); }

    // Add a SED instruction at byte 0
    bus->Write(0, 0xF8);

    for (int i = 0; i < 2; i++) { cpu->Clock(); }
    EXPECT_EQ(1, cpu->GetFlag(CPU::Flag::D)) << "D flag incorrectly cleared after an attempted branch to SED";
}

TEST(CpuTests, JMP_Absolute)
{
    auto bus = std::make_shared<TestCpuBus>();
    auto cpu = std::make_shared<CPU>(bus);

    // Add jump instruction to program memory
    bus->Write(0, 0x4C);
    bus->Write(1, 0x60);
    bus->Write(2, 0x41);

    // Add SED instruction to byte 0x4160 of memory
    bus->Write(0x4160, 0xF8);

    for (int i = 0; i < 5; i++) { cpu->Clock(); }
    EXPECT_EQ(1, cpu->GetFlag(CPU::Flag::D)) << "D flag incorrectly cleared after an attempted jump to SED";
}

TEST(CpuTests, JMP_Indirect)
{
    auto bus = std::make_shared<TestCpuBus>();
    auto cpu = std::make_shared<CPU>(bus);

    // Add pointer to memory
    bus->Write(0x4160, 0x67);
    bus->Write(0x4161, 0xF3);
    
    // Add jump instruction to program memory
    bus->Write(0, 0x6C);
    bus->Write(1, 0x60);
    bus->Write(2, 0x41);

    // Add SED instruction to byte 0x4160 of memory
    bus->Write(0xF367, 0xF8);

    for (int i = 0; i < 7; i++) { cpu->Clock(); }
    EXPECT_EQ(1, cpu->GetFlag(CPU::Flag::D)) << "D flag incorrectly cleared after an attempted jump to SED";
}

TEST(CpuTests, BRK)
{
    auto bus = std::make_shared<TestCpuBus>();
    auto cpu = std::make_shared<CPU>(bus);

    /* BRK opcode starts out in memory */

    // Set BRK destination
    bus->Write(CPU::IRQ_VECTOR, 0x11);
    bus->Write(CPU::IRQ_VECTOR + 1, 0x41);

    // Add SED instruction to BRK destination
    bus->Write(0x4111, 0xF8);

    for (int i = 0; i < 9; i++) { cpu->Clock(); }
    EXPECT_EQ(1, cpu->GetFlag(CPU::Flag::D)) << "D flag incorrectly cleared after an attempted break to SED";
}

TEST(CpuTests, IRQ_Interrupt)
{
    auto bus = std::make_shared<TestCpuBus>();
    auto cpu = std::make_shared<CPU>(bus);

    // Add increment memory instruction to program memory
    bus->Write(0, 0xE6);
    bus->Write(1, 0xF2);

    // Set IRQ destination
    bus->Write(CPU::IRQ_VECTOR, 0x11);
    bus->Write(CPU::IRQ_VECTOR + 1, 0x41);

    // Add SED instruction to BRK destination
    bus->Write(0x4111, 0xF8);

    for (int i = 0; i < 2; i++) { cpu->Clock(); }
    cpu->Interrupt(CPU::InterruptType::IRQ);
    for (int i = 0; i < 12; i++) { cpu->Clock(); }

    EXPECT_EQ(1, bus->Read(0x00F2)) << "Inital instruction is not completed when an IRQ interrupt occurs";
    EXPECT_EQ(1, cpu->GetFlag(CPU::Flag::D)) << "D flag incorrectly cleared after an attempted IRQ interrupt to SED";
}

TEST(CpuTests, NMI_Interrupt)
{
    auto bus = std::make_shared<TestCpuBus>();
    auto cpu = std::make_shared<CPU>(bus);

    // Add increment memory instruction to program memory
    bus->Write(0, 0xE6);
    bus->Write(1, 0xF2);

    // Set NMI destination
    bus->Write(CPU::NMI_VECTOR, 0x11);
    bus->Write(CPU::NMI_VECTOR + 1, 0x41);

    // Add SED instruction to BRK destination
    bus->Write(0x4111, 0xF8);

    for (int i = 0; i < 2; i++) { cpu->Clock(); }
    cpu->Interrupt(CPU::InterruptType::NMI);
    for (int i = 0; i < 12; i++) { cpu->Clock(); }

    EXPECT_EQ(1, bus->Read(0x00F2)) << "Inital instruction is not completed when an NMI interrupt occurs";
    EXPECT_EQ(1, cpu->GetFlag(CPU::Flag::D)) << "D flag incorrectly cleared after an attempted NMI interrupt to SED";
}

TEST(CpuTests, NMI_Hijack)
{
    auto bus = std::make_shared<TestCpuBus>();
    auto cpu = std::make_shared<CPU>(bus);

    /* BRK opcode starts out in memory */

    // Set BRK destination
    bus->Write(CPU::IRQ_VECTOR, 0x11);
    bus->Write(CPU::IRQ_VECTOR + 1, 0x41);

    // Set NMI destination
    bus->Write(CPU::NMI_VECTOR, 0x11);
    bus->Write(CPU::NMI_VECTOR + 1, 0x81);

    // Add SED instruction to BRK destination
    bus->Write(0x4111, 0xF8);

    // Add SEC instruction to NMI destination
    bus->Write(0x8111, 0x38);

    for (int i = 0; i < 3; i++) { cpu->Clock(); }
    cpu->Interrupt(CPU::InterruptType::NMI);
    for (int i = 0; i < 6; i++) { cpu->Clock(); }

    EXPECT_EQ(0, cpu->GetFlag(CPU::Flag::D)) << "NMI interrupt failed to hijack the BRK vector";
    EXPECT_EQ(1, cpu->GetFlag(CPU::Flag::C)) << "Hijack was unsuccessful performing the instruction at the NMI vector";
    
    // Add CLC instruction to NMI destination
    bus->Write(0x8111, 0x18);

    for (int i = 0; i < 9; i++) { cpu->Clock(); }

    EXPECT_EQ(0, cpu->GetFlag(CPU::Flag::D)) << "The instruction at BRK's destination executed when it shouldn't have";
    EXPECT_EQ(0, cpu->GetFlag(CPU::Flag::C)) << "The CPU failed to perform another NMI interrupt after the first hijack";
}