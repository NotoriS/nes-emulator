#include <memory>
#include <stdexcept>
#include <chrono>
#include <thread>
#include <iostream>

#include <SDL2/SDL.h>

#include "debug/logger.h"
#include "nes.h"

#ifdef _WIN32
#define popen _popen
#define pclose _pclose
#endif

static std::string OpenFileDialog()
{
    std::string command;
    std::string filename;

#ifdef _WIN32  // Windows: Use PowerShell
    command = "powershell -Command \"Add-Type -AssemblyName System.Windows.Forms; "
        "$f = New-Object System.Windows.Forms.OpenFileDialog; "
        "$f.Filter = 'All Files (*.*)|*.*'; "
        "If ($f.ShowDialog() -eq 'OK') { $f.FileName }\"";
#elif __APPLE__  // macOS: Use AppleScript
    command = "osascript -e 'set filename to choose file' -e 'POSIX path of filename'";
#elif __linux__  // Linux: Use Zenity
    command = "zenity --file-selection";
#endif

    FILE* pipe = popen(command.c_str(), "r");
    if (!pipe) {
        std::cerr << "Error opening file dialog." << std::endl;
        return "";
    }

    char buffer[1024];
    if (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
        filename = buffer;
        filename.erase(filename.find_last_not_of(" \n\r\t") + 1); // Trim newline
    }
    pclose(pipe);

    return filename;
}

int main(int argc, char* argv[])
{
    Logger::GetInstance().SetLoggingMode(Logger::LoggingMode::Disabled);
    std::string romPath;

    // Handle all program arguments
    for (int i = 1; i < argc; i++)
    {
        std::string arg = argv[i];

        if (arg == "--filename" || arg == "-f")
        {
            if (i + 1 >= argc)
            {
                std::cout << "Error: no filename provided." << std::endl;
                return -1;
            }
            i++;
            romPath = argv[i];
            continue;
        }
        else if (arg == "--console-logging")
        {
            Logger::GetInstance().SetLoggingMode(Logger::LoggingMode::Console);
            continue;
        }
    }

    if (romPath.empty()) romPath = OpenFileDialog();

    // Check if a filename was provided
    if (romPath.empty())
    {
        Logger::GetInstance().Error("no filename provided.");
        return -1;
    }

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