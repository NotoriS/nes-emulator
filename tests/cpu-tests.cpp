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