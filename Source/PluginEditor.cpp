#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
CLEMMY3AudioProcessorEditor::CLEMMY3AudioProcessorEditor(CLEMMY3AudioProcessor& p)
    : AudioProcessorEditor(&p), audioProcessor(p),
      midiKeyboard(audioProcessor.getKeyboardState(), juce::MidiKeyboardComponent::horizontalKeyboard)
{
    // Set editor size to accommodate keyboard
    setSize(600, 300);

    // Add waveform selector
    addAndMakeVisible(waveformSelector);
    waveformSelector.addItem("Sine", 1);
    waveformSelector.addItem("Sawtooth", 2);
    waveformSelector.addItem("Square", 3);
    waveformSelector.setSelectedId(1);
    waveformSelector.onChange = [this] { waveformChanged(); };

    addAndMakeVisible(waveformLabel);
    waveformLabel.setText("Waveform", juce::dontSendNotification);
    waveformLabel.attachToComponent(&waveformSelector, true);

    // Add pulse width slider
    addAndMakeVisible(pulseWidthSlider);
    pulseWidthSlider.setRange(0.01, 0.99, 0.01);
    pulseWidthSlider.setValue(0.5);
    pulseWidthSlider.setTextValueSuffix("%");
    pulseWidthSlider.onValueChange = [this] { pulseWidthChanged(); };

    addAndMakeVisible(pulseWidthLabel);
    pulseWidthLabel.setText("Pulse Width", juce::dontSendNotification);
    pulseWidthLabel.attachToComponent(&pulseWidthSlider, true);

    // Attach to parameters
    waveformAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(
        audioProcessor.parameters, "waveform", waveformSelector);

    pulseWidthAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.parameters, "pulseWidth", pulseWidthSlider);

    // Add virtual MIDI keyboard
    addAndMakeVisible(midiKeyboard);

    // Enable keyboard focus so computer keyboard works
    setWantsKeyboardFocus(true);
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
    g.drawFittedText("CLEMMY3", getLocalBounds().removeFromTop(60), juce::Justification::centred, 1);

    // Draw subtitle
    g.setFont(14.0f);
    g.setColour(juce::Colours::lightgrey);
    g.drawFittedText("Phase 1: Single Oscillator Engine", getLocalBounds().removeFromTop(80), juce::Justification::centred, 1);
}

void CLEMMY3AudioProcessorEditor::resized()
{
    auto area = getLocalBounds();

    // Title area
    area.removeFromTop(60);

    // Controls area
    auto controlsArea = area.removeFromTop(100);
    controlsArea.reduce(20, 10);

    // Waveform selector
    auto waveformArea = controlsArea.removeFromTop(30);
    waveformArea.removeFromLeft(100); // Space for label
    waveformSelector.setBounds(waveformArea);

    controlsArea.removeFromTop(10); // Spacing

    // Pulse width slider
    auto pwArea = controlsArea.removeFromTop(30);
    pwArea.removeFromLeft(100); // Space for label
    pulseWidthSlider.setBounds(pwArea);

    // Virtual MIDI keyboard at bottom
    auto keyboardArea = area.reduced(10, 10);
    midiKeyboard.setBounds(keyboardArea);
}

void CLEMMY3AudioProcessorEditor::waveformChanged()
{
    // Attachment handles parameter updates automatically
}

void CLEMMY3AudioProcessorEditor::pulseWidthChanged()
{
    // Attachment handles parameter updates automatically
}
