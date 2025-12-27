#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
CLEMMY3AudioProcessorEditor::CLEMMY3AudioProcessorEditor(CLEMMY3AudioProcessor& p)
    : AudioProcessorEditor(&p), audioProcessor(p)
{
    // Set editor size
    setSize(400, 300);
}

CLEMMY3AudioProcessorEditor::~CLEMMY3AudioProcessorEditor()
{
}

//==============================================================================
void CLEMMY3AudioProcessorEditor::paint(juce::Graphics& g)
{
    // Fill background
    g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));

    // Draw title
    g.setColour(juce::Colours::white);
    g.setFont(28.0f);
    g.drawFittedText("CLEMMY3", getLocalBounds(), juce::Justification::centred, 1);

    // Draw subtitle
    g.setFont(14.0f);
    auto bounds = getLocalBounds();
    bounds.removeFromTop(bounds.getHeight() / 2 + 20);
    g.drawFittedText("Triple Oscillator Synthesizer", bounds, juce::Justification::centred, 1);

    // Draw phase info
    g.setFont(12.0f);
    bounds.removeFromTop(30);
    g.setColour(juce::Colours::lightgrey);
    g.drawFittedText("Phase 0: Test Implementation", bounds, juce::Justification::centred, 1);
    bounds.removeFromTop(20);
    g.drawFittedText("Play MIDI note to hear 440Hz sine wave", bounds, juce::Justification::centred, 1);
}

void CLEMMY3AudioProcessorEditor::resized()
{
    // Layout will be implemented in Phase 8
}
