#include "Voice.h"
#include "AudioUtils.h"
#include <cmath>

Voice::Voice()
{
}

void Voice::setSampleRate(double newSampleRate)
{
    oscillator.setSampleRate(newSampleRate);
    envelope.setSampleRate(newSampleRate);
}

void Voice::noteOn(int midiNote, float velocity, float detune)
{
    currentMidiNote = midiNote;
    unisonDetune = detune;

    // Reset oscillator phase for clean waveform start
    // This prevents frequency discontinuities when note changes
    oscillator.reset();

    // Update oscillator frequency with detune
    updateFrequency();

    // Trigger envelope with velocity
    // Envelope starts from current level for smooth retriggering
    envelope.noteOn(velocity);

    // Reset age for voice stealing
    resetAge();
}

void Voice::noteOff()
{
    // Release envelope (voice continues sounding until release phase completes)
    envelope.noteOff();

    // Note: currentMidiNote stays set until voice goes idle
    // This allows proper voice allocation for retriggering
}

void Voice::reset()
{
    // Reset oscillator phase to 0 for clean start
    oscillator.reset();

    // Reset envelope to idle state
    envelope.reset();

    // Clear voice state
    currentMidiNote = -1;
    age = 0;
    unisonDetune = 0.0f;
}

void Voice::setOscillatorWaveform(Oscillator::Waveform waveform)
{
    oscillator.setWaveform(waveform);
}

void Voice::setOscillatorPulseWidth(float pw)
{
    oscillator.setPulseWidth(pw);
}

void Voice::setEnvelopeParameters(float attack, float decay, float sustain, float release)
{
    envelope.setParameters(attack, decay, sustain, release);
}

float Voice::processSample()
{
    // Generate oscillator sample
    float oscSample = oscillator.processSample();

    // Apply envelope
    float envLevel = envelope.processSample();

    // If envelope has finished (idle), mark voice as free
    if (!envelope.isActive())
    {
        currentMidiNote = -1;
    }

    return oscSample * envLevel;
}

bool Voice::isActive() const
{
    // Voice is active if envelope is not idle
    return envelope.isActive();
}

bool Voice::isSounding() const
{
    // Voice is producing audible output if envelope is active
    // Prefer stealing voices in release phase over attack/decay/sustain
    if (!envelope.isActive())
        return false;

    // Voices in release phase are better candidates for stealing
    return envelope.getCurrentPhase() != Envelope::Phase::Release;
}

void Voice::updateFrequency()
{
    if (currentMidiNote < 0)
        return;

    // Calculate base frequency from MIDI note
    float baseFrequency = AudioUtils::midiNoteToFrequency(currentMidiNote);

    // Apply unison detuning (in cents)
    // Formula: freq * 2^(cents/1200)
    if (unisonDetune != 0.0f)
    {
        float detuneFactor = std::pow(2.0f, unisonDetune / 1200.0f);
        baseFrequency *= detuneFactor;
    }

    oscillator.setFrequency(baseFrequency);
}
