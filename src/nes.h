#include <string>
#include <memory>
#include <chrono>
#include <thread>

#include <SDL2/SDL.h>

#include "debug/logger.h"
#include "cpu/cpu.h"
#include "cpu/cpu-bus.h"
#include "cartridge/cartridge.h"

class NES
{
public:
    NES(const std::string& romPath);
    ~NES();

    void DrawFrame(SDL_Renderer* renderer, SDL_Texture* texture);

private:
    std::string m_romPath;

    std::shared_ptr<Cartridge> m_cartridge;
    std::shared_ptr<PPU> m_ppu;
    std::shared_ptr<PpuBus> m_ppuBus;
    std::unique_ptr<CPU> m_cpu;
    std::shared_ptr<CpuBus> m_cpuBus;

    void InitializeCartridge();
    void InitializePPU();
    void InitializeCPU();
};