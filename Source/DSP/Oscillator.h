#pragma once

#include "AudioUtils.h"

/**
 * Oscillator - Waveform generator with PolyBLEP anti-aliasing
 *
 * Generates three waveforms:
 * - Sine: Pure sine wave
 * - Sawtooth: Bright, buzzy waveform with PolyBLEP
 * - Square: Hollow waveform with pulse width modulation and PolyBLEP
 *
 * Python reference: sine_generator_qt.py:3555-3615 (generate_waveform)
 */
class Oscillator
{
public:
    enum class Waveform
    {
        Sine = 0,
        Sawtooth = 1,
        Square = 2
    };

    Oscillator();

    /**
     * Set the sample rate
     * Must be called before processing
     */
    void setSampleRate(double sampleRate);

    /**
     * Set the oscillator frequency in Hz
     * @param frequency Frequency in Hz (20 - 20000)
     */
    void setFrequency(float frequency);

    /**
     * Set the waveform type
     * @param waveform Sine, Sawtooth, or Square
     */
    void setWaveform(Waveform waveform);

    /**
     * Set pulse width for square wave
     * @param pw Pulse width (0.01 - 0.99), default 0.5
     */
    void setPulseWidth(float pw);

    /**
     * Generate one audio sample
     * @return Audio sample in range -1.0 to +1.0
     */
    float processSample();

    /**
     * Reset oscillator phase to 0
     */
    void reset();

private:
    // Oscillator state
    double phase = 0.0;              // Current phase (0.0 to 1.0)
    double phaseIncrement = 0.0;     // Phase increment per sample
    double sampleRate = 44100.0;

    // Parameters
    float frequency = 440.0f;
    Waveform waveform = Waveform::Sine;
    float pulseWidth = 0.5f;

    // Waveform generators
    float generateSine();
    float generateSawtooth();
    float generateSquare();

    // Helper methods
    void updatePhaseIncrement();
};
