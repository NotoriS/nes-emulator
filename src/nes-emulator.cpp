﻿#include <stdexcept>
#include <iostream>

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
    CPU cpu;
    CpuBus cpuBus;
    Cartridge cartridge;

    cpu.ConnectBus(&cpuBus);
    cpuBus.ConnectCartridge(&cartridge);

    try
    {
        cartridge.LoadROM(m_romFilename);
    }
    catch (const std::runtime_error& e)
    {
        std::cerr << "Fatal Error (" << e.what() << ")" << std::endl;
        return;
    }

    while (true)
    {
        // TODO: Modify to only clock at the specified clock rate.
        cpu.Clock();
    }
}
