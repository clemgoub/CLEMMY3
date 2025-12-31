#pragma once

#include "Oscillator.h"
#include "NoiseGenerator.h"
#include "Envelope.h"
#include "MoogFilter.h"
#include "LFO.h"
#include <array>

/**
 * Voice - Single synthesizer voice with triple oscillator architecture
 *
 * Contains 3 independent oscillators + noise generator, all mixed before
 * applying a single envelope (post-mixer architecture for efficiency).
 *
 * Each oscillator has independent controls:
 * - Enabled/disabled
 * - Waveform selection
 * - Gain (0-100%)
 * - Detune (±100 cents)
 * - Octave offset (-3 to +3)
 * - Pulse width (for square wave)
 *
 * Python reference: sine_generator_qt.py:523-620 (Voice class with 3 oscillators)
 */
class Voice
{
public:
    static constexpr int NUM_OSCILLATORS = 3;

    Voice();

    /**
     * Set the sample rate
     * Must be called before processing
     */
    void setSampleRate(double sampleRate);

    /**
     * Voice lifecycle
     */
    void noteOn(int midiNote, float velocity, float unisonDetune = 0.0f, bool randomizePhase = false);
    void noteOff();
    void reset();

    /**
     * Per-oscillator parameter updates
     */
    void setOscillatorEnabled(int oscIndex, bool enabled);
    void setOscillatorWaveform(int oscIndex, Oscillator::Waveform waveform);
    void setOscillatorGain(int oscIndex, float gain);          // 0.0 to 1.0
    void setOscillatorDetune(int oscIndex, float cents);       // ±100 cents
    void setOscillatorOctave(int oscIndex, int octaveOffset);  // -3 to +3
    void setOscillatorPulseWidth(int oscIndex, float pw);      // 0.01 to 0.99

    /**
     * Noise generator parameters
     */
    void setNoiseEnabled(bool enabled);
    void setNoiseType(NoiseGenerator::NoiseType type);
    void setNoiseGain(float gain);  // 0.0 to 1.0

    /**
     * Envelope parameters (shared by all oscillators + noise)
     */
    void setEnvelopeParameters(float attack, float decay, float sustain, float release);

    /**
     * Filter parameters
     */
    void setFilterMode(MoogFilter::Mode mode);
    void setFilterCutoff(float cutoffHz);      // 20.0 - 12000.0 Hz
    void setFilterResonance(float resonance);  // 0.0 - 1.0

    /**
     * LFO parameters (2 LFOs per voice)
     */
    void setLFO1Waveform(LFO::Waveform waveform);
    void setLFO1Rate(float rateHz);              // 0.01 - 20 Hz
    void setLFO1Depth(float depth);              // 0.0 - 1.0
    void setLFO1Destination(int dest);           // Modulation target
    void setLFO1RateMode(LFO::RateMode mode);    // Free or Sync
    void setLFO1SyncDivision(LFO::SyncDivision division);  // 1/16, 1/8, 1/4, etc.
    void setLFO1BPM(float bpm);                  // For MIDI sync

    void setLFO2Waveform(LFO::Waveform waveform);
    void setLFO2Rate(float rateHz);              // 0.01 - 20 Hz
    void setLFO2Depth(float depth);              // 0.0 - 1.0
    void setLFO2Destination(int dest);           // Modulation target
    void setLFO2RateMode(LFO::RateMode mode);    // Free or Sync
    void setLFO2SyncDivision(LFO::SyncDivision division);  // 1/16, 1/8, 1/4, etc.
    void setLFO2BPM(float bpm);                  // For MIDI sync

    /**
     * Audio generation
     * @return Single audio sample (mixed oscillators with envelope applied)
     */
    float processSample();

    /**
     * Voice state queries
     */
    bool isActive() const;          // Has active note (envelope not idle)
    bool isSounding() const;        // Producing audible output (not in release)
    int getCurrentNote() const { return currentMidiNote; }
    int getAge() const { return age; }

    /**
     * Age management for voice stealing (LRU algorithm)
     */
    void incrementAge() { ++age; }
    void resetAge() { age = 0; }

private:
    // LFO modulation destinations
    enum ModDestination
    {
        ModNone = 0,
        ModFilterCutoff,
        ModPitch,        // Vibrato (all oscillators)
        ModPWM,          // Modulate pulse width
        ModFilterRes,    // Filter resonance
        ModVolume        // Tremolo (amplitude modulation)
    };

    // Oscillator settings (per-oscillator parameters)
    struct OscillatorSettings
    {
        bool enabled = true;
        float gain = 0.33f;              // Default: 33% each for 3 oscillators
        float detuneCents = 0.0f;        // ±100 cents
        int octaveOffset = 0;            // -3 to +3 octaves
    };

    // DSP components
    std::array<Oscillator, NUM_OSCILLATORS> oscillators;
    NoiseGenerator noiseGenerator;
    MoogFilter filter;       // Applied after mixing, before envelope
    Envelope envelope;
    LFO lfo1;
    LFO lfo2;

    // Per-oscillator settings
    std::array<OscillatorSettings, NUM_OSCILLATORS> oscSettings;

    // Noise settings
    bool noiseEnabled = false;
    float noiseGain = 0.0f;

    // LFO settings
    ModDestination lfo1Destination = ModNone;
    ModDestination lfo2Destination = ModNone;
    float baseFilterCutoff = 1000.0f;     // Unmodulated filter cutoff
    float baseFilterResonance = 0.0f;     // Unmodulated filter resonance

    // Voice state
    int currentMidiNote = -1;   // -1 = voice is free
    int age = 0;                // Increments each audio callback (for LRU stealing)
    float unisonDetune = 0.0f;  // Detuning in cents for unison mode

    /**
     * Update all oscillator frequencies based on MIDI note, octave, detune, and unison detune
     */
    void updateOscillatorFrequencies();

    /**
     * Mix all enabled oscillators + noise
     * @return Mixed signal (before envelope)
     */
    float mixOscillators();
};
