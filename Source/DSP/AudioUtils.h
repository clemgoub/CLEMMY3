#pragma once

#include <cmath>

namespace AudioUtils
{
    /**
     * Convert MIDI note number to frequency in Hz
     * @param midiNote MIDI note number (0-127)
     * @return Frequency in Hz
     *
     * Formula: f = 440 * 2^((n-69)/12)
     * where n is MIDI note number, 69 = A4 (440Hz)
     */
    inline float midiNoteToFrequency(int midiNote)
    {
        return 440.0f * std::pow(2.0f, (midiNote - 69) / 12.0f);
    }

    /**
     * PolyBLEP (Polynomial Band-Limited Step) anti-aliasing
     * Removes aliasing artifacts from discontinuities in waveforms
     *
     * Based on the paper: "Alias-Free Digital Synthesis of Classic Analog Waveforms"
     *
     * @param t Normalized phase (0.0 to 1.0)
     * @param dt Normalized frequency (phase increment per sample)
     * @return Correction value to add to naive waveform
     *
     * Python reference: sine_generator_qt.py:640-680
     */
    inline float polyBLEP(double t, double dt)
    {
        // t is phase from 0.0 to 1.0
        // dt is the phase increment (frequency / sampleRate)

        // Discontinuity at t=0 (rising edge)
        if (t < dt)
        {
            t /= dt;
            // 2t - t^2 - 1
            return t + t - t * t - 1.0f;
        }
        // Discontinuity at t=1 (falling edge)
        else if (t > 1.0 - dt)
        {
            t = (t - 1.0) / dt;
            // t^2 + 2t + 1
            return t * t + t + t + 1.0f;
        }

        // No discontinuity
        return 0.0f;
    }

    /**
     * Wrap phase to 0.0-1.0 range
     * @param phase Phase value to wrap (modified in-place)
     */
    inline void wrapPhase(double& phase)
    {
        while (phase >= 1.0)
            phase -= 1.0;
        while (phase < 0.0)
            phase += 1.0;
    }

    /**
     * Linear interpolation
     * @param a Start value
     * @param b End value
     * @param t Interpolation factor (0.0 to 1.0)
     * @return Interpolated value
     */
    inline float lerp(float a, float b, float t)
    {
        return a + t * (b - a);
    }

    /**
     * Clamp value to range
     * @param value Value to clamp
     * @param min Minimum value
     * @param max Maximum value
     * @return Clamped value
     */
    inline float clamp(float value, float min, float max)
    {
        if (value < min) return min;
        if (value > max) return max;
        return value;
    }
}
