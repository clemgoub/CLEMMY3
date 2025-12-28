#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
CLEMMY3AudioProcessorEditor::CLEMMY3AudioProcessorEditor(CLEMMY3AudioProcessor& p)
    : AudioProcessorEditor(&p), audioProcessor(p),
      midiKeyboard(audioProcessor.getKeyboardState(), juce::MidiKeyboardComponent::horizontalKeyboard)
{
    // Set editor size for full GUI - 3 osc + horizontal mixer + ADSR + keyboard
    setSize(900, 700);

    // ========== VOICE MODE ==========
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

    // ========== OSCILLATOR 1 CONTROLS ==========
    // Enable button
    addAndMakeVisible(osc1EnableButton);
    osc1EnableButton.setButtonText("OSC 1 ON");
    osc1EnableButton.setToggleState(true, juce::dontSendNotification);

    osc1EnableAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(
        audioProcessor.parameters, "osc1Enabled", osc1EnableButton);

    // Waveform selector
    addAndMakeVisible(osc1WaveformSelector);
    osc1WaveformSelector.addItem("Sine", 1);
    osc1WaveformSelector.addItem("Sawtooth", 2);
    osc1WaveformSelector.addItem("Square", 3);
    osc1WaveformSelector.addItem("Triangle", 4);
    osc1WaveformSelector.setSelectedId(1);  // Default: Sine

    addAndMakeVisible(osc1WaveformLabel);
    osc1WaveformLabel.setText("Waveform", juce::dontSendNotification);
    osc1WaveformLabel.setJustificationType(juce::Justification::centred);

    osc1WaveformAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(
        audioProcessor.parameters, "osc1Waveform", osc1WaveformSelector);

    // Gain knob
    addAndMakeVisible(osc1GainSlider);
    osc1GainSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    osc1GainSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 18);
    osc1GainSlider.setRange(0.0, 1.0, 0.01);
    osc1GainSlider.setValue(0.33);
    osc1GainSlider.setTextValueSuffix(" %");

    addAndMakeVisible(osc1GainLabel);
    osc1GainLabel.setText("Osc.1", juce::dontSendNotification);
    osc1GainLabel.setJustificationType(juce::Justification::centred);

    osc1GainAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.parameters, "osc1Gain", osc1GainSlider);

    // Detune knob
    addAndMakeVisible(osc1DetuneSlider);
    osc1DetuneSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    osc1DetuneSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 18);
    osc1DetuneSlider.setRange(-100.0, 100.0, 1.0);
    osc1DetuneSlider.setValue(0.0);
    osc1DetuneSlider.setTextValueSuffix(" ct");

    addAndMakeVisible(osc1DetuneLabel);
    osc1DetuneLabel.setText("Detune", juce::dontSendNotification);
    osc1DetuneLabel.setJustificationType(juce::Justification::centred);

    osc1DetuneAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.parameters, "osc1Detune", osc1DetuneSlider);

    // Octave +/- buttons
    addAndMakeVisible(osc1OctaveUpButton);
    osc1OctaveUpButton.setButtonText("+");
    osc1OctaveUpButton.onClick = [this]() {
        auto* param = audioProcessor.parameters.getRawParameterValue("osc1Octave");
        int currentOctave = (int)param->load();
        if (currentOctave < 3)
            audioProcessor.parameters.getParameter("osc1Octave")->setValueNotifyingHost((currentOctave + 1 + 3) / 6.0f);
    };

    addAndMakeVisible(osc1OctaveDownButton);
    osc1OctaveDownButton.setButtonText("-");
    osc1OctaveDownButton.onClick = [this]() {
        auto* param = audioProcessor.parameters.getRawParameterValue("osc1Octave");
        int currentOctave = (int)param->load();
        if (currentOctave > -3)
            audioProcessor.parameters.getParameter("osc1Octave")->setValueNotifyingHost((currentOctave - 1 + 3) / 6.0f);
    };

    addAndMakeVisible(osc1OctaveLabel);
    osc1OctaveLabel.setText("Octave", juce::dontSendNotification);
    osc1OctaveLabel.setJustificationType(juce::Justification::centred);

    // Pulse Width knob
    addAndMakeVisible(osc1PWSlider);
    osc1PWSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    osc1PWSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 18);
    osc1PWSlider.setRange(0.01, 0.99, 0.01);
    osc1PWSlider.setValue(0.5);
    osc1PWSlider.setTextValueSuffix(" %");

    addAndMakeVisible(osc1PWLabel);
    osc1PWLabel.setText("PW", juce::dontSendNotification);
    osc1PWLabel.setJustificationType(juce::Justification::centred);

    osc1PWAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.parameters, "osc1PW", osc1PWSlider);

    // ========== OSCILLATOR 2 CONTROLS ==========
    // Enable button
    addAndMakeVisible(osc2EnableButton);
    osc2EnableButton.setButtonText("OSC 2 ON");
    osc2EnableButton.setToggleState(true, juce::dontSendNotification);

    osc2EnableAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(
        audioProcessor.parameters, "osc2Enabled", osc2EnableButton);

    // Waveform selector
    addAndMakeVisible(osc2WaveformSelector);
    osc2WaveformSelector.addItem("Sine", 1);
    osc2WaveformSelector.addItem("Sawtooth", 2);
    osc2WaveformSelector.addItem("Square", 3);
    osc2WaveformSelector.addItem("Triangle", 4);
    osc2WaveformSelector.setSelectedId(2);  // Default: Sawtooth

    addAndMakeVisible(osc2WaveformLabel);
    osc2WaveformLabel.setText("Waveform", juce::dontSendNotification);
    osc2WaveformLabel.setJustificationType(juce::Justification::centred);

    osc2WaveformAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(
        audioProcessor.parameters, "osc2Waveform", osc2WaveformSelector);

    // Gain knob
    addAndMakeVisible(osc2GainSlider);
    osc2GainSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    osc2GainSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 18);
    osc2GainSlider.setRange(0.0, 1.0, 0.01);
    osc2GainSlider.setValue(0.33);
    osc2GainSlider.setTextValueSuffix(" %");

    addAndMakeVisible(osc2GainLabel);
    osc2GainLabel.setText("Osc.2", juce::dontSendNotification);
    osc2GainLabel.setJustificationType(juce::Justification::centred);

    osc2GainAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.parameters, "osc2Gain", osc2GainSlider);

    // Detune knob
    addAndMakeVisible(osc2DetuneSlider);
    osc2DetuneSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    osc2DetuneSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 18);
    osc2DetuneSlider.setRange(-100.0, 100.0, 1.0);
    osc2DetuneSlider.setValue(0.0);
    osc2DetuneSlider.setTextValueSuffix(" ct");

    addAndMakeVisible(osc2DetuneLabel);
    osc2DetuneLabel.setText("Detune", juce::dontSendNotification);
    osc2DetuneLabel.setJustificationType(juce::Justification::centred);

    osc2DetuneAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.parameters, "osc2Detune", osc2DetuneSlider);

    // Octave +/- buttons
    addAndMakeVisible(osc2OctaveUpButton);
    osc2OctaveUpButton.setButtonText("+");
    osc2OctaveUpButton.onClick = [this]() {
        auto* param = audioProcessor.parameters.getRawParameterValue("osc2Octave");
        int currentOctave = (int)param->load();
        if (currentOctave < 3)
            audioProcessor.parameters.getParameter("osc2Octave")->setValueNotifyingHost((currentOctave + 1 + 3) / 6.0f);
    };

    addAndMakeVisible(osc2OctaveDownButton);
    osc2OctaveDownButton.setButtonText("-");
    osc2OctaveDownButton.onClick = [this]() {
        auto* param = audioProcessor.parameters.getRawParameterValue("osc2Octave");
        int currentOctave = (int)param->load();
        if (currentOctave > -3)
            audioProcessor.parameters.getParameter("osc2Octave")->setValueNotifyingHost((currentOctave - 1 + 3) / 6.0f);
    };

    addAndMakeVisible(osc2OctaveLabel);
    osc2OctaveLabel.setText("Octave", juce::dontSendNotification);
    osc2OctaveLabel.setJustificationType(juce::Justification::centred);

    // Pulse Width knob
    addAndMakeVisible(osc2PWSlider);
    osc2PWSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    osc2PWSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 18);
    osc2PWSlider.setRange(0.01, 0.99, 0.01);
    osc2PWSlider.setValue(0.5);
    osc2PWSlider.setTextValueSuffix(" %");

    addAndMakeVisible(osc2PWLabel);
    osc2PWLabel.setText("PW", juce::dontSendNotification);
    osc2PWLabel.setJustificationType(juce::Justification::centred);

    osc2PWAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.parameters, "osc2PW", osc2PWSlider);

    // ========== OSCILLATOR 3 CONTROLS ==========
    // Enable button
    addAndMakeVisible(osc3EnableButton);
    osc3EnableButton.setButtonText("OSC 3 ON");
    osc3EnableButton.setToggleState(true, juce::dontSendNotification);

    osc3EnableAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(
        audioProcessor.parameters, "osc3Enabled", osc3EnableButton);

    // Waveform selector
    addAndMakeVisible(osc3WaveformSelector);
    osc3WaveformSelector.addItem("Sine", 1);
    osc3WaveformSelector.addItem("Sawtooth", 2);
    osc3WaveformSelector.addItem("Square", 3);
    osc3WaveformSelector.addItem("Triangle", 4);
    osc3WaveformSelector.setSelectedId(3);  // Default: Square

    addAndMakeVisible(osc3WaveformLabel);
    osc3WaveformLabel.setText("Waveform", juce::dontSendNotification);
    osc3WaveformLabel.setJustificationType(juce::Justification::centred);

    osc3WaveformAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(
        audioProcessor.parameters, "osc3Waveform", osc3WaveformSelector);

    // Gain knob
    addAndMakeVisible(osc3GainSlider);
    osc3GainSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    osc3GainSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 18);
    osc3GainSlider.setRange(0.0, 1.0, 0.01);
    osc3GainSlider.setValue(0.33);
    osc3GainSlider.setTextValueSuffix(" %");

    addAndMakeVisible(osc3GainLabel);
    osc3GainLabel.setText("Osc.3", juce::dontSendNotification);
    osc3GainLabel.setJustificationType(juce::Justification::centred);

    osc3GainAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.parameters, "osc3Gain", osc3GainSlider);

    // Detune knob
    addAndMakeVisible(osc3DetuneSlider);
    osc3DetuneSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    osc3DetuneSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 18);
    osc3DetuneSlider.setRange(-100.0, 100.0, 1.0);
    osc3DetuneSlider.setValue(0.0);
    osc3DetuneSlider.setTextValueSuffix(" ct");

    addAndMakeVisible(osc3DetuneLabel);
    osc3DetuneLabel.setText("Detune", juce::dontSendNotification);
    osc3DetuneLabel.setJustificationType(juce::Justification::centred);

    osc3DetuneAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.parameters, "osc3Detune", osc3DetuneSlider);

    // Octave +/- buttons
    addAndMakeVisible(osc3OctaveUpButton);
    osc3OctaveUpButton.setButtonText("+");
    osc3OctaveUpButton.onClick = [this]() {
        auto* param = audioProcessor.parameters.getRawParameterValue("osc3Octave");
        int currentOctave = (int)param->load();
        if (currentOctave < 3)
            audioProcessor.parameters.getParameter("osc3Octave")->setValueNotifyingHost((currentOctave + 1 + 3) / 6.0f);
    };

    addAndMakeVisible(osc3OctaveDownButton);
    osc3OctaveDownButton.setButtonText("-");
    osc3OctaveDownButton.onClick = [this]() {
        auto* param = audioProcessor.parameters.getRawParameterValue("osc3Octave");
        int currentOctave = (int)param->load();
        if (currentOctave > -3)
            audioProcessor.parameters.getParameter("osc3Octave")->setValueNotifyingHost((currentOctave - 1 + 3) / 6.0f);
    };

    addAndMakeVisible(osc3OctaveLabel);
    osc3OctaveLabel.setText("Octave", juce::dontSendNotification);
    osc3OctaveLabel.setJustificationType(juce::Justification::centred);

    // Pulse Width knob
    addAndMakeVisible(osc3PWSlider);
    osc3PWSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    osc3PWSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 18);
    osc3PWSlider.setRange(0.01, 0.99, 0.01);
    osc3PWSlider.setValue(0.5);
    osc3PWSlider.setTextValueSuffix(" %");

    addAndMakeVisible(osc3PWLabel);
    osc3PWLabel.setText("PW", juce::dontSendNotification);
    osc3PWLabel.setJustificationType(juce::Justification::centred);

    osc3PWAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.parameters, "osc3PW", osc3PWSlider);

    // ========== MIXER CONTROLS (NOISE) ==========
    // Enable button
    addAndMakeVisible(noiseEnableButton);
    noiseEnableButton.setButtonText("NOISE ON");
    noiseEnableButton.setToggleState(false, juce::dontSendNotification);

    noiseEnableAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(
        audioProcessor.parameters, "noiseEnabled", noiseEnableButton);

    // Noise type selector
    addAndMakeVisible(noiseTypeSelector);
    noiseTypeSelector.addItem("White", 1);
    noiseTypeSelector.addItem("Pink", 2);
    noiseTypeSelector.addItem("Brown", 3);
    noiseTypeSelector.setSelectedId(1);  // Default: White

    addAndMakeVisible(noiseTypeLabel);
    noiseTypeLabel.setText("Type", juce::dontSendNotification);
    noiseTypeLabel.setJustificationType(juce::Justification::centred);

    noiseTypeAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(
        audioProcessor.parameters, "noiseType", noiseTypeSelector);

    // Gain knob
    addAndMakeVisible(noiseGainSlider);
    noiseGainSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    noiseGainSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 18);
    noiseGainSlider.setRange(0.0, 1.0, 0.01);
    noiseGainSlider.setValue(0.0);
    noiseGainSlider.setTextValueSuffix(" %");

    addAndMakeVisible(noiseGainLabel);
    noiseGainLabel.setText("Gain", juce::dontSendNotification);
    noiseGainLabel.setJustificationType(juce::Justification::centred);

    noiseGainAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.parameters, "noiseGain", noiseGainSlider);

    // Master Volume knob (bigger)
    addAndMakeVisible(masterVolumeSlider);
    masterVolumeSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    masterVolumeSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 70, 20);
    masterVolumeSlider.setRange(0.0, 1.0, 0.01);
    masterVolumeSlider.setValue(0.8);
    masterVolumeSlider.setTextValueSuffix(" %");

    addAndMakeVisible(masterVolumeLabel);
    masterVolumeLabel.setText("Master", juce::dontSendNotification);
    masterVolumeLabel.setJustificationType(juce::Justification::centred);

    masterVolumeAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.parameters, "masterVolume", masterVolumeSlider);

    // ========== ADSR ENVELOPE ==========
    // Attack
    addAndMakeVisible(attackSlider);
    attackSlider.setSliderStyle(juce::Slider::LinearVertical);
    attackSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 18);
    attackSlider.setRange(0.001, 2.0, 0.001);
    attackSlider.setValue(0.01);
    attackSlider.setTextValueSuffix(" s");
    attackSlider.setSkewFactorFromMidPoint(0.1);

    addAndMakeVisible(attackLabel);
    attackLabel.setText("Attack", juce::dontSendNotification);
    attackLabel.setJustificationType(juce::Justification::centred);

    attackAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.parameters, "attack", attackSlider);

    // Decay
    addAndMakeVisible(decaySlider);
    decaySlider.setSliderStyle(juce::Slider::LinearVertical);
    decaySlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 18);
    decaySlider.setRange(0.001, 2.0, 0.001);
    decaySlider.setValue(0.3);
    decaySlider.setTextValueSuffix(" s");
    decaySlider.setSkewFactorFromMidPoint(0.1);

    addAndMakeVisible(decayLabel);
    decayLabel.setText("Decay", juce::dontSendNotification);
    decayLabel.setJustificationType(juce::Justification::centred);

    decayAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.parameters, "decay", decaySlider);

    // Sustain
    addAndMakeVisible(sustainSlider);
    sustainSlider.setSliderStyle(juce::Slider::LinearVertical);
    sustainSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 18);
    sustainSlider.setRange(0.0, 1.0, 0.01);
    sustainSlider.setValue(0.7);
    sustainSlider.setTextValueSuffix(" %");

    addAndMakeVisible(sustainLabel);
    sustainLabel.setText("Sustain", juce::dontSendNotification);
    sustainLabel.setJustificationType(juce::Justification::centred);

    sustainAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.parameters, "sustain", sustainSlider);

    // Release
    addAndMakeVisible(releaseSlider);
    releaseSlider.setSliderStyle(juce::Slider::LinearVertical);
    releaseSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 18);
    releaseSlider.setRange(0.001, 5.0, 0.001);
    releaseSlider.setValue(0.5);
    releaseSlider.setTextValueSuffix(" s");
    releaseSlider.setSkewFactorFromMidPoint(0.2);

    addAndMakeVisible(releaseLabel);
    releaseLabel.setText("Release", juce::dontSendNotification);
    releaseLabel.setJustificationType(juce::Justification::centred);

    releaseAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.parameters, "release", releaseSlider);

    // ========== MIDI KEYBOARD ==========
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
    g.drawFittedText("CLEMMY3", getLocalBounds().removeFromTop(50), juce::Justification::centred, 1);

    // Draw subtitle
    g.setFont(14.0f);
    g.setColour(juce::Colours::lightgrey);
    g.drawFittedText("Phase 4: Triple Oscillator Architecture with Full GUI", getLocalBounds().removeFromTop(70), juce::Justification::centred, 1);

    // Draw section labels
    g.setFont(12.0f);
    g.setColour(juce::Colours::white);

    // Oscillator section labels (positioned above each column)
    g.drawText("OSCILLATOR 1", 20, 100, 200, 20, juce::Justification::centred);
    g.drawText("OSCILLATOR 2", 235, 100, 200, 20, juce::Justification::centred);
    g.drawText("OSCILLATOR 3", 450, 100, 200, 20, juce::Justification::centred);

    // Mixer column label
    g.drawText("MIXER", 665, 100, 215, 20, juce::Justification::centred);

    // ADSR Envelope section label
    g.drawText("ADSR ENVELOPE", 20, 460, 400, 20, juce::Justification::centred);

    // LFO placeholder label
    g.setColour(juce::Colours::grey);
    g.drawText("LFO MODULATION (Coming Soon)", 440, 460, 440, 20, juce::Justification::centred);
}

void CLEMMY3AudioProcessorEditor::resized()
{
    auto area = getLocalBounds();

    // Title and subtitle area
    area.removeFromTop(80);

    // Voice mode selector
    auto voiceModeArea = area.removeFromTop(30);
    voiceModeArea.reduce(20, 0);
    voiceModeArea.removeFromLeft(80);  // Space for label
    voiceModeSelector.setBounds(voiceModeArea.removeFromLeft(120));

    area.removeFromTop(10);  // Spacing

    // ========== OSCILLATOR + MIXER SECTION ==========
    // 4 columns: Osc1, Osc2, Osc3, Mixer
    auto topSection = area.removeFromTop(330);
    topSection.reduce(20, 0);

    int oscColumnWidth = 200;
    int mixerColumnWidth = 140;
    int columnSpacing = 15;
    int sliderHeight = 110;
    int controlSpacing = 5;

    // Helper lambda to layout one oscillator column (with +/- buttons for octave)
    auto layoutOscillatorColumn = [&](juce::Rectangle<int> column,
                                       juce::ToggleButton& enableBtn,
                                       juce::ComboBox& waveSelector, juce::Label& waveLabel,
                                       juce::Slider& detuneSlider, juce::Label& detuneLabel,
                                       juce::TextButton& octaveUpBtn, juce::TextButton& octaveDownBtn, juce::Label& octaveLabel,
                                       juce::Slider& pwSlider, juce::Label& pwLabel)
    {
        auto col = column;
        col.removeFromTop(20);  // Space for section label

        // Enable button
        enableBtn.setBounds(col.removeFromTop(25));
        col.removeFromTop(controlSpacing);

        // Waveform selector
        waveLabel.setBounds(col.removeFromTop(15));
        waveSelector.setBounds(col.removeFromTop(25));
        col.removeFromTop(10);

        // Knobs and controls row
        auto controlsRow = col.removeFromTop(sliderHeight);

        // Detune knob
        auto detuneArea = controlsRow.removeFromLeft(62);
        detuneLabel.setBounds(detuneArea.removeFromTop(18));
        detuneSlider.setBounds(detuneArea);

        controlsRow.removeFromLeft(8);

        // Octave +/- buttons (vertical, bigger)
        auto octaveArea = controlsRow.removeFromLeft(60);
        octaveLabel.setBounds(octaveArea.removeFromTop(18));
        octaveUpBtn.setBounds(octaveArea.removeFromTop(44));
        octaveArea.removeFromTop(4);
        octaveDownBtn.setBounds(octaveArea.removeFromTop(44));

        controlsRow.removeFromLeft(8);

        // Pulse Width knob
        auto pwArea = controlsRow.removeFromLeft(62);
        pwLabel.setBounds(pwArea.removeFromTop(18));
        pwSlider.setBounds(pwArea);
    };

    // Oscillator 1
    auto osc1Area = topSection.removeFromLeft(oscColumnWidth);
    layoutOscillatorColumn(osc1Area,
                           osc1EnableButton,
                           osc1WaveformSelector, osc1WaveformLabel,
                           osc1DetuneSlider, osc1DetuneLabel,
                           osc1OctaveUpButton, osc1OctaveDownButton, osc1OctaveLabel,
                           osc1PWSlider, osc1PWLabel);

    topSection.removeFromLeft(columnSpacing);

    // Oscillator 2
    auto osc2Area = topSection.removeFromLeft(oscColumnWidth);
    layoutOscillatorColumn(osc2Area,
                           osc2EnableButton,
                           osc2WaveformSelector, osc2WaveformLabel,
                           osc2DetuneSlider, osc2DetuneLabel,
                           osc2OctaveUpButton, osc2OctaveDownButton, osc2OctaveLabel,
                           osc2PWSlider, osc2PWLabel);

    topSection.removeFromLeft(columnSpacing);

    // Oscillator 3
    auto osc3Area = topSection.removeFromLeft(oscColumnWidth);
    layoutOscillatorColumn(osc3Area,
                           osc3EnableButton,
                           osc3WaveformSelector, osc3WaveformLabel,
                           osc3DetuneSlider, osc3DetuneLabel,
                           osc3OctaveUpButton, osc3OctaveDownButton, osc3OctaveLabel,
                           osc3PWSlider, osc3PWLabel);

    // ========== MIXER COLUMN (VERTICAL) ==========
    // Mixer column: Osc.1/2/3 gains + Noise + Master (stacked vertically)
    auto mixerArea = topSection;
    mixerArea.removeFromTop(20);  // Space for section label

    int knobHeight = 70;
    int knobSpacing = 3;

    // Osc 1 Gain
    auto osc1GainArea = mixerArea.removeFromTop(knobHeight);
    osc1GainLabel.setBounds(osc1GainArea.removeFromTop(18));
    osc1GainSlider.setBounds(osc1GainArea);

    mixerArea.removeFromTop(knobSpacing);

    // Osc 2 Gain
    auto osc2GainArea = mixerArea.removeFromTop(knobHeight);
    osc2GainLabel.setBounds(osc2GainArea.removeFromTop(18));
    osc2GainSlider.setBounds(osc2GainArea);

    mixerArea.removeFromTop(knobSpacing);

    // Osc 3 Gain
    auto osc3GainArea = mixerArea.removeFromTop(knobHeight);
    osc3GainLabel.setBounds(osc3GainArea.removeFromTop(18));
    osc3GainSlider.setBounds(osc3GainArea);

    mixerArea.removeFromTop(8);  // Extra spacing before noise

    // Noise section
    noiseEnableButton.setBounds(mixerArea.removeFromTop(22));
    mixerArea.removeFromTop(2);
    noiseTypeLabel.setBounds(mixerArea.removeFromTop(15));
    noiseTypeSelector.setBounds(mixerArea.removeFromTop(22));
    mixerArea.removeFromTop(2);
    auto noiseGainArea = mixerArea.removeFromTop(60);
    noiseGainLabel.setBounds(noiseGainArea.removeFromTop(18));
    noiseGainSlider.setBounds(noiseGainArea);

    mixerArea.removeFromTop(8);  // Extra spacing before master

    // Master Volume (bigger knob at bottom)
    auto masterArea = mixerArea;
    masterVolumeLabel.setBounds(masterArea.removeFromTop(18));
    masterVolumeSlider.setBounds(masterArea);

    // ========== ADSR ENVELOPE SECTION ==========
    area.removeFromTop(20);  // Spacing + label space
    auto adsrArea = area.removeFromTop(120);
    adsrArea.reduce(20, 0);

    int adsrSliderWidth = 80;
    int adsrSpacing = 15;

    auto attackArea = adsrArea.removeFromLeft(adsrSliderWidth);
    attackLabel.setBounds(attackArea.removeFromTop(15));
    attackSlider.setBounds(attackArea);

    adsrArea.removeFromLeft(adsrSpacing);

    auto decayArea = adsrArea.removeFromLeft(adsrSliderWidth);
    decayLabel.setBounds(decayArea.removeFromTop(15));
    decaySlider.setBounds(decayArea);

    adsrArea.removeFromLeft(adsrSpacing);

    auto sustainArea = adsrArea.removeFromLeft(adsrSliderWidth);
    sustainLabel.setBounds(sustainArea.removeFromTop(15));
    sustainSlider.setBounds(sustainArea);

    adsrArea.removeFromLeft(adsrSpacing);

    auto releaseArea = adsrArea.removeFromLeft(adsrSliderWidth);
    releaseLabel.setBounds(releaseArea.removeFromTop(15));
    releaseSlider.setBounds(releaseArea);

    // ========== MIDI KEYBOARD ==========
    auto keyboardArea = area.reduced(10, 5);
    midiKeyboard.setBounds(keyboardArea);
}
