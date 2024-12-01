#include "nes-emulator.h"
#include "cpu/cpu.h"
#include "cpu/cpu-bus.h"

NesEmulator::NesEmulator() {}

NesEmulator::~NesEmulator() {}

void NesEmulator::Run()
{
    CPU cpu = CPU();
    CpuBus cpuBus = CpuBus();

    cpu.ConnectBus(&cpuBus);

    while (true)
    {
        // TODO: Modify to only clock at the specified clock rate.
        cpu.Clock();
    }
}
