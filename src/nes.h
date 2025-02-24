#pragma once

#include <string>
#include <memory>
#include <cstdint>

#include <SDL2/SDL.h>

#include "debug/logger.h"
#include "cpu/cpu.h"
#include "cpu/cpu-bus.h"
#include "cartridge/cartridge.h"
#include "audio/apu.h"

class NES
{
    enum class ControllerButton : uint8_t
    {
        Right = 0x01,
        Left = 0x02,
        Down = 0x04,
        Up = 0x08,
        Start = 0x10,
        Select = 0x20,
        B = 0x40,
        A = 0x80
    };

public:
    NES(const std::string& romPath);
    ~NES();

    void DrawFrame(SDL_Renderer* renderer, SDL_Texture* texture);
    void CheckControllerInput(const SDL_Event& event);

private:
    std::string m_romPath;

    std::shared_ptr<Cartridge> m_cartridge;
    std::shared_ptr<PPU> m_ppu;
    std::shared_ptr<PpuBus> m_ppuBus;
    std::shared_ptr<APU> m_apu;
    std::unique_ptr<CPU> m_cpu;
    std::shared_ptr<CpuBus> m_cpuBus;

    std::shared_ptr<uint8_t> m_controllerOneState;
    std::shared_ptr<uint8_t> m_controllerTwoState;

    bool m_oddCpuCycle = false;

    void InitializeCartridge();
    void InitializePPU();
    void InitializeAPU();
    void InitializeCPU();

    void SetControllerButtonState(uint8_t controllerNumber, ControllerButton button, bool newState) const;
};