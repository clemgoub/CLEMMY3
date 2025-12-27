#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_gui_basics/juce_gui_basics.h>
#include "PluginProcessor.h"

//==============================================================================
/**
    CLEMMY3 Editor
    Phase 0: Simple placeholder UI
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

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(CLEMMY3AudioProcessorEditor)
};
