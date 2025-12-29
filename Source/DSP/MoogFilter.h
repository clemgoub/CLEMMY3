#pragma once

#include <cmath>

/**
 * MoogFilter - 4-pole Moog ladder filter
 *
 * Classic Moog lowpass topology with three modes:
 * - LowPass: 24dB/octave rolloff (classic Moog sound)
 * - BandPass: 12dB/octave bandpass (isolates mid frequencies)
 * - HighPass: High-pass by subtraction (removes bass)
 *
 * Features:
 * - Resonance control (0.0 - 1.0) creates peak at cutoff frequency
 * - Tanh saturation for analog warmth and stability
 * - Coefficient caching for performance
 *
 * Signal flow:
 * Input → [Feedback] → 4 one-pole stages → Output
 *
 * Python reference: sine_generator_qt.py:370-440 (MoogLadderFilter class)
 */
class MoogFilter
{
public:
    enum Mode
    {
        LowPass = 0,   // 24dB/octave lowpass (output from stage 4)
        BandPass = 1,  // 12dB/octave bandpass (output from stage 2)
        HighPass = 2   // High-pass by subtraction (input - stage 4)
    };

    MoogFilter();

    /**
     * Initialization
     */
    void setSampleRate(double sampleRate);
    void reset();

    /**
     * Filter parameters
     */
    void setMode(Mode mode);
    void setCutoff(float cutoffHz);        // 20.0 - 12000.0 Hz
    void setResonance(float resonance);    // 0.0 - 1.0

    /**
     * Audio processing
     * @param input Audio sample to filter
     * @return Filtered sample
     */
    float processSample(float input);

    /**
     * Getters
     */
    Mode getMode() const { return mode; }
    float getCutoff() const { return cutoff; }
    float getResonance() const { return resonance; }

private:
    // Filter mode
    Mode mode = LowPass;

    // Sample rate
    double sampleRate = 44100.0;

    // Filter parameters
    float cutoff = 1000.0f;      // Hz
    float resonance = 0.0f;      // 0.0 - 1.0

    // 4 filter stages (one-pole lowpass each)
    float stage1 = 0.0f;
    float stage2 = 0.0f;
    float stage3 = 0.0f;
    float stage4 = 0.0f;

    // Cached coefficients (only recalculate when parameters change)
    float g = 0.0f;              // Cutoff coefficient
    float feedbackGain = 0.0f;   // Resonance feedback amount
    bool coefficientsNeedUpdate = true;

    /**
     * Update filter coefficients when cutoff or resonance changes
     */
    void updateCoefficients();
};
