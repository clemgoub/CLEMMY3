#include "PresetManager.h"

PresetManager::PresetManager(juce::AudioProcessorValueTreeState& apvts)
    : parameters(apvts)
{
    loadFactoryPresets();
    scanUserPresets();
}

// ========== PRESET LOADING ==========

void PresetManager::loadPreset(int presetIndex)
{
    if (presetIndex >= 0 && presetIndex < (int)presets.size())
    {
        parameters.replaceState(presets[presetIndex].state.createCopy());
        currentPresetIndex = presetIndex;
    }
}

void PresetManager::loadNextPreset()
{
    int nextIndex = (currentPresetIndex + 1) % presets.size();
    loadPreset(nextIndex);
}

void PresetManager::loadPreviousPreset()
{
    int prevIndex = (currentPresetIndex - 1 + presets.size()) % presets.size();
    loadPreset(prevIndex);
}

// ========== PRESET SAVING ==========

void PresetManager::saveUserPreset(const juce::String& presetName)
{
    auto currentState = parameters.copyState();
    savePresetToFile(presetName, currentState);

    // Reload user presets to include the new one
    scanUserPresets();
}

void PresetManager::deleteUserPreset(int presetIndex)
{
    if (presetIndex >= 0 && presetIndex < (int)presets.size() && !presets[presetIndex].isFactory)
    {
        // Find and delete the file
        auto userDir = getUserPresetDirectory();
        auto fileName = presets[presetIndex].name + ".clemmy3";
        auto file = userDir.getChildFile(fileName);

        if (file.existsAsFile())
        {
            file.deleteFile();
        }

        // Reload presets
        presets.clear();
        loadFactoryPresets();
        scanUserPresets();

        // Adjust current preset index if needed
        if (currentPresetIndex >= (int)presets.size())
        {
            currentPresetIndex = 0;
        }
    }
}

// ========== PRESET INFO ==========

int PresetManager::getNumPresets() const
{
    return (int)presets.size();
}

juce::String PresetManager::getPresetName(int index) const
{
    if (index >= 0 && index < (int)presets.size())
    {
        return presets[index].name;
    }
    return "";
}

bool PresetManager::isFactoryPreset(int index) const
{
    if (index >= 0 && index < (int)presets.size())
    {
        return presets[index].isFactory;
    }
    return false;
}

// ========== FILE OPERATIONS ==========

juce::File PresetManager::getUserPresetDirectory() const
{
    auto userDir = juce::File::getSpecialLocation(juce::File::userDocumentsDirectory)
        .getChildFile("CLEMMY3")
        .getChildFile("Presets");

    if (!userDir.exists())
    {
        userDir.createDirectory();
    }

    return userDir;
}

void PresetManager::savePresetToFile(const juce::String& presetName, const juce::ValueTree& state)
{
    auto userDir = getUserPresetDirectory();
    auto file = userDir.getChildFile(presetName + ".clemmy3");

    // Convert ValueTree to XML
    auto xml = state.createXml();
    if (xml != nullptr)
    {
        xml->writeTo(file);
    }
}

juce::ValueTree PresetManager::loadPresetFromFile(const juce::File& file)
{
    auto xml = juce::XmlDocument::parse(file);
    if (xml != nullptr)
    {
        return juce::ValueTree::fromXml(*xml);
    }
    return juce::ValueTree();
}

void PresetManager::scanUserPresets()
{
    auto userDir = getUserPresetDirectory();

    juce::Array<juce::File> presetFiles;
    userDir.findChildFiles(presetFiles, juce::File::findFiles, false, "*.clemmy3");

    for (const auto& file : presetFiles)
    {
        auto state = loadPresetFromFile(file);
        if (state.isValid())
        {
            auto presetName = file.getFileNameWithoutExtension();
            presets.emplace_back(presetName, state, false);
        }
    }
}

// ========== FACTORY PRESETS ==========

void PresetManager::loadFactoryPresets()
{
    createFactoryPresets();
}

void PresetManager::createFactoryPresets()
{
    // Factory Preset 1: Init (Clean Starting Point)
    {
        std::map<juce::String, float> params;
        params["voiceMode"] = 1;  // Poly (0=Mono, 1=Poly, 2=Unison)
        params["unisonDetune"] = 2;  // 10 cents

        // Osc 1: Sine, enabled
        params["osc1Enabled"] = 1.0f;
        params["osc1Waveform"] = 0;  // Sine
        params["osc1Gain"] = 0.5f;
        params["osc1Detune"] = 0.0f;
        params["osc1Octave"] = 0.5f;  // 0
        params["osc1PW"] = 0.5f;
        params["osc1Drive"] = 1.0f;  // No saturation

        // Osc 2: Disabled
        params["osc2Enabled"] = 0.0f;
        params["osc2Waveform"] = 1;  // Sawtooth
        params["osc2Gain"] = 0.33f;
        params["osc2Detune"] = 0.0f;
        params["osc2Octave"] = 0.5f;
        params["osc2PW"] = 0.5f;
        params["osc2Drive"] = 1.0f;

        // Osc 3: Disabled
        params["osc3Enabled"] = 0.0f;
        params["osc3Waveform"] = 2;  // Square
        params["osc3Gain"] = 0.33f;
        params["osc3Detune"] = 0.0f;
        params["osc3Octave"] = 0.5f;
        params["osc3PW"] = 0.5f;
        params["osc3Drive"] = 1.0f;

        // Noise: Off
        params["noiseEnabled"] = 0.0f;
        params["noiseType"] = 0;
        params["noiseGain"] = 0.0f;

        // Master
        params["masterVolume"] = 0.8f;

        // Filter: LP, 1kHz, no resonance
        params["filterMode"] = 0.0f;  // LP
        params["filterCutoff"] = 1000.0f;
        params["filterResonance"] = 0.0f;

        // ADSR: Medium envelope
        params["attack"] = 0.01f;
        params["decay"] = 0.3f;
        params["sustain"] = 0.7f;
        params["release"] = 0.5f;

        // LFO 1: Off
        params["lfo1Waveform"] = 0;
        params["lfo1RateMode"] = 0;  // Free
        params["lfo1Rate"] = 2.0f;
        params["lfo1SyncDiv"] = 5;  // 1/4
        params["lfo1Depth"] = 0.0f;
        params["lfo1Destination"] = 0;  // None

        // LFO 2: Off
        params["lfo2Waveform"] = 0;
        params["lfo2RateMode"] = 0;
        params["lfo2Rate"] = 5.0f;
        params["lfo2SyncDiv"] = 5;
        params["lfo2Depth"] = 0.0f;
        params["lfo2Destination"] = 0;

        presets.emplace_back("[INIT] Init", createPresetState(params), true);
    }

    // Factory Preset 2: Classic Analog (All 3 oscillators)
    {
        std::map<juce::String, float> params;
        params["voiceMode"] = 1;  // Poly
        params["unisonDetune"] = 2;

        // Osc 1: Sawtooth
        params["osc1Enabled"] = 1.0f;
        params["osc1Waveform"] = 1;  // Sawtooth
        params["osc1Gain"] = 0.4f;
        params["osc1Detune"] = -5.0f;
        params["osc1Octave"] = 0.5f;
        params["osc1PW"] = 0.5f;
        params["osc1Drive"] = 1.0f;

        // Osc 2: Sawtooth, detuned
        params["osc2Enabled"] = 1.0f;
        params["osc2Waveform"] = 1;  // Sawtooth
        params["osc2Gain"] = 0.4f;
        params["osc2Detune"] = 5.0f;
        params["osc2Octave"] = 0.5f;
        params["osc2PW"] = 0.5f;
        params["osc2Drive"] = 1.0f;

        // Osc 3: Square, one octave down
        params["osc3Enabled"] = 1.0f;
        params["osc3Waveform"] = 2;  // Square
        params["osc3Gain"] = 0.3f;
        params["osc3Detune"] = 0.0f;
        params["osc3Octave"] = (0.5f - 1.0f/6.0f);  // -1 octave
        params["osc3PW"] = 0.5f;
        params["osc3Drive"] = 1.0f;

        params["noiseEnabled"] = 0.0f;
        params["noiseType"] = 0;
        params["noiseGain"] = 0.0f;
        params["masterVolume"] = 0.7f;

        // Filter: LP with slight resonance
        params["filterMode"] = 0.0f;
        params["filterCutoff"] = 2500.0f;
        params["filterResonance"] = 0.3f;

        // ADSR: Classic
        params["attack"] = 0.005f;
        params["decay"] = 0.4f;
        params["sustain"] = 0.6f;
        params["release"] = 0.7f;

        // LFO 1: Filter sweep
        params["lfo1Waveform"] = 0;  // Sine
        params["lfo1RateMode"] = 0;
        params["lfo1Rate"] = 3.0f;
        params["lfo1SyncDiv"] = 5;
        params["lfo1Depth"] = 0.5f;
        params["lfo1Destination"] = 1;  // Filter Cutoff

        params["lfo2Waveform"] = 0;
        params["lfo2RateMode"] = 0;
        params["lfo2Rate"] = 5.0f;
        params["lfo2SyncDiv"] = 5;
        params["lfo2Depth"] = 0.0f;
        params["lfo2Destination"] = 0;

        presets.emplace_back("[SYNTH] Classic Analog", createPresetState(params), true);
    }

    // Factory Preset 3: Bass Monster
    {
        std::map<juce::String, float> params;
        params["voiceMode"] = 0;  // Mono
        params["unisonDetune"] = 2;

        // Osc 1: Sawtooth
        params["osc1Enabled"] = 1.0f;
        params["osc1Waveform"] = 1;  // Sawtooth
        params["osc1Gain"] = 0.7f;
        params["osc1Detune"] = 0.0f;
        params["osc1Octave"] = (0.5f - 1.0f/6.0f);  // -1 octave
        params["osc1PW"] = 0.5f;
        params["osc1Drive"] = 1.0f;

        // Osc 2: Square
        params["osc2Enabled"] = 1.0f;
        params["osc2Waveform"] = 2;  // Square
        params["osc2Gain"] = 0.5f;
        params["osc2Detune"] = -3.0f;
        params["osc2Octave"] = (0.5f - 1.0f/6.0f);  // -1 octave
        params["osc2PW"] = 0.3f;
        params["osc2Drive"] = 1.0f;

        // Osc 3: Triangle, sub octave
        params["osc3Enabled"] = 1.0f;
        params["osc3Waveform"] = 3;  // Triangle
        params["osc3Gain"] = 0.6f;
        params["osc3Detune"] = 0.0f;
        params["osc3Octave"] = (0.5f - 2.0f/6.0f);  // -2 octaves
        params["osc3PW"] = 0.5f;
        params["osc3Drive"] = 1.0f;

        params["noiseEnabled"] = 0.0f;
        params["noiseType"] = 0;
        params["noiseGain"] = 0.0f;
        params["masterVolume"] = 0.75f;

        // Filter: LP, low cutoff, high resonance
        params["filterMode"] = 0.0f;
        params["filterCutoff"] = 400.0f;
        params["filterResonance"] = 0.6f;

        // ADSR: Punchy
        params["attack"] = 0.001f;
        params["decay"] = 0.1f;
        params["sustain"] = 0.5f;
        params["release"] = 0.2f;

        // LFO 1: Filter modulation
        params["lfo1Waveform"] = 0;
        params["lfo1RateMode"] = 0;
        params["lfo1Rate"] = 0.5f;
        params["lfo1SyncDiv"] = 5;
        params["lfo1Depth"] = 0.4f;
        params["lfo1Destination"] = 1;  // Filter Cutoff

        params["lfo2Waveform"] = 0;
        params["lfo2RateMode"] = 0;
        params["lfo2Rate"] = 5.0f;
        params["lfo2SyncDiv"] = 5;
        params["lfo2Depth"] = 0.0f;
        params["lfo2Destination"] = 0;

        presets.emplace_back("[BASS] Bass Monster", createPresetState(params), true);
    }

    // Factory Preset 4: Lush Pad
    {
        std::map<juce::String, float> params;
        params["voiceMode"] = 1;  // Poly
        params["unisonDetune"] = 4;  // 15 cents (unused in Poly mode)

        // Osc 1: Sawtooth
        params["osc1Enabled"] = 1.0f;
        params["osc1Waveform"] = 1;
        params["osc1Gain"] = 0.4f;
        params["osc1Detune"] = 0.0f;
        params["osc1Octave"] = 0.5f;
        params["osc1PW"] = 0.5f;
        params["osc1Drive"] = 1.0f;

        // Osc 2: Triangle
        params["osc2Enabled"] = 1.0f;
        params["osc2Waveform"] = 3;
        params["osc2Gain"] = 0.3f;
        params["osc2Detune"] = 7.0f;
        params["osc2Octave"] = 0.5f;
        params["osc2PW"] = 0.5f;
        params["osc2Drive"] = 1.0f;

        // Osc 3: Square with PWM
        params["osc3Enabled"] = 1.0f;
        params["osc3Waveform"] = 2;
        params["osc3Gain"] = 0.25f;
        params["osc3Detune"] = -7.0f;
        params["osc3Octave"] = (0.5f + 1.0f/6.0f);  // +1 octave
        params["osc3PW"] = 0.5f;
        params["osc3Drive"] = 1.0f;

        params["noiseEnabled"] = 0.0f;
        params["noiseType"] = 0;
        params["noiseGain"] = 0.0f;
        params["masterVolume"] = 0.6f;

        // Filter: LP, open
        params["filterMode"] = 0.0f;
        params["filterCutoff"] = 3500.0f;
        params["filterResonance"] = 0.2f;

        // ADSR: Slow attack, long release
        params["attack"] = 0.8f;
        params["decay"] = 0.5f;
        params["sustain"] = 0.8f;
        params["release"] = 1.5f;

        // LFO 1: PWM on osc3
        params["lfo1Waveform"] = 0;
        params["lfo1RateMode"] = 0;
        params["lfo1Rate"] = 0.3f;
        params["lfo1SyncDiv"] = 5;
        params["lfo1Depth"] = 0.6f;
        params["lfo1Destination"] = 3;  // PWM

        // LFO 2: Gentle vibrato
        params["lfo2Waveform"] = 0;
        params["lfo2RateMode"] = 0;
        params["lfo2Rate"] = 4.5f;
        params["lfo2SyncDiv"] = 5;
        params["lfo2Depth"] = 0.15f;
        params["lfo2Destination"] = 2;  // Pitch

        presets.emplace_back("[PAD] Lush Pad", createPresetState(params), true);
    }

    // Factory Preset 5: Lead Synth
    {
        std::map<juce::String, float> params;
        params["voiceMode"] = 0;  // Mono
        params["unisonDetune"] = 2;

        // Osc 1: Sawtooth
        params["osc1Enabled"] = 1.0f;
        params["osc1Waveform"] = 1;
        params["osc1Gain"] = 0.6f;
        params["osc1Detune"] = 0.0f;
        params["osc1Octave"] = 0.5f;
        params["osc1PW"] = 0.5f;

        // Osc 2: Square
        params["osc2Enabled"] = 1.0f;
        params["osc2Waveform"] = 2;
        params["osc2Gain"] = 0.4f;
        params["osc2Detune"] = -12.0f;
        params["osc2Octave"] = 0.5f;
        params["osc2PW"] = 0.5f;

        // Osc 3: Off
        params["osc3Enabled"] = 0.0f;
        params["osc3Waveform"] = 0;
        params["osc3Gain"] = 0.33f;
        params["osc3Detune"] = 0.0f;
        params["osc3Octave"] = 0.5f;
        params["osc3PW"] = 0.5f;

        // Drive/Saturation
        params["osc1Drive"] = 1.0f;
        params["osc2Drive"] = 1.0f;
        params["osc3Drive"] = 1.0f;

        params["noiseEnabled"] = 0.0f;
        params["noiseType"] = 0;
        params["noiseGain"] = 0.0f;
        params["masterVolume"] = 0.75f;

        // Filter: LP, bright
        params["filterMode"] = 0.0f;
        params["filterCutoff"] = 4000.0f;
        params["filterResonance"] = 0.4f;

        // ADSR: Fast attack, medium release
        params["attack"] = 0.005f;
        params["decay"] = 0.2f;
        params["sustain"] = 0.7f;
        params["release"] = 0.3f;

        // LFO 1: Vibrato
        params["lfo1Waveform"] = 0;
        params["lfo1RateMode"] = 0;
        params["lfo1Rate"] = 5.5f;
        params["lfo1SyncDiv"] = 5;
        params["lfo1Depth"] = 0.3f;
        params["lfo1Destination"] = 2;  // Pitch

        params["lfo2Waveform"] = 0;
        params["lfo2RateMode"] = 0;
        params["lfo2Rate"] = 5.0f;
        params["lfo2SyncDiv"] = 5;
        params["lfo2Depth"] = 0.0f;
        params["lfo2Destination"] = 0;

        presets.emplace_back("[LEAD] Lead Synth", createPresetState(params), true);
    }

    // Factory Preset 6: Pluck
    {
        std::map<juce::String, float> params;
        params["voiceMode"] = 1;  // Poly
        params["unisonDetune"] = 2;

        // Osc 1: Triangle
        params["osc1Enabled"] = 1.0f;
        params["osc1Waveform"] = 3;
        params["osc1Gain"] = 0.8f;
        params["osc1Detune"] = 0.0f;
        params["osc1Octave"] = 0.5f;
        params["osc1PW"] = 0.5f;

        // Osc 2: Square, one octave up
        params["osc2Enabled"] = 1.0f;
        params["osc2Waveform"] = 2;
        params["osc2Gain"] = 0.3f;
        params["osc2Detune"] = 0.0f;
        params["osc2Octave"] = (0.5f + 1.0f/6.0f);  // +1 octave
        params["osc2PW"] = 0.5f;

        // Osc 3: Off
        params["osc3Enabled"] = 0.0f;
        params["osc3Waveform"] = 0;
        params["osc3Gain"] = 0.33f;
        params["osc3Detune"] = 0.0f;
        params["osc3Octave"] = 0.5f;
        params["osc3PW"] = 0.5f;

        // Drive/Saturation
        params["osc1Drive"] = 1.0f;
        params["osc2Drive"] = 1.0f;
        params["osc3Drive"] = 1.0f;

        params["noiseEnabled"] = 0.0f;
        params["noiseType"] = 0;
        params["noiseGain"] = 0.0f;
        params["masterVolume"] = 0.8f;

        // Filter: LP, medium cutoff
        params["filterMode"] = 0.0f;
        params["filterCutoff"] = 2000.0f;
        params["filterResonance"] = 0.1f;

        // ADSR: Very fast attack, short decay
        params["attack"] = 0.001f;
        params["decay"] = 0.05f;
        params["sustain"] = 0.0f;
        params["release"] = 0.1f;

        params["lfo1Waveform"] = 0;
        params["lfo1RateMode"] = 0;
        params["lfo1Rate"] = 2.0f;
        params["lfo1SyncDiv"] = 5;
        params["lfo1Depth"] = 0.0f;
        params["lfo1Destination"] = 0;

        params["lfo2Waveform"] = 0;
        params["lfo2RateMode"] = 0;
        params["lfo2Rate"] = 5.0f;
        params["lfo2SyncDiv"] = 5;
        params["lfo2Depth"] = 0.0f;
        params["lfo2Destination"] = 0;

        presets.emplace_back("[SYNTH] Pluck", createPresetState(params), true);
    }

    // Factory Preset 7: Highway 1 (Lead)
    {
        std::map<juce::String, float> params;
        params["voiceMode"] = 1;  // Poly
        params["unisonDetune"] = 2;

        // Osc 1: Sawtooth, -5 cents detune
        params["osc1Enabled"] = 1.0f;
        params["osc1Waveform"] = 1;  // Sawtooth
        params["osc1Gain"] = 0.4f;
        params["osc1Detune"] = -5.0f;
        params["osc1Octave"] = 0.5f;  // 0 octaves
        params["osc1PW"] = 0.5f;

        // Osc 2: Sawtooth, +5 cents detune
        params["osc2Enabled"] = 1.0f;
        params["osc2Waveform"] = 1;  // Sawtooth
        params["osc2Gain"] = 0.4f;
        params["osc2Detune"] = 5.0f;
        params["osc2Octave"] = 0.5f;  // 0 octaves
        params["osc2PW"] = 0.5f;

        // Osc 3: Square, -1 octave
        params["osc3Enabled"] = 1.0f;
        params["osc3Waveform"] = 2;  // Square
        params["osc3Gain"] = 0.3f;
        params["osc3Detune"] = 0.0f;
        params["osc3Octave"] = 0.3333333f;  // -1 octave
        params["osc3PW"] = 0.5f;

        // Drive/Saturation
        params["osc1Drive"] = 1.0f;
        params["osc2Drive"] = 1.0f;
        params["osc3Drive"] = 1.0f;

        params["noiseEnabled"] = 0.0f;
        params["noiseType"] = 0;
        params["noiseGain"] = 0.0f;
        params["masterVolume"] = 0.87f;

        // Filter: LP, 3902 Hz, resonance 0.3
        params["filterMode"] = 0.0f;
        params["filterCutoff"] = 3902.6f;
        params["filterResonance"] = 0.3f;

        // ADSR: Fast attack, medium decay/sustain/release
        params["attack"] = 0.002f;
        params["decay"] = 0.885f;
        params["sustain"] = 0.76f;
        params["release"] = 0.296f;

        // LFO 1: Sine to Filter Cutoff, MIDI sync
        params["lfo1Waveform"] = 0;  // Sine
        params["lfo1RateMode"] = 1;  // MIDI sync
        params["lfo1Rate"] = 3.0f;
        params["lfo1SyncDiv"] = 5;
        params["lfo1Depth"] = 0.5f;
        params["lfo1Destination"] = 1;  // Filter Cutoff

        // LFO 2: Off
        params["lfo2Waveform"] = 0;
        params["lfo2RateMode"] = 1;
        params["lfo2Rate"] = 5.0f;
        params["lfo2SyncDiv"] = 5;
        params["lfo2Depth"] = 0.0f;
        params["lfo2Destination"] = 0;

        presets.emplace_back("[LEAD] Highway 1", createPresetState(params), true);
    }

    // Factory Preset 8: Whimsical Pad
    {
        std::map<juce::String, float> params;
        params["voiceMode"] = 1;  // Poly
        params["unisonDetune"] = 2;

        // Osc 1: Square, -1 octave, PWM 39%
        params["osc1Enabled"] = 1.0f;
        params["osc1Waveform"] = 2;  // Square
        params["osc1Gain"] = 0.43f;
        params["osc1Detune"] = 0.3f;
        params["osc1Octave"] = -1.0f;  // -1 octave (raw value)
        params["osc1PW"] = 0.39f;

        // Osc 2: Off
        params["osc2Enabled"] = 0.0f;
        params["osc2Waveform"] = 1;  // Sawtooth
        params["osc2Gain"] = 0.37f;
        params["osc2Detune"] = -5.7f;
        params["osc2Octave"] = 0.5f;
        params["osc2PW"] = 0.5f;

        // Osc 3: Off
        params["osc3Enabled"] = 0.0f;
        params["osc3Waveform"] = 3;  // Triangle
        params["osc3Gain"] = 0.32f;
        params["osc3Detune"] = 2.9f;
        params["osc3Octave"] = 0.0f;  // -3 octaves (raw value)
        params["osc3PW"] = 0.5f;

        // Drive/Saturation (missing in original, adding defaults)
        params["osc1Drive"] = 1.0f;
        params["osc2Drive"] = 1.0f;
        params["osc3Drive"] = 1.0f;

        params["noiseEnabled"] = 0.0f;
        params["noiseType"] = 0;
        params["noiseGain"] = 0.0f;
        params["masterVolume"] = 0.8f;

        // Filter: LP, wide open at 12kHz, low resonance
        params["filterMode"] = 0.0f;
        params["filterCutoff"] = 12000.0f;
        params["filterResonance"] = 0.2f;

        // ADSR: Slow attack, long release (pad character)
        params["attack"] = 0.283f;
        params["decay"] = 0.746f;
        params["sustain"] = 0.74f;
        params["release"] = 1.865f;

        // LFO 1: Triangle to Pitch (vibrato), slow rate
        params["lfo1Waveform"] = 1;  // Triangle
        params["lfo1RateMode"] = 0;  // Free-running
        params["lfo1Rate"] = 0.71f;
        params["lfo1SyncDiv"] = 5;
        params["lfo1Depth"] = 0.14f;
        params["lfo1Destination"] = 2;  // Pitch

        // LFO 2: Triangle to Filter Cutoff, very slow
        params["lfo2Waveform"] = 1;  // Triangle
        params["lfo2RateMode"] = 0;  // Free-running
        params["lfo2Rate"] = 0.1f;
        params["lfo2SyncDiv"] = 5;
        params["lfo2Depth"] = 0.35f;
        params["lfo2Destination"] = 1;  // Filter Cutoff

        presets.emplace_back("[PAD] Whimsical Pad", createPresetState(params), true);
    }

    // Factory Preset 9: SuperSaw I (Heavy saturation lead)
    {
        std::map<juce::String, float> params;
        params["voiceMode"] = 2;  // Unison
        params["unisonDetune"] = 6;

        // Osc 1: Triangle, -1 octave
        params["osc1Enabled"] = 1.0f;
        params["osc1Waveform"] = 3;  // Triangle
        params["osc1Gain"] = 0.4f;
        params["osc1Detune"] = 0.0f;
        params["osc1Octave"] = -1.0f;  // -1 octave (raw value)
        params["osc1PW"] = 0.5f;

        // Osc 2: Sawtooth, 0 octaves, -1.9 cents detune
        params["osc2Enabled"] = 1.0f;
        params["osc2Waveform"] = 1;  // Sawtooth
        params["osc2Gain"] = 0.25f;
        params["osc2Detune"] = -1.9f;
        params["osc2Octave"] = 0.0f;  // 0 octaves (raw value)
        params["osc2PW"] = 0.5f;

        // Osc 3: Sawtooth, +1 octave, +0.8 cents detune
        params["osc3Enabled"] = 1.0f;
        params["osc3Waveform"] = 1;  // Sawtooth
        params["osc3Gain"] = 0.41f;
        params["osc3Detune"] = 0.8f;
        params["osc3Octave"] = 0.5f;  // +1 octave (raw value)
        params["osc3PW"] = 0.5f;

        // Drive/Saturation - HEAVY SATURATION!
        params["osc1Drive"] = 10.0f;
        params["osc2Drive"] = 10.0f;
        params["osc3Drive"] = 10.0f;

        params["noiseEnabled"] = 0.0f;
        params["noiseType"] = 0;
        params["noiseGain"] = 0.0f;
        params["masterVolume"] = 0.75f;

        // Filter: LP, bright 9589 Hz, high resonance
        params["filterMode"] = 0.0f;
        params["filterCutoff"] = 9589.2f;
        params["filterResonance"] = 0.47f;

        // ADSR: Very fast attack, medium decay/sustain, medium release
        params["attack"] = 0.001f;
        params["decay"] = 0.679f;
        params["sustain"] = 0.81f;
        params["release"] = 0.75f;

        // LFO 1: Square to Volume (tremolo), MIDI sync
        params["lfo1Waveform"] = 2;  // Square
        params["lfo1RateMode"] = 1;  // MIDI sync
        params["lfo1Rate"] = 8.6f;
        params["lfo1SyncDiv"] = 3;
        params["lfo1Depth"] = 0.78f;
        params["lfo1Destination"] = 5;  // Volume

        // LFO 2: Off
        params["lfo2Waveform"] = 3;  // Sawtooth
        params["lfo2RateMode"] = 1;
        params["lfo2Rate"] = 6.96f;
        params["lfo2SyncDiv"] = 5;
        params["lfo2Depth"] = 0.0f;
        params["lfo2Destination"] = 0;

        presets.emplace_back("[LEAD] SuperSaw I", createPresetState(params), true);
    }
}

juce::ValueTree PresetManager::createPresetState(const std::map<juce::String, float>& paramValues)
{
    // Get current state as template
    auto state = parameters.copyState();

    // Update with preset values
    for (const auto& [paramID, value] : paramValues)
    {
        auto* param = parameters.getParameter(paramID);
        if (param != nullptr)
        {
            // Find the parameter in the state tree
            for (int i = 0; i < state.getNumChildren(); ++i)
            {
                auto child = state.getChild(i);
                if (child.hasProperty("id") && child.getProperty("id").toString() == paramID)
                {
                    child.setProperty("value", value, nullptr);
                    break;
                }
            }
        }
    }

    return state;
}
