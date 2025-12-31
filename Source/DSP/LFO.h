#pragma once

#include <cmath>
#include <random>

/**
 * LFO (Low Frequency Oscillator)
 * Generates modulation signals for various synth parameters
 * Supports both free-running (Hz) and MIDI-synced modes
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

    enum RateMode
    {
        Free = 0,  // Free-running Hz
        Sync       // MIDI tempo sync
    };

    enum SyncDivision
    {
        Div_1_16 = 0,  // 1/16 note
        Div_1_8,       // 1/8 note
        Div_1_4,       // 1/4 note
        Div_1_2,       // 1/2 note
        Div_1_1,       // Whole note
        Div_2_1,       // 2 bars
        Div_4_1        // 4 bars
    };

    LFO();

    void setSampleRate(double sr);
    void reset();  // Reset phase to 0

    void setWaveform(Waveform wf);
    void setRate(float rateHz);  // 0.01 - 20 Hz (used in Free mode)
    void setDepth(float depth);  // 0.0 - 1.0
    void setRateMode(RateMode mode);
    void setSyncDivision(SyncDivision division);
    void setBPM(float bpm);  // For MIDI sync mode

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
    float getEffectiveRate() const;  // Calculate rate based on mode

    double sampleRate = 44100.0;
    Waveform waveform = Sine;
    RateMode rateMode = Free;
    SyncDivision syncDivision = Div_1_4;  // Default: quarter note
    float rate = 1.0f;  // Hz (Free mode)
    float bpm = 120.0f;  // BPM (Sync mode)
    float depth = 0.0f;  // 0.0 - 1.0

    float phase = 0.0f;  // 0.0 - 1.0
    float phaseIncrement = 0.0f;

    // For sample & hold
    float sampleAndHoldValue = 0.0f;
    float lastPhase = 0.0f;
    std::mt19937 randomGenerator;
    std::uniform_real_distribution<float> randomDistribution;
};
