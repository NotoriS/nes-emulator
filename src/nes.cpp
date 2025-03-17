#include "nes.h"

NES::NES(const std::string& romPath) : m_romPath(romPath)
{
    m_controllerOneState = std::make_shared<uint8_t>(0);
    m_controllerTwoState = std::make_shared<uint8_t>(0);

    InitializeCartridge();
    InitializePPU();
    InitializeAPU();
    InitializeCPU();
}

NES::~NES()
{
    SDL_CloseAudio();
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

        m_oddCpuCycle = !m_oddCpuCycle;
        if (!m_cpuBus->TryDirectMemoryAccess(m_oddCpuCycle))
            m_cpu->Clock();

        m_apu->Clock();
    }

    uint32_t* pixelBuffer = m_ppu->GetPixelBuffer();

    // Update the texture with the current frame
    SDL_UpdateTexture(texture, nullptr, pixelBuffer, PPU::DISPLAY_WIDTH * sizeof(uint32_t));

    // Clear the screen and render the texture
    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, texture, nullptr, nullptr);
    SDL_RenderPresent(renderer);
}

void NES::CheckControllerInput(const SDL_Event& event)
{
    if (event.type != SDL_EventType::SDL_KEYDOWN
        && event.type != SDL_EventType::SDL_KEYUP)
        return;

    auto key = event.key.keysym.sym;
    bool keyPressed = event.type == SDL_EventType::SDL_KEYDOWN;

    switch (key)
    {
    // Controller 1
    case SDLK_l:
        SetControllerButtonState(1, ControllerButton::A, keyPressed);
        break;
    case SDLK_k:
        SetControllerButtonState(1, ControllerButton::B, keyPressed);
        break;
    case SDLK_g:
        SetControllerButtonState(1, ControllerButton::Select, keyPressed);
        break;
    case SDLK_h:
        SetControllerButtonState(1, ControllerButton::Start, keyPressed);
        break;
    case SDLK_w:
        SetControllerButtonState(1, ControllerButton::Up, keyPressed);
        break;
    case SDLK_s:
        SetControllerButtonState(1, ControllerButton::Down, keyPressed);
        break;
    case SDLK_a:
        SetControllerButtonState(1, ControllerButton::Left, keyPressed);
        break;
    case SDLK_d:
        SetControllerButtonState(1, ControllerButton::Right, keyPressed);
        break;

    // Controller 2
    case SDLK_KP_3:
        SetControllerButtonState(2, ControllerButton::A, keyPressed);
        break;
    case SDLK_KP_2:
        SetControllerButtonState(2, ControllerButton::B, keyPressed);
        break;
    case SDLK_KP_4:
        SetControllerButtonState(2, ControllerButton::Select, keyPressed);
        break;
    case SDLK_KP_5:
        SetControllerButtonState(2, ControllerButton::Start, keyPressed);
        break;
    case SDLK_UP:
        SetControllerButtonState(2, ControllerButton::Up, keyPressed);
        break;
    case SDLK_DOWN:
        SetControllerButtonState(2, ControllerButton::Down, keyPressed);
        break;
    case SDLK_LEFT:
        SetControllerButtonState(2, ControllerButton::Left, keyPressed);
        break;
    case SDLK_RIGHT:
        SetControllerButtonState(2, ControllerButton::Right, keyPressed);
        break;
    }
}

void NES::AudioSampleCallback(void* userdata, Uint8* stream, int len)
{
    static std::vector<float> audioBuffer;
    static bool resetDelay = true;

    APU* apu = (APU*)userdata;
    std::vector<float>& apuSampleBuffer = apu->GetBuffer();

    if (apuSampleBuffer.size() > AudioConstants::CLOCK_RATE / OUTPUT_AUDIO_SAMPLE_RATE)
    {
        AudioUtils::LowPassFilter(apuSampleBuffer, 5000.0, AudioConstants::CLOCK_RATE);
        AudioUtils::ResampleAndAppend(apuSampleBuffer, audioBuffer, AudioConstants::CLOCK_RATE, OUTPUT_AUDIO_SAMPLE_RATE);
        apuSampleBuffer.clear();
    }

    const int BUFFER_DELAY = 4096;
    if (resetDelay)
    {
        if (audioBuffer.size() < BUFFER_DELAY) return;
        else resetDelay = false;
    }

    if (audioBuffer.size() < len / sizeof(float))
    {
        resetDelay = true;
        return;
    }

    memcpy(stream, audioBuffer.data(), len);
    audioBuffer.erase(audioBuffer.begin(), audioBuffer.begin() + (len / sizeof(float)));
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

void NES::InitializeAPU()
{
    m_apu = std::make_shared<APU>();

    SDL_AudioSpec audioSpec;

    // Configure Audio Spec
    SDL_zero(audioSpec);
    audioSpec.freq = OUTPUT_AUDIO_SAMPLE_RATE;
    audioSpec.format = AUDIO_F32;  // 32-bit float PCM
    audioSpec.channels = 1;        // Mono
    audioSpec.samples = 512;       // Buffer size
    audioSpec.callback = AudioSampleCallback;
    audioSpec.userdata = m_apu.get();

    if (SDL_OpenAudio(&audioSpec, NULL) < 0)
        throw std::runtime_error("SDL Open Audio Failed");

    SDL_PauseAudio(0);
}

void NES::InitializeCPU()
{
    m_cpuBus = std::make_shared<CpuBus>();
    m_cpuBus->ConnectControllers(m_controllerOneState, m_controllerTwoState);
    m_cpuBus->ConnectCartridge(m_cartridge);
    m_cpuBus->ConnectPPU(m_ppu);
    m_cpuBus->ConnectAPU(m_apu);
    m_cpu = std::make_unique<CPU>(m_cpuBus);
}

void NES::SetControllerButtonState(uint8_t controllerNumber, ControllerButton button, bool newState) const
{
    std::shared_ptr<uint8_t> controller;
    if (controllerNumber == 1) controller = m_controllerOneState;
    else if (controllerNumber == 2) controller = m_controllerTwoState;
    else return;

    if (newState) *controller |= static_cast<uint8_t>(button);
    else *controller &= ~static_cast<uint8_t>(button);
}
