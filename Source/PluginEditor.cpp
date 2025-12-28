#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
CLEMMY3AudioProcessorEditor::CLEMMY3AudioProcessorEditor(CLEMMY3AudioProcessor& p)
    : AudioProcessorEditor(&p), audioProcessor(p),
      midiKeyboard(audioProcessor.getKeyboardState(), juce::MidiKeyboardComponent::horizontalKeyboard)
{
    // Set editor size to accommodate ADSR controls and keyboard
    setSize(800, 380);

    // Add voice mode selector
    addAndMakeVisible(voiceModeSelector);
    voiceModeSelector.addItem("Mono", 1);
    voiceModeSelector.addItem("Poly", 2);
    voiceModeSelector.addItem("Unison", 3);
    voiceModeSelector.setSelectedId(2);  // Default: Poly

    addAndMakeVisible(voiceModeLabel);
    voiceModeLabel.setText("Voice Mode", juce::dontSendNotification);
    voiceModeLabel.attachToComponent(&voiceModeSelector, true);

    voiceModeAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(
        audioProcessor.parameters, "voiceMode", voiceModeSelector);

    // Add waveform selector
    addAndMakeVisible(waveformSelector);
    waveformSelector.addItem("Sine", 1);
    waveformSelector.addItem("Sawtooth", 2);
    waveformSelector.addItem("Square", 3);
    waveformSelector.addItem("Triangle", 4);
    waveformSelector.addItem("Noise", 5);
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

    // Add ADSR envelope controls
    // Attack
    addAndMakeVisible(attackSlider);
    attackSlider.setSliderStyle(juce::Slider::LinearVertical);
    attackSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 80, 20);
    attackSlider.setRange(0.001, 2.0, 0.001);
    attackSlider.setValue(0.01);
    attackSlider.setTextValueSuffix(" s");
    attackSlider.setSkewFactorFromMidPoint(0.1);  // Skewed for better control

    addAndMakeVisible(attackLabel);
    attackLabel.setText("Attack", juce::dontSendNotification);
    attackLabel.setJustificationType(juce::Justification::centredTop);
    attackLabel.attachToComponent(&attackSlider, false);

    attackAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.parameters, "attack", attackSlider);

    // Decay
    addAndMakeVisible(decaySlider);
    decaySlider.setSliderStyle(juce::Slider::LinearVertical);
    decaySlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 80, 20);
    decaySlider.setRange(0.001, 2.0, 0.001);
    decaySlider.setValue(0.3);
    decaySlider.setTextValueSuffix(" s");
    decaySlider.setSkewFactorFromMidPoint(0.1);

    addAndMakeVisible(decayLabel);
    decayLabel.setText("Decay", juce::dontSendNotification);
    decayLabel.setJustificationType(juce::Justification::centredTop);
    decayLabel.attachToComponent(&decaySlider, false);

    decayAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.parameters, "decay", decaySlider);

    // Sustain
    addAndMakeVisible(sustainSlider);
    sustainSlider.setSliderStyle(juce::Slider::LinearVertical);
    sustainSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 80, 20);
    sustainSlider.setRange(0.0, 1.0, 0.01);
    sustainSlider.setValue(0.7);
    sustainSlider.setTextValueSuffix(" %");

    addAndMakeVisible(sustainLabel);
    sustainLabel.setText("Sustain", juce::dontSendNotification);
    sustainLabel.setJustificationType(juce::Justification::centredTop);
    sustainLabel.attachToComponent(&sustainSlider, false);

    sustainAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.parameters, "sustain", sustainSlider);

    // Release
    addAndMakeVisible(releaseSlider);
    releaseSlider.setSliderStyle(juce::Slider::LinearVertical);
    releaseSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 80, 20);
    releaseSlider.setRange(0.001, 5.0, 0.001);
    releaseSlider.setValue(0.5);
    releaseSlider.setTextValueSuffix(" s");
    releaseSlider.setSkewFactorFromMidPoint(0.2);

    addAndMakeVisible(releaseLabel);
    releaseLabel.setText("Release", juce::dontSendNotification);
    releaseLabel.setJustificationType(juce::Justification::centredTop);
    releaseLabel.attachToComponent(&releaseSlider, false);

    releaseAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.parameters, "release", releaseSlider);

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
    g.drawFittedText("Phase 3: Polyphony", getLocalBounds().removeFromTop(80), juce::Justification::centred, 1);
}

void CLEMMY3AudioProcessorEditor::resized()
{
    auto area = getLocalBounds();

    // Title area
    area.removeFromTop(60);

    // Oscillator controls area (left side)
    auto topControls = area.removeFromTop(170);
    auto oscArea = topControls.removeFromLeft(400);
    oscArea.reduce(20, 10);

    // Voice mode selector
    auto voiceModeArea = oscArea.removeFromTop(30);
    voiceModeArea.removeFromLeft(100); // Space for label
    voiceModeSelector.setBounds(voiceModeArea);

    oscArea.removeFromTop(10); // Spacing

    // Waveform selector
    auto waveformArea = oscArea.removeFromTop(30);
    waveformArea.removeFromLeft(100); // Space for label
    waveformSelector.setBounds(waveformArea);

    oscArea.removeFromTop(10); // Spacing

    // Pulse width slider
    auto pwArea = oscArea.removeFromTop(30);
    pwArea.removeFromLeft(100); // Space for label
    pulseWidthSlider.setBounds(pwArea);

    // ADSR envelope controls (right side)
    auto adsrArea = topControls.reduced(10, 10);
    int sliderWidth = 80;
    int spacing = 10;

    // Layout four vertical sliders in a row
    auto attackArea = adsrArea.removeFromLeft(sliderWidth);
    attackArea.removeFromTop(20); // Space for label
    attackSlider.setBounds(attackArea);

    adsrArea.removeFromLeft(spacing);

    auto decayArea = adsrArea.removeFromLeft(sliderWidth);
    decayArea.removeFromTop(20);
    decaySlider.setBounds(decayArea);

    adsrArea.removeFromLeft(spacing);

    auto sustainArea = adsrArea.removeFromLeft(sliderWidth);
    sustainArea.removeFromTop(20);
    sustainSlider.setBounds(sustainArea);

    adsrArea.removeFromLeft(spacing);

    auto releaseArea = adsrArea.removeFromLeft(sliderWidth);
    releaseArea.removeFromTop(20);
    releaseSlider.setBounds(releaseArea);

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
