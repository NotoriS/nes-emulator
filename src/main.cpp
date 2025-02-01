#include <memory>
#include <stdexcept>
#include <chrono>
#include <thread>
#include <unordered_set>

#include <SDL2/SDL.h>

#include "debug/logger.h"
#include "cpu/cpu.h"
#include "cpu/cpu-bus.h"
#include "cartridge/cartridge.h"

int main(int argc, char* argv[])
{
    std::unordered_set<std::string> flagSet;
    std::string romPath;

    // Sort all program arguments
    for (int i = 1; i < argc; i++)
    {
        std::string arg = argv[i];

        if (arg.compare(0, 2, "--") == 0)
        {
            flagSet.insert(arg);
        }
        else
        {
            if (!romPath.empty())
            {
                Logger::GetInstance().Error("more than one filename provided.");
                return -1;
            }
            romPath = arg;
        }
    }

    if (romPath.empty())
    {
        Logger::GetInstance().Error("no filename provided.");
        return -1;
    }

    if (flagSet.count("--console-logging"))
        Logger::GetInstance().SetLoggingMode(Logger::LoggingModes::Console);
    else
        Logger::GetInstance().SetLoggingMode(Logger::LoggingModes::Disabled);

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
        const auto frameStart = std::chrono::high_resolution_clock().now();

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

        // Wait for the next frame to keep a consistent framerate
        const auto frameEnd = std::chrono::high_resolution_clock().now();
        const auto frameDuration = frameEnd - frameStart;
        const auto targetFrameDuration = std::chrono::duration<double, std::milli>(16.67);
        if (frameDuration < targetFrameDuration)
            std::this_thread::sleep_for(targetFrameDuration - frameDuration);
    }

    return 0;
}