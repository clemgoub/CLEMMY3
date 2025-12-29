#include "VoiceManager.h"
#include <cmath>

VoiceManager::VoiceManager()
{
}

void VoiceManager::setSampleRate(double sampleRate)
{
    // Broadcast sample rate to all voices
    for (auto& voice : voices)
    {
        voice.setSampleRate(sampleRate);
    }
}

void VoiceManager::setVoiceMode(VoiceMode mode)
{
    // When changing modes, silence all voices to avoid glitches
    if (mode != voiceMode)
    {
        allSoundOff();
    }

    voiceMode = mode;
}

void VoiceManager::noteOn(int midiNote, float velocity)
{
    // Dispatch to appropriate allocation strategy based on mode
    switch (voiceMode)
    {
        case VoiceMode::Mono:
            allocateMonoVoice(midiNote, velocity);
            break;

        case VoiceMode::Poly:
            allocatePolyVoice(midiNote, velocity);
            break;

        case VoiceMode::Unison:
            allocateUnisonVoices(midiNote, velocity);
            break;
    }

    // Increment age of all voices for LRU tracking
    incrementAllAges();
}

void VoiceManager::noteOff(int midiNote)
{
    // Find all voices playing this note and release them
    for (auto& voice : voices)
    {
        if (voice.getCurrentNote() == midiNote && voice.isActive())
        {
            voice.noteOff();
        }
    }
}

void VoiceManager::allNotesOff()
{
    // Send note-off to all active voices (releases envelopes)
    for (auto& voice : voices)
    {
        if (voice.isActive())
        {
            voice.noteOff();
        }
    }
}

void VoiceManager::allSoundOff()
{
    // Immediate silence - reset all voices
    for (auto& voice : voices)
    {
        voice.reset();
    }
}

//==============================================================================
// Per-Oscillator Parameter Broadcasting
//==============================================================================

void VoiceManager::setOscillatorEnabled(int oscIndex, bool enabled)
{
    for (auto& voice : voices)
    {
        voice.setOscillatorEnabled(oscIndex, enabled);
    }
}

void VoiceManager::setOscillatorWaveform(int oscIndex, Oscillator::Waveform waveform)
{
    for (auto& voice : voices)
    {
        voice.setOscillatorWaveform(oscIndex, waveform);
    }
}

void VoiceManager::setOscillatorGain(int oscIndex, float gain)
{
    for (auto& voice : voices)
    {
        voice.setOscillatorGain(oscIndex, gain);
    }
}

void VoiceManager::setOscillatorDetune(int oscIndex, float cents)
{
    for (auto& voice : voices)
    {
        voice.setOscillatorDetune(oscIndex, cents);
    }
}

void VoiceManager::setOscillatorOctave(int oscIndex, int octaveOffset)
{
    for (auto& voice : voices)
    {
        voice.setOscillatorOctave(oscIndex, octaveOffset);
    }
}

void VoiceManager::setOscillatorPulseWidth(int oscIndex, float pw)
{
    for (auto& voice : voices)
    {
        voice.setOscillatorPulseWidth(oscIndex, pw);
    }
}

//==============================================================================
// Noise Parameter Broadcasting
//==============================================================================

void VoiceManager::setNoiseEnabled(bool enabled)
{
    for (auto& voice : voices)
    {
        voice.setNoiseEnabled(enabled);
    }
}

void VoiceManager::setNoiseType(NoiseGenerator::NoiseType type)
{
    for (auto& voice : voices)
    {
        voice.setNoiseType(type);
    }
}

void VoiceManager::setNoiseGain(float gain)
{
    for (auto& voice : voices)
    {
        voice.setNoiseGain(gain);
    }
}

//==============================================================================
// Envelope Parameters
//==============================================================================

void VoiceManager::setEnvelopeParameters(float attack, float decay, float sustain, float release)
{
    // Broadcast to all voices
    for (auto& voice : voices)
    {
        voice.setEnvelopeParameters(attack, decay, sustain, release);
    }
}

//==============================================================================
// Filter Parameters
//==============================================================================

void VoiceManager::setFilterMode(MoogFilter::Mode mode)
{
    for (auto& voice : voices)
    {
        voice.setFilterMode(mode);
    }
}

void VoiceManager::setFilterCutoff(float cutoffHz)
{
    for (auto& voice : voices)
    {
        voice.setFilterCutoff(cutoffHz);
    }
}

void VoiceManager::setFilterResonance(float resonance)
{
    for (auto& voice : voices)
    {
        voice.setFilterResonance(resonance);
    }
}

//==============================================================================
// LFO Parameters
//==============================================================================

void VoiceManager::setLFO1Waveform(LFO::Waveform waveform)
{
    for (auto& voice : voices)
    {
        voice.setLFO1Waveform(waveform);
    }
}

void VoiceManager::setLFO1Rate(float rateHz)
{
    for (auto& voice : voices)
    {
        voice.setLFO1Rate(rateHz);
    }
}

void VoiceManager::setLFO1Depth(float depth)
{
    for (auto& voice : voices)
    {
        voice.setLFO1Depth(depth);
    }
}

void VoiceManager::setLFO1Destination(int dest)
{
    for (auto& voice : voices)
    {
        voice.setLFO1Destination(dest);
    }
}

void VoiceManager::setLFO2Waveform(LFO::Waveform waveform)
{
    for (auto& voice : voices)
    {
        voice.setLFO2Waveform(waveform);
    }
}

void VoiceManager::setLFO2Rate(float rateHz)
{
    for (auto& voice : voices)
    {
        voice.setLFO2Rate(rateHz);
    }
}

void VoiceManager::setLFO2Depth(float depth)
{
    for (auto& voice : voices)
    {
        voice.setLFO2Depth(depth);
    }
}

void VoiceManager::setLFO2Destination(int dest)
{
    for (auto& voice : voices)
    {
        voice.setLFO2Destination(dest);
    }
}

//==============================================================================
// Audio Generation
//==============================================================================

float VoiceManager::processSample()
{
    float output = 0.0f;
    int activeCount = 0;

    // Sum output from all active voices
    // Each voice now handles its own 3 oscillators + noise mixing with envelope
    for (auto& voice : voices)
    {
        if (voice.isActive())
        {
            output += voice.processSample();
            ++activeCount;
        }
    }

    // Apply gain compensation based on mode
    if (activeCount > 0)
    {
        if (voiceMode == VoiceMode::Unison)
        {
            // Unison: Light fixed gain for massive sound
            output /= 2.5f;
        }
        else if (voiceMode == VoiceMode::Poly)
        {
            // Poly: Fixed gain (don't normalize by count to avoid clicks)
            // Professional synths use fixed gain, not dynamic normalization
            output /= 2.0f;
        }
        // Mono: No gain adjustment needed (single voice)
    }

    return output;
}

int VoiceManager::getNumActiveVoices() const
{
    int count = 0;
    for (const auto& voice : voices)
    {
        if (voice.isActive())
        {
            ++count;
        }
    }
    return count;
}

//==============================================================================
// Voice Allocation Helpers
//==============================================================================

Voice* VoiceManager::findFreeVoice()
{
    // Look for a voice that's not active (envelope is idle)
    for (auto& voice : voices)
    {
        if (!voice.isActive())
        {
            return &voice;
        }
    }

    return nullptr;  // No free voices available
}

Voice* VoiceManager::findVoicePlayingNote(int midiNote)
{
    // Find a voice currently playing this MIDI note
    for (auto& voice : voices)
    {
        if (voice.getCurrentNote() == midiNote && voice.isActive())
        {
            return &voice;
        }
    }

    return nullptr;  // Note not currently playing
}

Voice* VoiceManager::stealVoice()
{
    // Least Recently Used (LRU) voice stealing algorithm
    // Prefer voices in release phase, then oldest voice

    Voice* candidate = nullptr;
    int maxAge = -1;

    // First pass: Find oldest voice in release phase
    for (auto& voice : voices)
    {
        if (voice.isActive() && !voice.isSounding() && voice.getAge() > maxAge)
        {
            maxAge = voice.getAge();
            candidate = &voice;
        }
    }

    // Second pass: If no releasing voices, steal oldest active voice
    if (!candidate)
    {
        maxAge = -1;
        for (auto& voice : voices)
        {
            if (voice.isActive() && voice.getAge() > maxAge)
            {
                maxAge = voice.getAge();
                candidate = &voice;
            }
        }
    }

    // Fallback: If still no candidate (shouldn't happen), use first voice
    return candidate ? candidate : &voices[0];
}

//==============================================================================
// Mode-Specific Allocation
//==============================================================================

void VoiceManager::allocateMonoVoice(int midiNote, float velocity)
{
    // MONO mode: Always use the first voice
    // Last note priority - new note retriggers envelope
    voices[0].noteOn(midiNote, velocity, 0.0f);
}

void VoiceManager::allocatePolyVoice(int midiNote, float velocity)
{
    // POLY mode: Polyphonic with voice stealing

    // First, try to find a free voice
    Voice* voice = findFreeVoice();

    // If no free voices, steal one
    if (!voice)
    {
        voice = stealVoice();
        // Note: Don't reset() - let noteOn() handle smooth retriggering
        // The envelope will transition smoothly from current level
    }

    // Trigger the voice (handles both free and stolen voices)
    if (voice)
    {
        voice->noteOn(midiNote, velocity, 0.0f);
    }
}

void VoiceManager::allocateUnisonVoices(int midiNote, float velocity)
{
    // UNISON mode: All voices play the same note, detuned

    // Silence any voices that might be ringing
    allSoundOff();

    // Trigger all voices with calculated detune amounts
    for (int i = 0; i < MAX_VOICES; ++i)
    {
        float detune = calculateUnisonDetune(i);
        voices[i].noteOn(midiNote, velocity, detune);
    }
}

float VoiceManager::calculateUnisonDetune(int voiceIndex) const
{
    // Spread voices across +/- 25 cents
    // Creates thick, chorused sound
    const float MAX_DETUNE_CENTS = 25.0f;

    // Center voice has no detune, others spread symmetrically
    // Example for 8 voices: -25, -17.86, -10.71, -3.57, +3.57, +10.71, +17.86, +25
    float step = (MAX_DETUNE_CENTS * 2.0f) / (MAX_VOICES - 1);
    return -MAX_DETUNE_CENTS + (voiceIndex * step);
}

void VoiceManager::incrementAllAges()
{
    // Increment age counter for all active voices
    // Used for LRU voice stealing algorithm
    for (auto& voice : voices)
    {
        if (voice.isActive())
        {
            voice.incrementAge();
        }
    }
}
