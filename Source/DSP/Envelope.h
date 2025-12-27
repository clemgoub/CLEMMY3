#pragma once

/**
 * Envelope - ADSR Envelope Generator
 *
 * Generates an Attack-Decay-Sustain-Release envelope for shaping note dynamics.
 *
 * State Machine:
 * Idle → Attack → Decay → Sustain → Release → Idle
 *
 * Python reference: sine_generator_qt.py:180-260 (EnvelopeGenerator class)
 */
class Envelope
{
public:
    enum class Phase
    {
        Idle,       // No note playing
        Attack,     // Rising from 0 to peak
        Decay,      // Falling from peak to sustain
        Sustain,    // Holding at sustain level
        Release     // Falling from current level to 0
    };

    Envelope();

    /**
     * Set the sample rate
     * Must be called before processing
     */
    void setSampleRate(double sampleRate);

    /**
     * Set ADSR parameters
     * @param attack Attack time in seconds (0.001 - 2.0)
     * @param decay Decay time in seconds (0.001 - 2.0)
     * @param sustain Sustain level (0.0 - 1.0)
     * @param release Release time in seconds (0.001 - 5.0)
     */
    void setParameters(float attack, float decay, float sustain, float release);

    /**
     * Trigger note-on event
     * @param velocity Note velocity (0.0 - 1.0)
     */
    void noteOn(float velocity = 1.0f);

    /**
     * Trigger note-off event
     */
    void noteOff();

    /**
     * Reset envelope to idle state
     */
    void reset();

    /**
     * Process one sample
     * @return Envelope level (0.0 - 1.0)
     */
    float processSample();

    /**
     * Check if envelope is active (not idle)
     */
    bool isActive() const { return currentPhase != Phase::Idle; }

    /**
     * Get current envelope phase
     */
    Phase getCurrentPhase() const { return currentPhase; }

private:
    // Current state
    Phase currentPhase = Phase::Idle;
    float currentLevel = 0.0f;
    float velocity = 1.0f;

    // ADSR parameters (in seconds)
    float attackTime = 0.01f;
    float decayTime = 0.3f;
    float sustainLevel = 0.7f;
    float releaseTime = 0.5f;

    // Calculated rates (per-sample increments)
    float attackRate = 0.0f;
    float decayRate = 0.0f;
    float releaseRate = 0.0f;

    double sampleRate = 44100.0;

    // Helper methods
    void calculateRates();
    void enterPhase(Phase newPhase);
};
