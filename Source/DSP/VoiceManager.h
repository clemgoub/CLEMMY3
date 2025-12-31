#pragma once

#include "Voice.h"
#include <array>

/**
 * VoiceManager - Polyphonic voice management system
 *
 * Manages a pool of voices with three modes:
 * - MONO: Single voice, last note priority
 * - POLY: Up to MAX_VOICES polyphony with voice stealing (LRU)
 * - UNISON: All voices play same note, detuned for thickness
 *
 * Python reference: sine_generator_qt.py:4050-4140 (MIDI), 4230-4260 (stealing)
 */
class VoiceManager
{
public:
    enum class VoiceMode
    {
        Mono = 0,   // Single voice, last note priority
        Poly = 1,   // Up to MAX_VOICES polyphony
        Unison = 2  // All voices play same note, detuned
    };

    static constexpr int MAX_VOICES = 8;

    VoiceManager();

    /**
     * Initialization
     */
    void setSampleRate(double sampleRate);
    void setVoiceMode(VoiceMode mode);
    void setUnisonDetune(float detuneCents);  // 5-25 cents

    /**
     * MIDI note handling
     */
    void noteOn(int midiNote, float velocity);
    void noteOff(int midiNote);
    void allNotesOff();     // Send note-off to all voices
    void allSoundOff();     // Immediate silence

    /**
     * Per-oscillator parameter broadcasting
     */
    void setOscillatorEnabled(int oscIndex, bool enabled);
    void setOscillatorWaveform(int oscIndex, Oscillator::Waveform waveform);
    void setOscillatorGain(int oscIndex, float gain);
    void setOscillatorDetune(int oscIndex, float cents);
    void setOscillatorOctave(int oscIndex, int octaveOffset);
    void setOscillatorPulseWidth(int oscIndex, float pw);

    /**
     * Noise parameters (per-voice, controlled by envelope)
     */
    void setNoiseEnabled(bool enabled);
    void setNoiseType(NoiseGenerator::NoiseType type);
    void setNoiseGain(float gain);

    /**
     * Filter parameters (shared by all voices)
     */
    void setFilterMode(MoogFilter::Mode mode);
    void setFilterCutoff(float cutoffHz);      // 20.0 - 12000.0 Hz
    void setFilterResonance(float resonance);  // 0.0 - 1.0

    /**
     * Envelope parameters (shared by all voices)
     */
    void setEnvelopeParameters(float attack, float decay, float sustain, float release);

    /**
     * LFO parameters (shared by all voices)
     */
    void setLFO1Waveform(LFO::Waveform waveform);
    void setLFO1Rate(float rateHz);
    void setLFO1Depth(float depth);
    void setLFO1Destination(int dest);
    void setLFO1RateMode(LFO::RateMode mode);
    void setLFO1SyncDivision(LFO::SyncDivision division);
    void setLFO1BPM(float bpm);

    void setLFO2Waveform(LFO::Waveform waveform);
    void setLFO2Rate(float rateHz);
    void setLFO2Depth(float depth);
    void setLFO2Destination(int dest);
    void setLFO2RateMode(LFO::RateMode mode);
    void setLFO2SyncDivision(LFO::SyncDivision division);
    void setLFO2BPM(float bpm);

    /**
     * Audio generation
     * @return Mixed output from all active voices
     */
    float processSample();

    /**
     * Voice statistics
     */
    int getNumActiveVoices() const;

private:
    // Voice pool
    std::array<Voice, MAX_VOICES> voices;
    VoiceMode voiceMode = VoiceMode::Poly;
    float unisonDetuneAmount = 10.0f;  // Default: ±10 cents

    /**
     * Voice allocation helpers
     */
    Voice* findFreeVoice();
    Voice* findVoicePlayingNote(int midiNote);
    Voice* stealVoice();

    /**
     * Mode-specific allocation
     */
    void allocateMonoVoice(int midiNote, float velocity);
    void allocatePolyVoice(int midiNote, float velocity);
    void allocateUnisonVoices(int midiNote, float velocity);

    /**
     * Unison detuning calculation
     * Spreads voices across a range for thick sound
     */
    float calculateUnisonDetune(int voiceIndex) const;

    /**
     * Age management for LRU voice stealing
     */
    void incrementAllAges();
};
