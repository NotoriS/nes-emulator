#include <iostream>
#include <memory>
#include <stdexcept>
#include <SDL2/SDL.h>

#include "debug/logger.h"
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
        Logger::GetInstance().Error(e.what());
        return -1;
    }

    //Initialize the PPU and PPU bus
    auto ppuBus = std::make_shared<PpuBus>();
    ppuBus->ConnectCartridge(cartridge);
    auto ppu = std::make_shared<PPU>(ppuBus);

    // Initalize the CPU and main bus
    auto cpuBus = std::make_shared<CpuBus>();
    cpuBus->ConnectCartridge(cartridge);
    cpuBus->ConnectPPU(ppu);
    auto cpu = std::make_unique<CPU>(cpuBus);

    // Initialize SDL components
    SDL_Window* window = nullptr;
    SDL_Renderer* renderer = nullptr;
    SDL_Event event;
    SDL_Init(SDL_INIT_EVERYTHING);
    SDL_CreateWindowAndRenderer(
        PPU::DISPLAY_WIDTH*3, PPU::DISPLAY_HEIGHT*3, 
        SDL_WINDOW_RESIZABLE, 
        &window, &renderer
    );
    SDL_SetWindowTitle(window, "NES Emulator");

    // Create an SDL texture to render the frames
    SDL_Texture* texture = SDL_CreateTexture(
        renderer,
        SDL_PIXELFORMAT_RGBA8888,
        SDL_TEXTUREACCESS_STREAMING,
        256, 240
    );

    while (true)
    {
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT) return 0;
        }

        while (!ppu->FrameIsComplete())
        {
            bool nmiInterruptRaised = false;
            for (int i = 0; i < 3; i++)
            {
                ppu->Clock();
                nmiInterruptRaised |= ppu->NmiInterruptWasRaised();
            }

            if (nmiInterruptRaised) 
                cpu->Interrupt(CPU::InterruptType::NMI);

            cpu->Clock();
        }

        uint32_t* pixelBuffer = ppu->GetPixelBuffer();

        // Update the texture with the current frame
        SDL_UpdateTexture(texture, nullptr, pixelBuffer, PPU::DISPLAY_WIDTH * sizeof(uint32_t));

        // Clear the screen and render the texture
        SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer, texture, nullptr, nullptr);
        SDL_RenderPresent(renderer);

        // TODO: Replace with a more robust timing restriction
        SDL_Delay(16);
    }

    return 0;
}