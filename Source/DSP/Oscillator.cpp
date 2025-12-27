#include "Oscillator.h"
#include <cmath>

#ifndef M_PI
    #define M_PI 3.14159265358979323846
#endif

Oscillator::Oscillator()
{
    updatePhaseIncrement();
}

void Oscillator::setSampleRate(double newSampleRate)
{
    sampleRate = newSampleRate;
    updatePhaseIncrement();
}

void Oscillator::setFrequency(float newFrequency)
{
    frequency = AudioUtils::clamp(newFrequency, 20.0f, 20000.0f);
    updatePhaseIncrement();
}

void Oscillator::setWaveform(Waveform newWaveform)
{
    waveform = newWaveform;
}

void Oscillator::setPulseWidth(float pw)
{
    pulseWidth = AudioUtils::clamp(pw, 0.01f, 0.99f);
}

void Oscillator::updatePhaseIncrement()
{
    // Phase increment = frequency / sampleRate
    // This gives us how much phase advances per sample
    phaseIncrement = frequency / sampleRate;
}

float Oscillator::processSample()
{
    float sample = 0.0f;

    // Generate waveform based on current selection
    switch (waveform)
    {
        case Waveform::Sine:
            sample = generateSine();
            break;

        case Waveform::Sawtooth:
            sample = generateSawtooth();
            break;

        case Waveform::Square:
            sample = generateSquare();
            break;
    }

    // Advance phase
    phase += phaseIncrement;

    // Wrap phase to 0.0-1.0 range
    AudioUtils::wrapPhase(phase);

    return sample;
}

void Oscillator::reset()
{
    phase = 0.0;
}

// ============================================================================
// Waveform Generators
// ============================================================================

float Oscillator::generateSine()
{
    // Pure sine wave - no aliasing, no PolyBLEP needed
    return std::sin(phase * 2.0 * M_PI);
}

float Oscillator::generateSawtooth()
{
    // Naive sawtooth: linear ramp from -1 to +1
    float naiveSaw = 2.0f * static_cast<float>(phase) - 1.0f;

    // Apply PolyBLEP to smooth the discontinuity at phase wraparound
    // This removes aliasing artifacts
    float polyBlepCorrection = AudioUtils::polyBLEP(phase, phaseIncrement);

    return naiveSaw - polyBlepCorrection;
}

float Oscillator::generateSquare()
{
    // Naive square wave with pulse width modulation
    float naiveSquare = (phase < pulseWidth) ? 1.0f : -1.0f;

    // Apply PolyBLEP at both discontinuities
    float polyBlepCorrection = 0.0f;

    // Discontinuity at rising edge (phase = 0)
    polyBlepCorrection += AudioUtils::polyBLEP(phase, phaseIncrement);

    // Discontinuity at falling edge (phase = pulseWidth)
    // Shift phase to treat pulseWidth as the discontinuity point
    double phaseShifted = phase - pulseWidth;
    if (phaseShifted < 0.0)
        phaseShifted += 1.0;

    polyBlepCorrection -= AudioUtils::polyBLEP(phaseShifted, phaseIncrement);

    return naiveSquare - polyBlepCorrection;
}
