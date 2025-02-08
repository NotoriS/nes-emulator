#include "nes.h"

NES::NES(const std::string& romPath) : m_romPath(romPath)
{
    m_controllerOneState = std::make_shared<uint8_t>(0);
    m_controllerTwoState = std::make_shared<uint8_t>(0);

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

    // Reset controller state
    *m_controllerOneState = 0;
    *m_controllerTwoState = 0;
}

void NES::CheckControllerInput(const SDL_Event& event)
{
    if (event.type != SDL_EventType::SDL_KEYDOWN) return;

    auto key = event.key.keysym.sym;
    switch (key)
    {
    // Controller 1
    case SDLK_l: // A Button
        *m_controllerOneState |= 0x80;
        break;
    case SDLK_k: // B Button
        *m_controllerOneState |= 0x40;
        break;
    case SDLK_g: // Select Button
        *m_controllerOneState |= 0x20;
        break;
    case SDLK_h: // Start Button
        *m_controllerOneState |= 0x10;
        break;
    case SDLK_w: // D-Pad Up
        *m_controllerOneState |= 0x08;
        break;
    case SDLK_s: // D-Pad Down
        *m_controllerOneState |= 0x04;
        break;
    case SDLK_a: // D-Pad Left
        *m_controllerOneState |= 0x02;
        break;
    case SDLK_d: // D-Pad Right
        *m_controllerOneState |= 0x01;
        break;

    // Controller 2
    case SDLK_KP_3: // A Button
        *m_controllerTwoState |= 0x80;
        break;
    case SDLK_KP_2: // B Button
        *m_controllerTwoState |= 0x40;
        break;
    case SDLK_KP_4: // Select Button
        *m_controllerTwoState |= 0x20;
        break;
    case SDLK_KP_5: // Start Button
        *m_controllerTwoState |= 0x10;
        break;
    case SDLK_UP: // D-Pad Up
        *m_controllerTwoState |= 0x08;
        break;
    case SDLK_DOWN: // D-Pad Down
        *m_controllerTwoState |= 0x04;
        break;
    case SDLK_LEFT: // D-Pad Left
        *m_controllerTwoState |= 0x02;
        break;
    case SDLK_RIGHT: // D-Pad Right
        *m_controllerTwoState |= 0x01;
        break;
    }
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
    m_cpuBus->ConnectControllers(m_controllerOneState, m_controllerTwoState);
    m_cpuBus->ConnectCartridge(m_cartridge);
    m_cpuBus->ConnectPPU(m_ppu);
    m_cpu = std::make_unique<CPU>(m_cpuBus);
}
