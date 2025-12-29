#include "MoogFilter.h"
#include <algorithm>
#include <cmath>

MoogFilter::MoogFilter()
{
    reset();
}

void MoogFilter::setSampleRate(double sr)
{
    sampleRate = sr;
    coefficientsNeedUpdate = true;
}

void MoogFilter::reset()
{
    // Clear all filter stages
    stage1 = 0.0f;
    stage2 = 0.0f;
    stage3 = 0.0f;
    stage4 = 0.0f;
}

void MoogFilter::setMode(Mode m)
{
    mode = m;
}

void MoogFilter::setCutoff(float cutoffHz)
{
    // Clamp to valid range: 20 Hz - 12 kHz
    // Max 12 kHz is typical for analog Moog-style filters
    cutoff = std::clamp(cutoffHz, 20.0f, 12000.0f);
    coefficientsNeedUpdate = true;
}

void MoogFilter::setResonance(float res)
{
    // Clamp to 0.0 - 1.0 range
    resonance = std::clamp(res, 0.0f, 1.0f);
    coefficientsNeedUpdate = true;
}

float MoogFilter::processSample(float input)
{
    // Update coefficients if parameters changed
    if (coefficientsNeedUpdate)
    {
        updateCoefficients();
    }

    // Check for NaN/infinity in filter state and reset if detected
    if (std::isnan(stage1) || std::isnan(stage2) || std::isnan(stage3) || std::isnan(stage4) ||
        std::isinf(stage1) || std::isinf(stage2) || std::isinf(stage3) || std::isinf(stage4))
    {
        reset();
    }

    // Feedback from output to input (creates resonance peak)
    // Classic Moog ladder: feedback always from stage4 (final output)
    float feedback = stage4;
    float inputWithFeedback = input - feedback * feedbackGain;

    // Apply tanh saturation to input for analog warmth and stability
    // This prevents the filter from exploding at high resonance
    float saturatedInput = std::tanh(inputWithFeedback);

    // 4 one-pole lowpass stages in series (cascade)
    // Each stage smooths the signal: stage[n] += g * (input - stage[n])
    // These are LINEAR filters - no saturation here!
    stage1 = stage1 + g * (saturatedInput - stage1);
    stage2 = stage2 + g * (stage1 - stage2);
    stage3 = stage3 + g * (stage2 - stage3);
    stage4 = stage4 + g * (stage3 - stage4);

    // Output depends on mode
    float output = 0.0f;

    switch (mode)
    {
        case LowPass:
            // 24dB/octave lowpass (all 4 stages)
            output = stage4;
            break;

        case BandPass:
            // 12dB/octave bandpass (2 stages)
            output = stage2;
            break;

        case HighPass:
            // High-pass by subtraction (input minus lowpass)
            // Multiply LP by 1.5 to remove bass more aggressively
            output = input - (stage4 * 1.5f);
            break;
    }

    // Resonance compensation: boost output volume at high resonance
    // to compensate for feedback-induced volume loss
    float resonanceCompensation = 1.0f + (feedbackGain * 0.15f);  // Up to ~1.5x at max resonance
    output *= resonanceCompensation;

    return output;
}

void MoogFilter::updateCoefficients()
{
    // Cutoff coefficient (g)
    // Normalize cutoff to Nyquist frequency (0.0 - 0.5)
    // Clamp to well below Nyquist to prevent instability
    // 0.45 is safe maximum (0.499 causes tan(π/2) → infinity!)
    float normalizedCutoff = std::clamp(cutoff / static_cast<float>(sampleRate), 0.0f, 0.45f);

    // Use tan() for frequency warping (bilinear transform pre-warping)
    // This compensates for the frequency response error of the bilinear transform
    g = std::tan(static_cast<float>(M_PI) * normalizedCutoff);

    // Clamp g to prevent extreme values
    // At normalizedCutoff = 0.45, g ≈ 4.7, which is safe
    g = std::clamp(g, 0.0f, 10.0f);

    // Resonance to feedback gain
    // Range: 0.0 - 3.5 (empirically chosen for good resonance without instability)
    // At 3.5, the filter self-oscillates (becomes a sine wave oscillator)
    // We limit to slightly below self-oscillation for musical use
    feedbackGain = resonance * 3.5f;

    coefficientsNeedUpdate = false;
}
