#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_audio_utils/juce_audio_utils.h>
#include "PluginProcessor.h"

//==============================================================================
/**
    CLEMMY3 Editor
    Phase 6: Dual LFO Modulation
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
    juce::Label voiceModeLabel;
    juce::TextButton voiceModeMonoButton;
    juce::TextButton voiceModePolyButton;
    juce::TextButton voiceModeUnisonButton;
    juce::ComboBox unisonDetuneSelector;
    juce::Label unisonDetuneLabel;

    // ========== OSCILLATOR 1 CONTROLS ==========
    juce::TextButton osc1EnableButton;
    juce::ComboBox osc1WaveformSelector;
    juce::Label osc1WaveformLabel;
    juce::Slider osc1GainSlider;
    juce::Label osc1GainLabel;
    juce::Slider osc1DetuneSlider;
    juce::Label osc1DetuneLabel;
    juce::TextButton osc1OctaveUpButton;
    juce::TextButton osc1OctaveDownButton;
    juce::Label osc1OctaveLabel;
    juce::Label osc1OctaveValueLabel;  // Displays current octave value (-3 to +3)
    juce::Slider osc1PWSlider;
    juce::Label osc1PWLabel;

    // ========== OSCILLATOR 2 CONTROLS ==========
    juce::TextButton osc2EnableButton;
    juce::ComboBox osc2WaveformSelector;
    juce::Label osc2WaveformLabel;
    juce::Slider osc2GainSlider;
    juce::Label osc2GainLabel;
    juce::Slider osc2DetuneSlider;
    juce::Label osc2DetuneLabel;
    juce::TextButton osc2OctaveUpButton;
    juce::TextButton osc2OctaveDownButton;
    juce::Label osc2OctaveLabel;
    juce::Label osc2OctaveValueLabel;  // Displays current octave value (-3 to +3)
    juce::Slider osc2PWSlider;
    juce::Label osc2PWLabel;

    // ========== OSCILLATOR 3 CONTROLS ==========
    juce::TextButton osc3EnableButton;
    juce::ComboBox osc3WaveformSelector;
    juce::Label osc3WaveformLabel;
    juce::Slider osc3GainSlider;
    juce::Label osc3GainLabel;
    juce::Slider osc3DetuneSlider;
    juce::Label osc3DetuneLabel;
    juce::TextButton osc3OctaveUpButton;
    juce::TextButton osc3OctaveDownButton;
    juce::Label osc3OctaveLabel;
    juce::Label osc3OctaveValueLabel;  // Displays current octave value (-3 to +3)
    juce::Slider osc3PWSlider;
    juce::Label osc3PWLabel;

    // ========== MIXER CONTROLS ==========
    juce::Label noiseLabel;
    juce::TextButton noiseEnableButton;
    juce::ComboBox noiseTypeSelector;
    juce::Label noiseTypeLabel;
    juce::Slider noiseGainSlider;
    juce::Label noiseGainLabel;

    // Master Volume
    juce::Slider masterVolumeSlider;
    juce::Label masterVolumeLabel;

    // ========== FILTER CONTROLS ==========
    juce::TextButton filterLowPassButton;
    juce::TextButton filterBandPassButton;
    juce::TextButton filterHighPassButton;
    juce::Slider filterCutoffSlider;
    juce::Label filterCutoffLabel;
    juce::Slider filterResonanceSlider;
    juce::Label filterResonanceLabel;

    // ========== ADSR ENVELOPE ==========
    juce::Slider attackSlider;
    juce::Label attackLabel;
    juce::Slider decaySlider;
    juce::Label decayLabel;
    juce::Slider sustainSlider;
    juce::Label sustainLabel;
    juce::Slider releaseSlider;
    juce::Label releaseLabel;

    // ========== LFO 1 ==========
    juce::Label lfo1Label;
    juce::ComboBox lfo1WaveformSelector;
    juce::Label lfo1WaveformLabel;
    juce::ComboBox lfo1RateModeSelector;
    juce::Label lfo1RateModeLabel;
    juce::Slider lfo1RateSlider;
    juce::Label lfo1RateLabel;
    juce::ComboBox lfo1SyncDivSelector;
    juce::Label lfo1SyncDivLabel;
    juce::Slider lfo1DepthSlider;
    juce::Label lfo1DepthLabel;
    juce::ComboBox lfo1DestinationSelector;
    juce::Label lfo1DestinationLabel;

    // ========== LFO 2 ==========
    juce::Label lfo2Label;
    juce::ComboBox lfo2WaveformSelector;
    juce::Label lfo2WaveformLabel;
    juce::ComboBox lfo2RateModeSelector;
    juce::Label lfo2RateModeLabel;
    juce::Slider lfo2RateSlider;
    juce::Label lfo2RateLabel;
    juce::ComboBox lfo2SyncDivSelector;
    juce::Label lfo2SyncDivLabel;
    juce::Slider lfo2DepthSlider;
    juce::Label lfo2DepthLabel;
    juce::ComboBox lfo2DestinationSelector;
    juce::Label lfo2DestinationLabel;

    // Virtual MIDI keyboard
    juce::MidiKeyboardComponent midiKeyboard;

    // ========== PARAMETER ATTACHMENTS ==========
    // Voice mode - no attachment, uses onClick handlers
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> unisonDetuneAttachment;

    // Oscillator 1 - osc1Enable uses onClick handler
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> osc1WaveformAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> osc1GainAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> osc1DetuneAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> osc1PWAttachment;

    // Oscillator 2 - osc2Enable uses onClick handler
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> osc2WaveformAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> osc2GainAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> osc2DetuneAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> osc2PWAttachment;

    // Oscillator 3 - osc3Enable uses onClick handler
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> osc3WaveformAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> osc3GainAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> osc3DetuneAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> osc3PWAttachment;

    // Noise - noiseEnable uses onClick handler
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> noiseTypeAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> noiseGainAttachment;

    // Master Volume
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> masterVolumeAttachment;

    // Filter
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> filterCutoffAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> filterResonanceAttachment;

    // ADSR
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> attackAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> decayAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> sustainAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> releaseAttachment;

    // LFO 1
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> lfo1WaveformAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> lfo1RateModeAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> lfo1RateAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> lfo1SyncDivAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> lfo1DepthAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> lfo1DestinationAttachment;

    // LFO 2
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> lfo2WaveformAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> lfo2RateModeAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> lfo2RateAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> lfo2SyncDivAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> lfo2DepthAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> lfo2DestinationAttachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(CLEMMY3AudioProcessorEditor)
};
