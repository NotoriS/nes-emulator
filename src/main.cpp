#include <iostream>
#include <memory>
#include <stdexcept>

#include "cpu/cpu.h"
#include "cpu/cpu-bus.h"
#include "cartridge/cartridge.h"

int main(int argc, char* argv[])
{
    // Fetch the ROM filepath from the program arguments
    if (argc < 2)
    {
        std::cerr << "No filename provided" << std::endl;
        return -1;
    }
    const std::string romPath = argv[1];

    // Initalize the game cartridge
    auto cartridge = std::make_shared<Cartridge>();
    try
    {
        cartridge->LoadROM(romPath);
    }
    catch (const std::runtime_error& e)
    {
        std::cerr << "Fatal Error (" << e.what() << ")" << std::endl;
        return -1;
    }

    // Initalize the CPU and main bus
    auto cpuBus = std::make_shared<CpuBus>();
    cpuBus->ConnectCartridge(cartridge);
    auto cpu = std::make_unique<CPU>(cpuBus);

    while (true)
    {
        // TODO: Modify to only clock at the specified clock rate.
        cpu->Clock();
    }

    return 0;
}