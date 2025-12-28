#include "Voice.h"
#include "AudioUtils.h"
#include <cmath>

Voice::Voice()
{
}

void Voice::setSampleRate(double newSampleRate)
{
    // Initialize all oscillators with sample rate
    for (auto& osc : oscillators)
    {
        osc.setSampleRate(newSampleRate);
    }

    noiseGenerator.setSampleRate(newSampleRate);
    envelope.setSampleRate(newSampleRate);
}

void Voice::noteOn(int midiNote, float velocity, float detune)
{
    currentMidiNote = midiNote;
    unisonDetune = detune;

    // Reset oscillator phases for clean waveform start
    for (auto& osc : oscillators)
    {
        osc.reset();
    }

    // Update all oscillator frequencies with new note
    updateOscillatorFrequencies();

    // Trigger envelope with velocity
    envelope.noteOn(velocity);

    // Reset age for voice stealing
    resetAge();
}

void Voice::noteOff()
{
    // Release envelope (voice continues sounding until release phase completes)
    envelope.noteOff();
}

void Voice::reset()
{
    // Reset all oscillators
    for (auto& osc : oscillators)
    {
        osc.reset();
    }

    // Reset noise generator
    noiseGenerator.reset();

    // Reset envelope to idle state
    envelope.reset();

    // Clear voice state
    currentMidiNote = -1;
    age = 0;
    unisonDetune = 0.0f;
}

//==============================================================================
// Per-Oscillator Parameter Updates
//==============================================================================

void Voice::setOscillatorEnabled(int oscIndex, bool enabled)
{
    if (oscIndex >= 0 && oscIndex < NUM_OSCILLATORS)
    {
        oscSettings[oscIndex].enabled = enabled;
    }
}

void Voice::setOscillatorWaveform(int oscIndex, Oscillator::Waveform waveform)
{
    if (oscIndex >= 0 && oscIndex < NUM_OSCILLATORS)
    {
        oscillators[oscIndex].setWaveform(waveform);
    }
}

void Voice::setOscillatorGain(int oscIndex, float gain)
{
    if (oscIndex >= 0 && oscIndex < NUM_OSCILLATORS)
    {
        oscSettings[oscIndex].gain = AudioUtils::clamp(gain, 0.0f, 1.0f);
    }
}

void Voice::setOscillatorDetune(int oscIndex, float cents)
{
    if (oscIndex >= 0 && oscIndex < NUM_OSCILLATORS)
    {
        oscSettings[oscIndex].detuneCents = AudioUtils::clamp(cents, -100.0f, 100.0f);
        updateOscillatorFrequencies();
    }
}

void Voice::setOscillatorOctave(int oscIndex, int octaveOffset)
{
    if (oscIndex >= 0 && oscIndex < NUM_OSCILLATORS)
    {
        oscSettings[oscIndex].octaveOffset = AudioUtils::clamp(octaveOffset, -3, 3);
        updateOscillatorFrequencies();
    }
}

void Voice::setOscillatorPulseWidth(int oscIndex, float pw)
{
    if (oscIndex >= 0 && oscIndex < NUM_OSCILLATORS)
    {
        oscillators[oscIndex].setPulseWidth(pw);
    }
}

//==============================================================================
// Noise Parameter Updates
//==============================================================================

void Voice::setNoiseEnabled(bool enabled)
{
    noiseEnabled = enabled;
}

void Voice::setNoiseType(NoiseGenerator::NoiseType type)
{
    noiseGenerator.setNoiseType(type);
}

void Voice::setNoiseGain(float gain)
{
    noiseGain = AudioUtils::clamp(gain, 0.0f, 1.0f);
}

//==============================================================================
// Envelope Parameters
//==============================================================================

void Voice::setEnvelopeParameters(float attack, float decay, float sustain, float release)
{
    envelope.setParameters(attack, decay, sustain, release);
}

//==============================================================================
// Audio Processing
//==============================================================================

float Voice::processSample()
{
    if (!isActive())
        return 0.0f;

    // Mix all enabled oscillators + noise (post-mixer architecture)
    float mix = mixOscillators();

    // Apply single envelope to mixed signal
    float envLevel = envelope.processSample();

    // If envelope has finished (idle), mark voice as free
    if (!envelope.isActive())
    {
        currentMidiNote = -1;
    }

    return mix * envLevel;
}

float Voice::mixOscillators()
{
    float sum = 0.0f;

    // Mix all enabled oscillators with their individual gains
    for (int i = 0; i < NUM_OSCILLATORS; ++i)
    {
        if (oscSettings[i].enabled)
        {
            float oscSample = oscillators[i].processSample();
            sum += oscSample * oscSettings[i].gain;
        }
    }

    // Mix noise if enabled (like a 4th oscillator)
    if (noiseEnabled)
    {
        float noiseSample = noiseGenerator.processSample();
        sum += noiseSample * noiseGain;
    }

    return sum;
}

//==============================================================================
// Frequency Calculation
//==============================================================================

void Voice::updateOscillatorFrequencies()
{
    if (currentMidiNote < 0)
        return;

    // Calculate base frequency from MIDI note
    float baseFreq = AudioUtils::midiNoteToFrequency(currentMidiNote);

    // Update each oscillator with its own octave offset and detune
    for (int i = 0; i < NUM_OSCILLATORS; ++i)
    {
        // Apply octave offset (multiply by 2^octave)
        // -3 octaves = ×1/8, +3 octaves = ×8
        float octaveMultiplier = std::pow(2.0f, static_cast<float>(oscSettings[i].octaveOffset));

        // Apply oscillator detune + unison detune (in cents)
        // 1200 cents = 1 octave
        float totalDetuneCents = oscSettings[i].detuneCents + unisonDetune;
        float detuneMultiplier = std::pow(2.0f, totalDetuneCents / 1200.0f);

        // Calculate final frequency
        float finalFreq = baseFreq * octaveMultiplier * detuneMultiplier;

        oscillators[i].setFrequency(finalFreq);
    }
}

//==============================================================================
// Voice State Queries
//==============================================================================

bool Voice::isActive() const
{
    // Voice is active if envelope is not idle
    return envelope.isActive();
}

bool Voice::isSounding() const
{
    // Voice is producing audible output if envelope is active and not in release
    if (!envelope.isActive())
        return false;

    // Voices in release phase are better candidates for stealing
    return envelope.getCurrentPhase() != Envelope::Phase::Release;
}
