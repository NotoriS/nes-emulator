#pragma once

#include <vector>
#include <numbers>

struct AudioUtils
{
    static void LowPassFilter(std::vector<float>& buffer, double cutoffFreq, double sampleRate);
    static void ResampleAndAppend(const std::vector<float>& input, std::vector<float>& output, int inputRate, int outputRate);
};