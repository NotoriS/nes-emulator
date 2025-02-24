#include "apu.h"

APU::APU()
{
}

APU::~APU()
{
}

void APU::Clock()
{
}

uint8_t APU::Read(uint16_t address)
{
    return 0;
}

void APU::Write(uint16_t address, uint8_t data)
{
}

int16_t APU::Sample()
{
    // Temporary function to test audio output

    static int phase = 0;              // Tracks the current sample position
    static float frequency = 440.0f;   // Frequency of the square wave (Hz)
    static float sampleRate = 44100.0f; // Sample rate (Hz)
    static int16_t amplitude = 100;  // Amplitude (volume)

    // Generate a square wave
    int16_t sample = (phase < (sampleRate / frequency / 2)) ? amplitude : -amplitude;

    // Increment the phase and wrap it around
    phase++;
    if (phase >= sampleRate / frequency) {
        phase = 0;
    }

    return sample;
}
