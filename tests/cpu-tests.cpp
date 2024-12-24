#include <gtest/gtest.h>

#include "../src/cpu/cpu.h"
#include "fakes/fake-cpu-bus.h"

TEST(CpuTests, ZeroPage_ASL)
{
    TestCpuBus bus;
    CPU cpu(&bus);

    // Write test data
    bus.Write(0x00F6, 2);

    // Add instruction to program memory
    bus.Write(0, 0x06);

    // Add target address to program memory
    bus.Write(1, 0xF6);

    for (int i = 0; i < 5; i++) { cpu.Clock(); }

    EXPECT_EQ(4, bus.Read(0x00F6));
}

TEST(CpuTests, Absolute_ASL)
{
    TestCpuBus bus;
    CPU cpu(&bus);

    // Write test data
    bus.Write(0x0EF6, 2);

    // Add instruction to program memory
    bus.Write(0, 0x0E);

    // Add target address to program memory
    bus.Write(1, 0xF6);
    bus.Write(2, 0x0E);

    for (int i = 0; i < 6; i++) { cpu.Clock(); }

    EXPECT_EQ(4, bus.Read(0x0EF6));
}

TEST(CpuTests, ZeroPage_LSR)
{
    TestCpuBus bus;
    CPU cpu(&bus);

    // Write test data
    bus.Write(0x0011, 9);

    // Add LSR instruction to program memory
    bus.Write(0, 0x46);
    bus.Write(1, 0x11);

    for (int i = 0; i < 5; i++) { cpu.Clock(); }

    EXPECT_EQ(4, bus.Read(0x0011)) << "Value should have been 4 after performing LSR on 9";
    EXPECT_EQ(1, cpu.GetFlag(CPU::Flag::C)) << "Carry flag was cleared after performing LSR on 9";
    EXPECT_EQ(0, cpu.GetFlag(CPU::Flag::Z)) << "Zero flag was set after performing LSR on 9";
    EXPECT_EQ(0, cpu.GetFlag(CPU::Flag::N)) << "Negative flag was set after performing LSR on 9";
}

TEST(CpuTests, Absolute_ROL)
{
    TestCpuBus bus;
    CPU cpu(&bus);

    // Write test data
    bus.Write(0x0045, 0b01101010);

    for (int i = 0; i < 9; i++)
    {
        // Add instruction to program memory
        bus.Write(i * 2, 0x26);

        // Add target address to program memory
        bus.Write((i * 2) + 1, 0x45);
    }

    for (int i = 0; i < 45; i++) { cpu.Clock(); }

    EXPECT_EQ(0b01101010, bus.Read(0x0045));
}

TEST(CpuTests, Absolute_LDA_STA)
{
    TestCpuBus bus;
    CPU cpu(&bus);

    // Write test data
    bus.Write(0x1EF6, 5);

    // Add load instruction and address to memory
    bus.Write(0, 0xAD);
    bus.Write(1, 0xF6);
    bus.Write(2, 0x1E);

    // Add store instruction and address to memory
    bus.Write(3, 0x8D);
    bus.Write(4, 0x45);
    bus.Write(5, 0x26);

    for (int i = 0; i < 8; i++) { cpu.Clock(); }

    EXPECT_EQ(5, bus.Read(0x2645));
}

TEST(CpuTests, Absolute_LDX_STX)
{
    TestCpuBus bus;
    CPU cpu(&bus);

    // Write test data
    bus.Write(0x1EF6, 5);

    // Add load instruction and address to memory
    bus.Write(0, 0xAE);
    bus.Write(1, 0xF6);
    bus.Write(2, 0x1E);

    // Add store instruction and address to memory
    bus.Write(3, 0x8E);
    bus.Write(4, 0x45);
    bus.Write(5, 0x26);

    for (int i = 0; i < 8; i++) { cpu.Clock(); }

    EXPECT_EQ(5, bus.Read(0x2645));
}

TEST(CpuTests, Absolute_LDY_STY)
{
    TestCpuBus bus;
    CPU cpu(&bus);

    // Write test data
    bus.Write(0x1EF6, 5);

    // Add load instruction and address to memory
    bus.Write(0, 0xAC);
    bus.Write(1, 0xF6);
    bus.Write(2, 0x1E);

    // Add store instruction and address to memory
    bus.Write(3, 0x8C);
    bus.Write(4, 0x45);
    bus.Write(5, 0x26);

    for (int i = 0; i < 8; i++) { cpu.Clock(); }

    EXPECT_EQ(5, bus.Read(0x2645));
}

TEST(CpuTests, TAX)
{
    TestCpuBus bus;
    CPU cpu(&bus);

    // Add load A instruction and address to memory
    bus.Write(0, 0xA9);
    bus.Write(1, 5);

    // Add transfer instruction to memory
    bus.Write(2, 0xAA);

    // Add store X instruction and address to memory
    bus.Write(3, 0x8E);
    bus.Write(4, 0x45);
    bus.Write(5, 0x26);

    for (int i = 0; i < 8; i++) { cpu.Clock(); }

    EXPECT_EQ(5, bus.Read(0x2645));
}

TEST(CpuTests, TAY)
{
    TestCpuBus bus;
    CPU cpu(&bus);

    // Add load A instruction and address to memory
    bus.Write(0, 0xA9);
    bus.Write(1, 5);

    // Add transfer instruction to memory
    bus.Write(2, 0xA8);

    // Add store Y instruction and address to memory
    bus.Write(3, 0x8C);
    bus.Write(4, 0x45);
    bus.Write(5, 0x26);

    for (int i = 0; i < 8; i++) { cpu.Clock(); }

    EXPECT_EQ(5, bus.Read(0x2645));
}

TEST(CpuTests, TSX)
{
    TestCpuBus bus;
    CPU cpu(&bus);

    // Add transfer instruction to memory
    bus.Write(0, 0xBA);

    // Add store X instruction and address to memory
    bus.Write(1, 0x8E);
    bus.Write(2, 0x45);
    bus.Write(3, 0x26);

    for (int i = 0; i < 6; i++) { cpu.Clock(); }

    EXPECT_EQ(0xFD, bus.Read(0x2645));
}

TEST(CpuTests, TYA)
{
    TestCpuBus bus;
    CPU cpu(&bus);

    // Add load Y instruction and address to memory
    bus.Write(0, 0xA0);
    bus.Write(1, 5);

    // Add transfer instruction to memory
    bus.Write(2, 0x98);

    // Add store A instruction and address to memory
    bus.Write(3, 0x8D);
    bus.Write(4, 0x45);
    bus.Write(5, 0x26);

    for (int i = 0; i < 8; i++) { cpu.Clock(); }

    EXPECT_EQ(5, bus.Read(0x2645));
}

TEST(CpuTests, TXA)
{
    TestCpuBus bus;
    CPU cpu(&bus);

    // Add load X instruction and address to memory
    bus.Write(0, 0xA2);
    bus.Write(1, 5);

    // Add transfer instruction to memory
    bus.Write(2, 0x8A);

    // Add store A instruction and address to memory
    bus.Write(3, 0x8D);
    bus.Write(4, 0x45);
    bus.Write(5, 0x26);

    for (int i = 0; i < 8; i++) { cpu.Clock(); }

    EXPECT_EQ(5, bus.Read(0x2645));
}

TEST(CpuTests, TXS_TSX)
{
    TestCpuBus bus;
    CPU cpu(&bus);

    // Add load X instruction to memory
    bus.Write(0, 0xA2);
    bus.Write(1, 5);

    // Add TXS instruction to memory
    bus.Write(2, 0x9A);

    // Set X to something else
    bus.Write(3, 0xA2);
    bus.Write(4, 7);

    // Add store X instruction and address to memory
    bus.Write(5, 0x8E);
    bus.Write(6, 0x45);
    bus.Write(7, 0x26);

    // Check that X was set properly
    for (int i = 0; i < 10; i++) { cpu.Clock(); }
    EXPECT_EQ(7, bus.Read(0x2645));

    // Add TSX instruction to memory
    bus.Write(8, 0xBA);

    // Add store X instruction and address to memory
    bus.Write(9, 0x8E);
    bus.Write(10, 0x45);
    bus.Write(11, 0x26);

    // Check that X was changed to the value of SP
    for (int i = 0; i < 6; i++) { cpu.Clock(); }
    EXPECT_EQ(5, bus.Read(0x2645));
}

TEST(CpuTests, Immediate_ADC)
{
    TestCpuBus bus;
    CPU cpu(&bus);

    // Add ADC instruction to program memory
    bus.Write(0, 0x69);
    bus.Write(1, 130);

    // Add store A instruction to program memory
    bus.Write(2, 0x8D);
    bus.Write(3, 0x44);
    bus.Write(4, 0x44);

    for (int i = 0; i < 6; i++) { cpu.Clock(); }

    // Check state after subtraction with overflow and carry cleared
    EXPECT_EQ(0, cpu.GetFlag(CPU::Flag::C)) << "Carry flag is set after normal ADC";
    EXPECT_EQ(130, bus.Read(0x4444)) << "ADC did not add properly with the C flag cleared";

    // Add ADC instruction to program memory
    bus.Write(5, 0x69);
    bus.Write(6, 130);

    // Add store A instruction to program memory
    bus.Write(7, 0x8D);
    bus.Write(8, 0x55);
    bus.Write(9, 0x55);

    for (int i = 0; i < 6; i++) { cpu.Clock(); }

    // Check state after add with overflow and carry cleared
    EXPECT_EQ(1, cpu.GetFlag(CPU::Flag::C)) << "Carry flag is cleared after ADC with overflow";
    EXPECT_EQ(4, bus.Read(0x5555)) << "ADC did not overflow properly with the C flag cleared";

    // Add ADC instruction to program memory
    bus.Write(10, 0x69);
    bus.Write(11, 130);

    // Add store A instruction to program memory
    bus.Write(12, 0x8D);
    bus.Write(13, 0x44);
    bus.Write(14, 0x44);

    for (int i = 0; i < 6; i++) { cpu.Clock(); }

    // Check state after subtraction with overflow and carry cleared
    EXPECT_EQ(0, cpu.GetFlag(CPU::Flag::C)) << "Carry flag is set after normal ADC";
    EXPECT_EQ(135, bus.Read(0x4444)) << "ADC did not add properly with the C flag set";
}

TEST(CpuTests, Immediate_SBC)
{
    TestCpuBus bus;
    CPU cpu(&bus);

    // Add SBC instruction to program memory
    bus.Write(0, 0xE9);
    bus.Write(1, 1);

    // Add store A instruction to program memory
    bus.Write(2, 0x8D);
    bus.Write(3, 0x44);
    bus.Write(4, 0x44);

    for (int i = 0; i < 6; i++) { cpu.Clock(); }

    // Check state after subtraction with overflow and carry cleared
    EXPECT_EQ(0, cpu.GetFlag(CPU::Flag::C)) << "Carry flag is set after SBC underflows";
    EXPECT_EQ(254, bus.Read(0x4444)) << "SBC did not subtract properly with the C flag cleared";

    // Add SBC instruction to program memory
    bus.Write(5, 0xE9);
    bus.Write(6, 14);

    // Add store A instruction to program memory
    bus.Write(7, 0x8D);
    bus.Write(8, 0x55);
    bus.Write(9, 0x55);

    for (int i = 0; i < 6; i++) { cpu.Clock(); }

    // Check state after subtraction with no overflow and carry cleared
    EXPECT_EQ(1, cpu.GetFlag(CPU::Flag::C)) << "Carry flag is cleared after normal SBC";
    EXPECT_EQ(239, bus.Read(0x5555)) << "SBC did not underflow properly with the C flag cleared";

    // Add SBC instruction to program memory
    bus.Write(10, 0xE9);
    bus.Write(11, 9);

    // Add store A instruction to program memory
    bus.Write(12, 0x8D);
    bus.Write(13, 0x66);
    bus.Write(14, 0x66);

    for (int i = 0; i < 6; i++) { cpu.Clock(); }

    // Check state after subtraction with no overflow and carry set
    EXPECT_EQ(1, cpu.GetFlag(CPU::Flag::C)) << "Carry flag is cleared after normal SBC";
    EXPECT_EQ(230, bus.Read(0x6666)) << "SBC did not subtract properly with the C flag set";
}

TEST(CpuTests, ZeroPage_INC)
{
    TestCpuBus bus;
    CPU cpu(&bus);

    // Write value to increment
    bus.Write(0x0047, 0xFF);

    // Add INC instruction to program memory
    bus.Write(0, 0xE6);
    bus.Write(1, 0x47);

    for (int i = 0; i < 5; i++) { cpu.Clock(); }

    // Check state after overflowing INC
    EXPECT_EQ(0, bus.Read(0x0047)) << "Memory value does not equal zero after incrementing 0xFF";
    EXPECT_EQ(1, cpu.GetFlag(CPU::Flag::Z)) << "Zero flag is not set when INC results in 0";
}

TEST(CpuTests, ZeroPage_DEC)
{
    TestCpuBus bus;
    CPU cpu(&bus);

    // Add DEC instruction to program memory
    bus.Write(0, 0xC6);
    bus.Write(1, 0x47);

    for (int i = 0; i < 5; i++) { cpu.Clock(); }

    // Check state after overflowing INC
    EXPECT_EQ(0xFF, bus.Read(0x0047)) << "Memory value does not equal zero after incrementing 0xFF";
    EXPECT_EQ(0, cpu.GetFlag(CPU::Flag::Z)) << "Zero flag is set when DEC results in 0xFF";
    EXPECT_EQ(1, cpu.GetFlag(CPU::Flag::N)) << "Negative flag is not set when DEC results in 0xFF";
}

TEST(CpuTests, INX)
{
    TestCpuBus bus;
    CPU cpu(&bus);

    // Add INX to program memory
    bus.Write(0, 0xE8);

    // Add store X to program memory
    bus.Write(1, 0x86);
    bus.Write(2, 0x11);

    for (int i = 0; i < 5; i++) { cpu.Clock(); }

    // Check state after INX
    EXPECT_EQ(0x01, bus.Read(0x0011)) << "Memory value does not equal 0x01 after incrementing X = 0x00";
    EXPECT_EQ(0, cpu.GetFlag(CPU::Flag::Z)) << "Zero flag is set when INX results in 0x01";
    EXPECT_EQ(0, cpu.GetFlag(CPU::Flag::N)) << "Negative flag is set when INX results in 0x01";
}

TEST(CpuTests, DEX)
{
    TestCpuBus bus;
    CPU cpu(&bus);

    // Add DEX to program memory
    bus.Write(0, 0xCA);

    // Add store X to program memory
    bus.Write(1, 0x86);
    bus.Write(2, 0x11);

    for (int i = 0; i < 5; i++) { cpu.Clock(); }

    // Check state after DEX
    EXPECT_EQ(0xFF, bus.Read(0x0011)) << "Memory value does not equal 0xFF after decrementing X = 0x00";
    EXPECT_EQ(0, cpu.GetFlag(CPU::Flag::Z)) << "Zero flag is set when DEX results in 0xFF";
    EXPECT_EQ(1, cpu.GetFlag(CPU::Flag::N)) << "Negative flag is not set when DEX results in 0xFF";
}

TEST(CpuTests, INY)
{
    TestCpuBus bus;
    CPU cpu(&bus);

    // Add INY to program memory
    bus.Write(0, 0xC8);

    // Add store Y to program memory
    bus.Write(1, 0x84);
    bus.Write(2, 0x11);

    for (int i = 0; i < 5; i++) { cpu.Clock(); }

    // Check state after INY
    EXPECT_EQ(0x01, bus.Read(0x0011)) << "Memory value does not equal 0x01 after incrementing Y = 0x00";
    EXPECT_EQ(0, cpu.GetFlag(CPU::Flag::Z)) << "Zero flag is set when INY results in 0x01";
    EXPECT_EQ(0, cpu.GetFlag(CPU::Flag::N)) << "Negative flag is set when INY results in 0x01";
}

TEST(CpuTests, DEY)
{
    TestCpuBus bus;
    CPU cpu(&bus);

    // Add DEY to program memory
    bus.Write(0, 0x88);

    // Add store Y to program memory
    bus.Write(1, 0x84);
    bus.Write(2, 0x11);

    for (int i = 0; i < 5; i++) { cpu.Clock(); }

    // Check state after DEY
    EXPECT_EQ(0xFF, bus.Read(0x0011)) << "Memory value does not equal 0xFF after decrementing Y = 0x00";
    EXPECT_EQ(0, cpu.GetFlag(CPU::Flag::Z)) << "Zero flag is set when DEY results in 0xFF";
    EXPECT_EQ(1, cpu.GetFlag(CPU::Flag::N)) << "Negative flag is not set when DEY results in 0xFF";
}

TEST(CpuTests, Immediate_AND)
{
    TestCpuBus bus;
    CPU cpu(&bus);

    // Add load A to program memory
    bus.Write(0, 0xA9);
    bus.Write(1, 0b00101100);

    // Add AND to program memory
    bus.Write(2, 0x29);
    bus.Write(3, 0b00101011);

    // Add store A instruction to program memory
    bus.Write(4, 0x85);
    bus.Write(5, 0x11);

    for (int i = 0; i < 7; i++) { cpu.Clock(); }

    EXPECT_EQ(0b00101000, bus.Read(0x0011)) << "AND results in the wrong value in the accumulator";
    EXPECT_EQ(0, cpu.GetFlag(CPU::Flag::Z)) << "Z flag incorrectly set after AND";
    EXPECT_EQ(0, cpu.GetFlag(CPU::Flag::N)) << "N flag incorrectly set after AND";
}