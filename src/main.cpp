#include <iostream>

#include "nes-emulator.h"

int main(int argc, char* argv[])
{
    if (argc < 2)
    {
        std::cerr << "No filename provided" << std::endl;
        return -1;
    }

    NesEmulator emulator(argv[1]);
    emulator.Run();

    return 0;
}