#include "audio-utils.h"

void AudioUtils::AudioSampleCallback(void* userdata, Uint8* stream, int len)
{
    static std::vector<float> audioBuffer;

    APU* apu = (APU*)userdata;
    std::vector<float>& apuSampleBuffer = apu->GetBuffer();

    if (apuSampleBuffer.size() > APU_SAMPLE_RATE / OUTPUT_SAMPLE_RATE)
    {
        LowPassFilter(apuSampleBuffer, 13000.0, APU_SAMPLE_RATE);
        ResampleAndAppend(apuSampleBuffer, audioBuffer, APU_SAMPLE_RATE, OUTPUT_SAMPLE_RATE);
        apuSampleBuffer.clear();
    }

    int samplesToCopy = std::min<int>(len / sizeof(float), (int)audioBuffer.size());
    memcpy(stream, audioBuffer.data(), samplesToCopy * sizeof(float));
    audioBuffer.erase(audioBuffer.begin(), audioBuffer.begin() + samplesToCopy);
}

void AudioUtils::LowPassFilter(std::vector<float>& buffer, double cutoffFreq, double sampleRate)
{
    double RC = 1.0 / (cutoffFreq * 2.0 * M_PI);
    double dt = 1.0 / sampleRate;
    double alpha = dt / (RC + dt);

    float prevSample = buffer[0];
    for (size_t i = 1; i < buffer.size(); i++)
    {
        buffer[i] = prevSample + alpha * (buffer[i] - prevSample);
        prevSample = buffer[i];
    }
}

void AudioUtils::ResampleAndAppend(const std::vector<float>& input, std::vector<float>& output, int inputRate, int outputRate)
{
    double step = (double)inputRate / outputRate;
    double position = 0.0;

    while (position + 1 < input.size())
    {
        int index = (int)position;
        double t = position - index;

        float sample = (1 - t) * input[index] + t * input[index + 1];
        output.push_back(sample);
        position += step;
    }
}
