#include <stdexcept>
#include <iostream>
#include <memory>

#include "nes-emulator.h"
#include "cpu/cpu.h"
#include "cpu/cpu-bus.h"
#include "cartridge/cartridge.h"

NesEmulator::NesEmulator(std::string filename) 
{
    m_romFilename = filename;
}

NesEmulator::~NesEmulator() {}

void NesEmulator::Run()
{
    auto cartridge = std::make_shared<Cartridge>();
    try
    {
        cartridge->LoadROM(m_romFilename);
    }
    catch (const std::runtime_error& e)
    {
        std::cerr << "Fatal Error (" << e.what() << ")" << std::endl;
        return;
    }

    auto cpuBus = std::make_shared<CpuBus>();
    cpuBus->ConnectCartridge(cartridge);

    auto cpu = std::make_unique<CPU>(cpuBus);

    while (true)
    {
        // TODO: Modify to only clock at the specified clock rate.
        cpu->Clock();
    }
}
