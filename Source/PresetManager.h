#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_data_structures/juce_data_structures.h>

/**
 * PresetManager
 * Handles saving, loading, and managing presets for CLEMMY3
 * Supports both factory (read-only) and user (read-write) presets
 */
class PresetManager
{
public:
    PresetManager(juce::AudioProcessorValueTreeState& apvts);
    ~PresetManager() = default;

    // Preset loading
    void loadPreset(int presetIndex);
    void loadNextPreset();
    void loadPreviousPreset();

    // Preset saving (user presets only)
    void saveUserPreset(const juce::String& presetName);
    void deleteUserPreset(int presetIndex);

    // Preset info
    int getNumPresets() const;
    juce::String getPresetName(int index) const;
    int getCurrentPresetIndex() const { return currentPresetIndex; }
    bool isFactoryPreset(int index) const;

    // Initialization
    void loadFactoryPresets();
    void scanUserPresets();

private:
    struct Preset
    {
        juce::String name;
        juce::ValueTree state;
        bool isFactory;

        Preset(const juce::String& n, const juce::ValueTree& s, bool factory)
            : name(n), state(s), isFactory(factory) {}
    };

    juce::AudioProcessorValueTreeState& parameters;
    std::vector<Preset> presets;
    int currentPresetIndex = 0;

    // File operations
    juce::File getUserPresetDirectory() const;
    void savePresetToFile(const juce::String& presetName, const juce::ValueTree& state);
    juce::ValueTree loadPresetFromFile(const juce::File& file);

    // Factory preset creation
    void createFactoryPresets();
    juce::ValueTree createPresetState(const std::map<juce::String, float>& paramValues);
};
