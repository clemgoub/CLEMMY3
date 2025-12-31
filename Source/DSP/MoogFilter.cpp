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
    float inputWithFeedback = input - stage4 * feedbackGain;

    // Apply tanh saturation to input for analog warmth and low-end character
    // This prevents the filter from exploding at high resonance
    float saturatedInput = std::tanh(inputWithFeedback);

    // 4 one-pole lowpass stages in series (cascade)
    // Each stage smooths the signal: stage[n] += g * (input - stage[n])
    stage1 = stage1 + g * (saturatedInput - stage1);
    stage2 = stage2 + g * (stage1 - stage2);
    stage3 = stage3 + g * (stage2 - stage3);
    stage4 = stage4 + g * (stage3 - stage4);

    // Clamp state variables to prevent overflow
    stage1 = std::clamp(stage1, -10.0f, 10.0f);
    stage2 = std::clamp(stage2, -10.0f, 10.0f);
    stage3 = std::clamp(stage3, -10.0f, 10.0f);
    stage4 = std::clamp(stage4, -10.0f, 10.0f);

    // Output depends on mode
    float output = 0.0f;

    switch (mode)
    {
        case LowPass:
            // 24dB/octave lowpass (all 4 stages)
            output = stage4;
            break;

        case BandPass:
            // True bandpass: difference between stages creates notch filter
            // Cuts both lows (via stage1-stage4 difference) and highs (via filtering)
            // Creates a peak at the cutoff frequency
            output = stage1 - stage4;
            break;

        case HighPass:
            // High-pass: input (with feedback) minus lowpass output
            // HP = (input - feedback) - LP(input - feedback)
            output = inputWithFeedback - stage4;
            break;
    }

    // Resonance compensation: boost output volume at high resonance
    // Frequency-dependent: less compensation at very high cutoffs to prevent volume drop
    // At cutoff < 8kHz: full compensation, at 12kHz: minimal compensation
    float cutoffRatio = std::clamp((12000.0f - cutoff) / 4000.0f, 0.2f, 1.0f);  // 1.0 at low freq, 0.2 at 12kHz
    float resonanceCompensation = 1.0f + (feedbackGain * 0.15f * cutoffRatio);
    output *= resonanceCompensation;

    // Clamp output to prevent overflow
    output = std::clamp(output, -10.0f, 10.0f);

    return output;
}

void MoogFilter::updateCoefficients()
{
    // Normalize cutoff to Nyquist frequency (0.0 - 0.5)
    // Clamp to well below Nyquist to prevent instability
    // 0.45 is safe maximum (0.499 causes tan(π/2) → infinity!)
    float normalizedCutoff = std::clamp(cutoff / static_cast<float>(sampleRate), 0.0f, 0.45f);

    // Use tan() for frequency warping (bilinear transform pre-warping)
    // This gives the aggressive low-end character preferred by user
    g = std::tan(static_cast<float>(M_PI) * normalizedCutoff);

    // Clamp g to prevent extreme values
    // At normalizedCutoff = 0.45, g ≈ 4.7, which is safe
    g = std::clamp(g, 0.0f, 10.0f);

    // Resonance to feedback gain
    // Range: 0.0 - 3.5 (empirically chosen for good resonance without instability)
    // Reduce feedback at very high cutoffs to prevent volume drop issue
    float highCutoffReduction = 1.0f;
    if (cutoff > 8000.0f)
    {
        // Gradually reduce feedback from 8kHz to 12kHz
        // At 8kHz: 100%, at 12kHz: 60%
        highCutoffReduction = 1.0f - ((cutoff - 8000.0f) / 4000.0f) * 0.4f;
        highCutoffReduction = std::clamp(highCutoffReduction, 0.6f, 1.0f);
    }

    feedbackGain = resonance * 3.5f * highCutoffReduction;

    coefficientsNeedUpdate = false;
}
