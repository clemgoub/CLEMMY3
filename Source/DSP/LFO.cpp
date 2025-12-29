#include "LFO.h"
#include <algorithm>

LFO::LFO()
    : randomGenerator(std::random_device{}())
    , randomDistribution(-1.0f, 1.0f)
{
    updatePhaseIncrement();
}

void LFO::setSampleRate(double sr)
{
    sampleRate = sr;
    updatePhaseIncrement();
}

void LFO::reset()
{
    phase = 0.0f;
    lastPhase = 0.0f;
    sampleAndHoldValue = randomDistribution(randomGenerator);
}

void LFO::setWaveform(Waveform wf)
{
    waveform = wf;
}

void LFO::setRate(float rateHz)
{
    rate = std::clamp(rateHz, 0.01f, 20.0f);
    updatePhaseIncrement();
}

void LFO::setDepth(float d)
{
    depth = std::clamp(d, 0.0f, 1.0f);
}

float LFO::processSample()
{
    float value = 0.0f;

    switch (waveform)
    {
        case Sine:
            value = generateSine();
            break;
        case Triangle:
            value = generateTriangle();
            break;
        case Square:
            value = generateSquare();
            break;
        case Sawtooth:
            value = generateSawtooth();
            break;
        case SampleAndHold:
            value = generateSampleAndHold();
            break;
    }

    // Advance phase
    lastPhase = phase;
    phase += phaseIncrement;
    if (phase >= 1.0f)
        phase -= 1.0f;

    return value * depth;
}

float LFO::getCurrentValue() const
{
    // Return current value without advancing phase
    float value = 0.0f;

    switch (waveform)
    {
        case Sine:
            value = std::sin(phase * 2.0f * static_cast<float>(M_PI));
            break;
        case Triangle:
            if (phase < 0.5f)
                value = -1.0f + (phase * 4.0f);
            else
                value = 1.0f - ((phase - 0.5f) * 4.0f);
            break;
        case Square:
            value = (phase < 0.5f) ? 1.0f : -1.0f;
            break;
        case Sawtooth:
            value = -1.0f + (phase * 2.0f);
            break;
        case SampleAndHold:
            value = sampleAndHoldValue;
            break;
    }

    return value * depth;
}

float LFO::generateSine()
{
    return std::sin(phase * 2.0f * static_cast<float>(M_PI));
}

float LFO::generateTriangle()
{
    // Rising: 0 -> 0.5 maps to -1 -> +1
    // Falling: 0.5 -> 1.0 maps to +1 -> -1
    if (phase < 0.5f)
        return -1.0f + (phase * 4.0f);
    else
        return 1.0f - ((phase - 0.5f) * 4.0f);
}

float LFO::generateSquare()
{
    return (phase < 0.5f) ? 1.0f : -1.0f;
}

float LFO::generateSawtooth()
{
    // Rising sawtooth: 0 -> 1 maps to -1 -> +1
    return -1.0f + (phase * 2.0f);
}

float LFO::generateSampleAndHold()
{
    // Generate new random value when phase wraps
    if (phase < lastPhase)  // Phase wrapped
    {
        sampleAndHoldValue = randomDistribution(randomGenerator);
    }
    return sampleAndHoldValue;
}

void LFO::updatePhaseIncrement()
{
    // Phase increment per sample = frequency / sample rate
    phaseIncrement = rate / static_cast<float>(sampleRate);
}
