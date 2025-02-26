#pragma once

#include <algorithm>

#include <SDL2/SDL.h>

#include "apu.h"

struct AudioUtils
{
    static constexpr int APU_SAMPLE_RATE = 1789773;
    static constexpr int OUTPUT_SAMPLE_RATE = 44100;

    static void AudioSampleCallback(void* userdata, Uint8* stream, int len);
    static void LowPassFilter(std::vector<float>& buffer, double cutoffFreq, double sampleRate);
    static void ResampleAndAppend(const std::vector<float>& input, std::vector<float>& output, int inputRate, int outputRate);
};