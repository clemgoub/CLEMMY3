#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_audio_utils/juce_audio_utils.h>
#include "PluginProcessor.h"

//==============================================================================
/**
    CLEMMY3 Editor
    Phase 3: Voice Polyphony & Management
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

    // UI Components
    juce::ComboBox voiceModeSelector;
    juce::Label voiceModeLabel;

    // ADSR Envelope controls
    juce::Slider attackSlider;
    juce::Label attackLabel;
    juce::Slider decaySlider;
    juce::Label decayLabel;
    juce::Slider sustainSlider;
    juce::Label sustainLabel;
    juce::Slider releaseSlider;
    juce::Label releaseLabel;

    // Virtual MIDI keyboard for computer keyboard input
    juce::MidiKeyboardComponent midiKeyboard;

    // Parameter attachments
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> voiceModeAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> attackAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> decayAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> sustainAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> releaseAttachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(CLEMMY3AudioProcessorEditor)
};
