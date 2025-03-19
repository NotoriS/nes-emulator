#include <memory>
#include <stdexcept>
#include <chrono>
#include <thread>
#include <iostream>
#include <cstdlib>
#include <algorithm>
#include <format>

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

static void ResizeRenderer(SDL_Window* window, SDL_Renderer* renderer, int initialScale)
{
    int window_width, window_height;
    SDL_GetWindowSize(window, &window_width, &window_height);

    float scale_x = (float)window_width / (PPU::DISPLAY_WIDTH * 3);
    float scale_y = (float)window_height / (PPU::DISPLAY_HEIGHT * 3);
    float scale = (scale_x < scale_y) ? scale_x : scale_y;

    SDL_RenderSetLogicalSize(renderer, PPU::DISPLAY_WIDTH * initialScale * scale, PPU::DISPLAY_HEIGHT * initialScale * scale);
}

static void SetFullscreen(SDL_Window* window)
{
    static bool isFullscreen = false;
    isFullscreen = !isFullscreen;

    if (isFullscreen)
        SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN_DESKTOP);
    else
        SDL_SetWindowFullscreen(window, 0);
}

int main(int argc, char* argv[])
{
    SDL_Init(SDL_INIT_EVERYTHING);

    Logger::GetInstance().SetLoggingMode(Logger::LoggingMode::Disabled);
    std::string romPath;

    // Determine max window scale
    SDL_Rect displayBounds;
    SDL_GetDisplayBounds(0, &displayBounds);
    int maxScale = std::min<int>(displayBounds.w / PPU::DISPLAY_WIDTH, displayBounds.h / PPU::DISPLAY_HEIGHT);
    int windowScale = maxScale;

    // Handle all program arguments
    for (int i = 1; i < argc; i++)
    {
        std::string arg = argv[i];

        if (arg == "--filename" || arg == "-f")
        {
            if (i + 1 >= argc) continue;
            i++;
            romPath = argv[i];
            continue;
        }
        else if (arg == "--scale" || arg == "-s")
        {
            if (i + 1 >= argc) continue;
            i++;
            windowScale = std::atoi(argv[i]);
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

    // Check for issues with the initial window scale
    if (windowScale <= 0)
    {
        windowScale = 1;
        Logger::GetInstance().Warn("Window scale must be greater than 0. Setting the window scale to 1.");
    }
    else if (windowScale > maxScale)
    {
        windowScale = maxScale;
        Logger::GetInstance().Warn(std::format("The selected window scale it too large for the current display. Setting the window scale to {}.", maxScale));
    }

    // Initialize SDL components
    SDL_Window* window = nullptr;
    SDL_Renderer* renderer = nullptr;
    SDL_Event event;
    SDL_CreateWindowAndRenderer(
        PPU::DISPLAY_WIDTH * windowScale, 
        PPU::DISPLAY_HEIGHT * windowScale,
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

        constexpr std::chrono::nanoseconds interval(16639267);  // Target loop interval (~60 fps)
        auto next_wakeup = std::chrono::steady_clock::now() + interval;

        while (running)
        {
            while (SDL_PollEvent(&event))
            {
                if (event.type == SDL_QUIT) running = false;
                if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_RESIZED) 
                    ResizeRenderer(window, renderer, windowScale);
                if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_F11) SetFullscreen(window);
                else nes->CheckControllerInput(event);
            }

            nes->DrawFrame(renderer, texture);

            // Wait for the next frame to keep a consistent framerate
            std::this_thread::sleep_until(next_wakeup);
            next_wakeup += interval;
        }
    }
    catch (const std::runtime_error& e)
    {
        Logger::GetInstance().Error(e.what());
    }

    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}