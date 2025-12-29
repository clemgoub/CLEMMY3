#pragma once

#include <cmath>
#include <random>

/**
 * LFO (Low Frequency Oscillator)
 * Generates modulation signals for various synth parameters
 */
class LFO
{
public:
    enum Waveform
    {
        Sine = 0,
        Triangle,
        Square,
        Sawtooth,
        SampleAndHold  // Random stepped values
    };

    LFO();

    void setSampleRate(double sr);
    void reset();  // Reset phase to 0

    void setWaveform(Waveform wf);
    void setRate(float rateHz);  // 0.01 - 20 Hz
    void setDepth(float depth);  // 0.0 - 1.0

    // Process one sample and return modulation value (-1 to +1, scaled by depth)
    float processSample();

    // Get current LFO value without advancing (for display)
    float getCurrentValue() const;

private:
    float generateSine();
    float generateTriangle();
    float generateSquare();
    float generateSawtooth();
    float generateSampleAndHold();

    void updatePhaseIncrement();

    double sampleRate = 44100.0;
    Waveform waveform = Sine;
    float rate = 1.0f;  // Hz
    float depth = 0.0f;  // 0.0 - 1.0

    float phase = 0.0f;  // 0.0 - 1.0
    float phaseIncrement = 0.0f;

    // For sample & hold
    float sampleAndHoldValue = 0.0f;
    float lastPhase = 0.0f;
    std::mt19937 randomGenerator;
    std::uniform_real_distribution<float> randomDistribution;
};
