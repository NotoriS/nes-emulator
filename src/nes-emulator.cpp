#include "nes-emulator.h"
#include "cpu/cpu.h"

NesEmulator::NesEmulator() {}

NesEmulator::~NesEmulator() {}

void NesEmulator::Run()
{
    CPU cpu = CPU();

    while (true)
    {
        // TODO: Modify to only clock at the specified clock rate.
        cpu.Clock();
    }
}
