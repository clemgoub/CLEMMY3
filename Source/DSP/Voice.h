#pragma once

#include "Oscillator.h"
#include "Envelope.h"

/**
 * Voice - Single synthesizer voice
 *
 * Encapsulates an oscillator and envelope for polyphonic synthesis.
 * Manages voice lifecycle (note-on, note-off, age tracking for voice stealing).
 *
 * Python reference: sine_generator_qt.py:100-177 (Voice class)
 */
class Voice
{
public:
    Voice();

    /**
     * Set the sample rate
     * Must be called before processing
     */
    void setSampleRate(double sampleRate);

    /**
     * Voice lifecycle
     */
    void noteOn(int midiNote, float velocity, float unisonDetune = 0.0f);
    void noteOff();
    void reset();

    /**
     * Parameter updates - broadcast from VoiceManager
     */
    void setOscillatorWaveform(Oscillator::Waveform waveform);
    void setOscillatorPulseWidth(float pw);
    void setEnvelopeParameters(float attack, float decay, float sustain, float release);

    /**
     * Audio generation
     * @return Single audio sample
     */
    float processSample();

    /**
     * Voice state queries
     */
    bool isActive() const;          // Has active note (envelope not idle)
    bool isSounding() const;        // Producing audible output (level > threshold)
    int getCurrentNote() const { return currentMidiNote; }
    int getAge() const { return age; }

    /**
     * Age management for voice stealing (LRU algorithm)
     */
    void incrementAge() { ++age; }
    void resetAge() { age = 0; }

private:
    // DSP components
    Oscillator oscillator;
    Envelope envelope;

    // Voice state
    int currentMidiNote = -1;   // -1 = voice is free
    int age = 0;                // Increments each audio callback (for LRU stealing)
    float unisonDetune = 0.0f;  // Detuning in cents for unison mode

    /**
     * Update oscillator frequency based on MIDI note and detune
     */
    void updateFrequency();
};
