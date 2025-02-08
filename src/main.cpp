#include <memory>
#include <stdexcept>
#include <chrono>
#include <thread>
#include <unordered_set>

#include <SDL2/SDL.h>

#include "debug/logger.h"
#include "nes.h"

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

    // Check if a filename was provided
    if (romPath.empty())
    {
        Logger::GetInstance().Error("no filename provided.");
        return -1;
    }

    // Configure logging mode
    if (flagSet.count("--console-logging"))
        Logger::GetInstance().SetLoggingMode(Logger::LoggingModes::Console);
    else
        Logger::GetInstance().SetLoggingMode(Logger::LoggingModes::Disabled);

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

    // Run the emulation
    try
    {
        auto nes = std::make_unique<NES>(romPath);
        bool running = true;

        while (running)
        {
            const auto frameStart = std::chrono::high_resolution_clock().now();

            while (SDL_PollEvent(&event))
            {
                if (event.type == SDL_QUIT) running = false;
                else nes->CheckControllerInput(event);
            }

            nes->DrawFrame(renderer, texture);

            // Wait for the next frame to keep a consistent framerate
            const auto frameEnd = std::chrono::high_resolution_clock().now();
            const auto frameDuration = frameEnd - frameStart;
            const auto targetFrameDuration = std::chrono::duration<double, std::milli>(16.67);
            if (frameDuration < targetFrameDuration)
                std::this_thread::sleep_for(targetFrameDuration - frameDuration);
        }
    }
    catch (const std::runtime_error& e)
    {
        Logger::GetInstance().Error(e.what());
        return -1;
    }

    return 0;
}