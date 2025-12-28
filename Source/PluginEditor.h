#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_audio_utils/juce_audio_utils.h>
#include "PluginProcessor.h"

//==============================================================================
/**
    CLEMMY3 Editor
    Phase 4: Triple Oscillator Architecture with Full GUI
*/
class CLEMMY3AudioProcessorEditor : public juce::AudioProcessorEditor
{
public:
    CLEMMY3AudioProcessorEditor(CLEMMY3AudioProcessor&);
    ~CLEMMY3AudioProcessorEditor() override;

    //==============================================================================
    void paint(juce::Graphics&) override;
    void resized() override;

private:
    CLEMMY3AudioProcessor& audioProcessor;

    // Voice Mode
    juce::ComboBox voiceModeSelector;
    juce::Label voiceModeLabel;

    // ========== OSCILLATOR 1 CONTROLS ==========
    juce::ToggleButton osc1EnableButton;
    juce::ComboBox osc1WaveformSelector;
    juce::Label osc1WaveformLabel;
    juce::Slider osc1GainSlider;
    juce::Label osc1GainLabel;
    juce::Slider osc1DetuneSlider;
    juce::Label osc1DetuneLabel;
    juce::TextButton osc1OctaveUpButton;
    juce::TextButton osc1OctaveDownButton;
    juce::Label osc1OctaveLabel;
    juce::Slider osc1PWSlider;
    juce::Label osc1PWLabel;

    // ========== OSCILLATOR 2 CONTROLS ==========
    juce::ToggleButton osc2EnableButton;
    juce::ComboBox osc2WaveformSelector;
    juce::Label osc2WaveformLabel;
    juce::Slider osc2GainSlider;
    juce::Label osc2GainLabel;
    juce::Slider osc2DetuneSlider;
    juce::Label osc2DetuneLabel;
    juce::TextButton osc2OctaveUpButton;
    juce::TextButton osc2OctaveDownButton;
    juce::Label osc2OctaveLabel;
    juce::Slider osc2PWSlider;
    juce::Label osc2PWLabel;

    // ========== OSCILLATOR 3 CONTROLS ==========
    juce::ToggleButton osc3EnableButton;
    juce::ComboBox osc3WaveformSelector;
    juce::Label osc3WaveformLabel;
    juce::Slider osc3GainSlider;
    juce::Label osc3GainLabel;
    juce::Slider osc3DetuneSlider;
    juce::Label osc3DetuneLabel;
    juce::TextButton osc3OctaveUpButton;
    juce::TextButton osc3OctaveDownButton;
    juce::Label osc3OctaveLabel;
    juce::Slider osc3PWSlider;
    juce::Label osc3PWLabel;

    // ========== MIXER CONTROLS ==========
    juce::ToggleButton noiseEnableButton;
    juce::ComboBox noiseTypeSelector;
    juce::Label noiseTypeLabel;
    juce::Slider noiseGainSlider;
    juce::Label noiseGainLabel;

    // Master Volume
    juce::Slider masterVolumeSlider;
    juce::Label masterVolumeLabel;

    // ========== ADSR ENVELOPE ==========
    juce::Slider attackSlider;
    juce::Label attackLabel;
    juce::Slider decaySlider;
    juce::Label decayLabel;
    juce::Slider sustainSlider;
    juce::Label sustainLabel;
    juce::Slider releaseSlider;
    juce::Label releaseLabel;

    // Virtual MIDI keyboard
    juce::MidiKeyboardComponent midiKeyboard;

    // ========== PARAMETER ATTACHMENTS ==========
    // Voice mode
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> voiceModeAttachment;

    // Oscillator 1
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> osc1EnableAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> osc1WaveformAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> osc1GainAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> osc1DetuneAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> osc1PWAttachment;

    // Oscillator 2
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> osc2EnableAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> osc2WaveformAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> osc2GainAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> osc2DetuneAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> osc2PWAttachment;

    // Oscillator 3
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> osc3EnableAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> osc3WaveformAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> osc3GainAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> osc3DetuneAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> osc3PWAttachment;

    // Noise
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> noiseEnableAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> noiseTypeAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> noiseGainAttachment;

    // Master Volume
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> masterVolumeAttachment;

    // ADSR
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> attackAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> decayAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> sustainAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> releaseAttachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(CLEMMY3AudioProcessorEditor)
};
