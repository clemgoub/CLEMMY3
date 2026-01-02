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
    filter.setSampleRate(newSampleRate);
    envelope.setSampleRate(newSampleRate);
    lfo1.setSampleRate(newSampleRate);
    lfo2.setSampleRate(newSampleRate);
}

void Voice::noteOn(int midiNote, float velocity, float detune, bool randomizePhase)
{
    currentMidiNote = midiNote;
    unisonDetune = detune;

    // Reset or randomize oscillator phases
    for (auto& osc : oscillators)
    {
        if (randomizePhase)
        {
            // Random phase for unison mode - prevents phaser effect
            osc.setRandomPhase();
        }
        else
        {
            // Clean phase reset for mono/poly modes
            osc.reset();
        }
    }

    // Update all oscillator frequencies with new note
    updateOscillatorFrequencies();

    // Trigger envelope with velocity
    envelope.noteOn(velocity);

    // Reset LFO phases for note-synchronized modulation
    lfo1.reset();
    lfo2.reset();

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

    // Reset filter state
    filter.reset();

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

void Voice::setOscillatorDrive(int oscIndex, float drive)
{
    if (oscIndex >= 0 && oscIndex < NUM_OSCILLATORS)
    {
        oscSettings[oscIndex].drive = drive;
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
// Filter Parameters
//==============================================================================

void Voice::setFilterMode(MoogFilter::Mode mode)
{
    filter.setMode(mode);
}

void Voice::setFilterCutoff(float cutoffHz)
{
    baseFilterCutoff = cutoffHz;  // Store base value for modulation
    filter.setCutoff(cutoffHz);
}

void Voice::setFilterResonance(float resonance)
{
    baseFilterResonance = resonance;  // Store base value for modulation
    filter.setResonance(resonance);
}

//==============================================================================
// LFO Parameters
//==============================================================================

void Voice::setLFO1Waveform(LFO::Waveform waveform)
{
    lfo1.setWaveform(waveform);
}

void Voice::setLFO1Rate(float rateHz)
{
    lfo1.setRate(rateHz);
}

void Voice::setLFO1Depth(float depth)
{
    lfo1.setDepth(depth);
}

void Voice::setLFO1Destination(int dest)
{
    lfo1Destination = static_cast<ModDestination>(dest);
}

void Voice::setLFO2Waveform(LFO::Waveform waveform)
{
    lfo2.setWaveform(waveform);
}

void Voice::setLFO2Rate(float rateHz)
{
    lfo2.setRate(rateHz);
}

void Voice::setLFO2Depth(float depth)
{
    lfo2.setDepth(depth);
}

void Voice::setLFO2Destination(int dest)
{
    lfo2Destination = static_cast<ModDestination>(dest);
}

void Voice::setLFO1RateMode(LFO::RateMode mode)
{
    lfo1.setRateMode(mode);
}

void Voice::setLFO1SyncDivision(LFO::SyncDivision division)
{
    lfo1.setSyncDivision(division);
}

void Voice::setLFO1BPM(float bpm)
{
    lfo1.setBPM(bpm);
}

void Voice::setLFO2RateMode(LFO::RateMode mode)
{
    lfo2.setRateMode(mode);
}

void Voice::setLFO2SyncDivision(LFO::SyncDivision division)
{
    lfo2.setSyncDivision(division);
}

void Voice::setLFO2BPM(float bpm)
{
    lfo2.setBPM(bpm);
}

//==============================================================================
// Audio Processing
//==============================================================================

float Voice::processSample()
{
    if (!isActive())
        return 0.0f;

    // Signal chain: LFOs → Modulation → Oscillators → Mix → Filter → Envelope → Volume Mod → Output

    // 1. Process LFOs and get modulation values
    float lfo1Value = lfo1.processSample();  // -1 to +1, scaled by depth
    float lfo2Value = lfo2.processSample();

    // 2. Apply modulation to parameters

    // --- LFO1 Modulation ---
    if (lfo1Destination == ModFilterCutoff)
    {
        // Modulate filter cutoff (±2 octaves range)
        float modAmount = lfo1Value * baseFilterCutoff * 2.0f;
        filter.setCutoff(std::clamp(baseFilterCutoff + modAmount, 20.0f, 12000.0f));
    }
    else if (lfo1Destination == ModFilterRes)
    {
        // Modulate filter resonance
        float modAmount = lfo1Value * 0.5f;
        filter.setResonance(std::clamp(baseFilterResonance + modAmount, 0.0f, 1.0f));
    }
    else if (lfo1Destination == ModPitch)
    {
        // Modulate pitch (vibrato) - ±1 semitone range
        float pitchModCents = lfo1Value * 100.0f;  // ±100 cents = ±1 semitone
        for (int i = 0; i < NUM_OSCILLATORS; ++i)
        {
            if (oscSettings[i].enabled && currentMidiNote >= 0)
            {
                // Calculate base frequency with octave and detune
                float baseFreq = AudioUtils::midiNoteToFrequency(currentMidiNote);
                float octaveMultiplier = std::pow(2.0f, static_cast<float>(oscSettings[i].octaveOffset));
                float totalDetuneCents = oscSettings[i].detuneCents + unisonDetune + pitchModCents;
                float detuneMultiplier = std::pow(2.0f, totalDetuneCents / 1200.0f);
                float finalFreq = baseFreq * octaveMultiplier * detuneMultiplier;
                oscillators[i].setFrequency(finalFreq);
            }
        }
    }
    else if (lfo1Destination == ModPWM)
    {
        // Modulate pulse width - oscillate around 50% (0.25 to 0.75 range)
        float pwMod = 0.5f + (lfo1Value * 0.25f);  // 0.25 to 0.75
        for (int i = 0; i < NUM_OSCILLATORS; ++i)
        {
            if (oscSettings[i].enabled)
            {
                oscillators[i].setPulseWidth(std::clamp(pwMod, 0.01f, 0.99f));
            }
        }
    }
    else
    {
        // Reset filter parameters if not being modulated
        filter.setCutoff(baseFilterCutoff);
        filter.setResonance(baseFilterResonance);
    }

    // --- LFO2 Modulation ---
    if (lfo2Destination == ModFilterCutoff)
    {
        float modAmount = lfo2Value * baseFilterCutoff * 2.0f;
        filter.setCutoff(std::clamp(baseFilterCutoff + modAmount, 20.0f, 12000.0f));
    }
    else if (lfo2Destination == ModFilterRes)
    {
        float modAmount = lfo2Value * 0.5f;
        filter.setResonance(std::clamp(baseFilterResonance + modAmount, 0.0f, 1.0f));
    }
    else if (lfo2Destination == ModPitch)
    {
        // Modulate pitch (vibrato) - ±1 semitone range
        float pitchModCents = lfo2Value * 100.0f;  // ±100 cents = ±1 semitone
        for (int i = 0; i < NUM_OSCILLATORS; ++i)
        {
            if (oscSettings[i].enabled && currentMidiNote >= 0)
            {
                // Calculate base frequency with octave and detune
                float baseFreq = AudioUtils::midiNoteToFrequency(currentMidiNote);
                float octaveMultiplier = std::pow(2.0f, static_cast<float>(oscSettings[i].octaveOffset));
                float totalDetuneCents = oscSettings[i].detuneCents + unisonDetune + pitchModCents;
                float detuneMultiplier = std::pow(2.0f, totalDetuneCents / 1200.0f);
                float finalFreq = baseFreq * octaveMultiplier * detuneMultiplier;
                oscillators[i].setFrequency(finalFreq);
            }
        }
    }
    else if (lfo2Destination == ModPWM)
    {
        // Modulate pulse width - oscillate around 50% (0.25 to 0.75 range)
        float pwMod = 0.5f + (lfo2Value * 0.25f);  // 0.25 to 0.75
        for (int i = 0; i < NUM_OSCILLATORS; ++i)
        {
            if (oscSettings[i].enabled)
            {
                oscillators[i].setPulseWidth(std::clamp(pwMod, 0.01f, 0.99f));
            }
        }
    }
    else if (lfo2Destination != ModVolume && lfo1Destination != ModFilterCutoff && lfo1Destination != ModFilterRes)
    {
        // Reset filter parameters if neither LFO is modulating them
        filter.setCutoff(baseFilterCutoff);
        filter.setResonance(baseFilterResonance);
    }

    // 3. Mix all enabled oscillators + noise
    float mix = mixOscillators();

    // 4. Apply filter to mixed signal
    float filtered = filter.processSample(mix);

    // 5. Apply envelope to filtered signal
    float envLevel = envelope.processSample();

    // If envelope has finished (idle), mark voice as free
    if (!envelope.isActive())
    {
        currentMidiNote = -1;
    }

    float output = filtered * envLevel;

    // 6. Apply volume modulation (tremolo) if selected
    if (lfo1Destination == ModVolume)
    {
        // Tremolo: oscillate volume between 0.5 and 1.0 (never silent)
        float volumeMod = 0.75f + (lfo1Value * 0.25f);  // 0.5 to 1.0
        output *= volumeMod;
    }
    if (lfo2Destination == ModVolume)
    {
        // Tremolo: oscillate volume between 0.5 and 1.0 (never silent)
        float volumeMod = 0.75f + (lfo2Value * 0.25f);  // 0.5 to 1.0
        output *= volumeMod;
    }

    return output;
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

            // Apply tanh saturation/drive (1.0 = bypass, >1.0 = saturation)
            if (oscSettings[i].drive > 1.01f)  // Small threshold for floating point precision
            {
                // Soft saturation: tanh adds warm harmonics and compression
                // Volume drops slightly at high drive (expected behavior)
                oscSample = std::tanh(oscSample * oscSettings[i].drive);
            }

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
