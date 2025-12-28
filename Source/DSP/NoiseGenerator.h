#pragma once

/**
 * NoiseGenerator - Global noise source for mixer channel
 *
 * Generates three types of noise:
 * - White: Full spectrum noise (equal energy per frequency)
 * - Pink: 1/f noise (equal energy per octave)
 * - Brown: 1/f² noise (low-frequency emphasis, "brownian motion")
 *
 * This is a single global noise source mixed with oscillator output,
 * not a per-voice waveform option.
 */
class NoiseGenerator
{
public:
    enum class NoiseType
    {
        White = 0,
        Pink = 1,
        Brown = 2
    };

    NoiseGenerator();

    /**
     * Set the sample rate
     * Must be called before processing
     */
    void setSampleRate(double sampleRate);

    /**
     * Set the noise type
     * @param type White, Pink, or Brown
     */
    void setNoiseType(NoiseType type);

    /**
     * Generate one noise sample
     * @return Noise sample in range -1.0 to +1.0
     */
    float processSample();

    /**
     * Reset noise generator state
     */
    void reset();

private:
    double sampleRate = 44100.0;
    NoiseType noiseType = NoiseType::White;

    // Pink noise state (Paul Kellett algorithm)
    // Uses multiple octaves of noise with different update rates
    float pinkState0 = 0.0f;
    float pinkState1 = 0.0f;
    float pinkState2 = 0.0f;
    float pinkState3 = 0.0f;
    float pinkState4 = 0.0f;
    float pinkState5 = 0.0f;
    float pinkState6 = 0.0f;

    // Brown noise state (integrated white noise)
    float brownState = 0.0f;

    // Noise generators
    float generateWhite();
    float generatePink();
    float generateBrown();

    // Helper: Generate random float in range [-1, +1]
    float randomFloat();
};
