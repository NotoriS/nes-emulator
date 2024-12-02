#pragma once

#include <string>

class NesEmulator
{
public:
    NesEmulator(std::string filename);
    ~NesEmulator();

    void Run();

private:
    std::string m_romFilename;
};