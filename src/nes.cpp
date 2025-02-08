#include "nes.h"

NES::NES(const std::string& romPath) : m_romPath(romPath)
{
    InitializeCartridge();
    InitializePPU();
    InitializeCPU();
}

NES::~NES()
{
}

void NES::DrawFrame(SDL_Renderer* renderer, SDL_Texture* texture)
{
    while (!m_ppu->FrameIsComplete())
    {
        bool nmiInterruptRaised = false;
        for (int i = 0; i < 3; i++)
        {
            m_ppu->Clock();
            nmiInterruptRaised |= m_ppu->NmiInterruptWasRaised();
        }

        if (nmiInterruptRaised)
            m_cpu->Interrupt(CPU::InterruptType::NMI);

        m_cpu->Clock();
    }

    uint32_t* pixelBuffer = m_ppu->GetPixelBuffer();

    // Update the texture with the current frame
    SDL_UpdateTexture(texture, nullptr, pixelBuffer, PPU::DISPLAY_WIDTH * sizeof(uint32_t));

    // Clear the screen and render the texture
    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, texture, nullptr, nullptr);
    SDL_RenderPresent(renderer);
}

void NES::InitializeCartridge()
{
    m_cartridge = std::make_shared<Cartridge>();
    m_cartridge->LoadROM(m_romPath);
}

void NES::InitializePPU()
{
    m_ppuBus = std::make_shared<PpuBus>();
    m_ppuBus->ConnectCartridge(m_cartridge);
    m_ppu = std::make_shared<PPU>(m_ppuBus);
}

void NES::InitializeCPU()
{
    m_cpuBus = std::make_shared<CpuBus>();
    m_cpuBus->ConnectCartridge(m_cartridge);
    m_cpuBus->ConnectPPU(m_ppu);
    m_cpu = std::make_unique<CPU>(m_cpuBus);
}
