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