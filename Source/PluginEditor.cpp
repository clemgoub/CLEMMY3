#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
CLEMMY3AudioProcessorEditor::CLEMMY3AudioProcessorEditor(CLEMMY3AudioProcessor& p)
    : AudioProcessorEditor(&p), audioProcessor(p),
      midiKeyboard(audioProcessor.getKeyboardState(), juce::MidiKeyboardComponent::horizontalKeyboard)
{
    // Set editor size to match Python reference layout
    // Top row: Oscillators | Mixer | Filter
    // Bottom row: ADSR | LFO placeholder
    // Very bottom: MIDI keyboard
    setSize(1100, 780);  // Increased height for better ADSR fit

    // ========== VOICE MODE ==========
    // Mode label
    addAndMakeVisible(voiceModeLabel);
    voiceModeLabel.setText("Mode:", juce::dontSendNotification);
    voiceModeLabel.setJustificationType(juce::Justification::centredRight);

    // MONO button
    addAndMakeVisible(voiceModeMonoButton);
    voiceModeMonoButton.setButtonText("MONO");
    voiceModeMonoButton.setClickingTogglesState(true);
    voiceModeMonoButton.setRadioGroupId(2001);
    voiceModeMonoButton.onClick = [this]() {
        if (voiceModeMonoButton.getToggleState())
            audioProcessor.parameters.getParameter("voiceMode")->setValueNotifyingHost(0.0f);
    };

    // POLY button
    addAndMakeVisible(voiceModePolyButton);
    voiceModePolyButton.setButtonText("POLY");
    voiceModePolyButton.setClickingTogglesState(true);
    voiceModePolyButton.setRadioGroupId(2001);
    voiceModePolyButton.setToggleState(true, juce::dontSendNotification);  // Default
    voiceModePolyButton.onClick = [this]() {
        if (voiceModePolyButton.getToggleState())
            audioProcessor.parameters.getParameter("voiceMode")->setValueNotifyingHost(0.5f);
    };

    // UNISON button
    addAndMakeVisible(voiceModeUnisonButton);
    voiceModeUnisonButton.setButtonText("UNI");
    voiceModeUnisonButton.setClickingTogglesState(true);
    voiceModeUnisonButton.setRadioGroupId(2001);
    voiceModeUnisonButton.onClick = [this]() {
        if (voiceModeUnisonButton.getToggleState())
            audioProcessor.parameters.getParameter("voiceMode")->setValueNotifyingHost(1.0f);
    };

    // ========== OSCILLATOR 1 CONTROLS ==========
    // Enable button
    addAndMakeVisible(osc1EnableButton);
    osc1EnableButton.setButtonText("ON");
    osc1EnableButton.setClickingTogglesState(true);
    osc1EnableButton.setToggleState(true, juce::dontSendNotification);  // Default: ON
    osc1EnableButton.setColour(juce::TextButton::buttonOnColourId, juce::Colours::red);
    osc1EnableButton.onClick = [this]() {
        audioProcessor.parameters.getParameter("osc1Enabled")->setValueNotifyingHost(
            osc1EnableButton.getToggleState() ? 1.0f : 0.0f);
    };

    // Waveform selector
    addAndMakeVisible(osc1WaveformSelector);
    osc1WaveformSelector.addItem("Sine", 1);
    osc1WaveformSelector.addItem("Sawtooth", 2);
    osc1WaveformSelector.addItem("Square", 3);
    osc1WaveformSelector.addItem("Triangle", 4);
    osc1WaveformSelector.setSelectedId(1);  // Default: Sine

    addAndMakeVisible(osc1WaveformLabel);
    osc1WaveformLabel.setText("Wave", juce::dontSendNotification);
    osc1WaveformLabel.setJustificationType(juce::Justification::centredLeft);

    osc1WaveformAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(
        audioProcessor.parameters, "osc1Waveform", osc1WaveformSelector);

    // Gain knob
    addAndMakeVisible(osc1GainSlider);
    osc1GainSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    osc1GainSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    osc1GainSlider.setRange(0.0, 1.0, 0.01);
    osc1GainSlider.setValue(0.33);

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
        if (currentOctave < 3) {
            audioProcessor.parameters.getParameter("osc1Octave")->setValueNotifyingHost((currentOctave + 1 + 3) / 6.0f);
            osc1OctaveValueLabel.setText(juce::String(currentOctave + 1), juce::dontSendNotification);
        }
    };

    addAndMakeVisible(osc1OctaveDownButton);
    osc1OctaveDownButton.setButtonText("-");
    osc1OctaveDownButton.onClick = [this]() {
        auto* param = audioProcessor.parameters.getRawParameterValue("osc1Octave");
        int currentOctave = (int)param->load();
        if (currentOctave > -3) {
            audioProcessor.parameters.getParameter("osc1Octave")->setValueNotifyingHost((currentOctave - 1 + 3) / 6.0f);
            osc1OctaveValueLabel.setText(juce::String(currentOctave - 1), juce::dontSendNotification);
        }
    };

    addAndMakeVisible(osc1OctaveLabel);
    osc1OctaveLabel.setText("Octave", juce::dontSendNotification);
    osc1OctaveLabel.setJustificationType(juce::Justification::centred);

    // Octave value display (shows current octave: -3 to +3)
    addAndMakeVisible(osc1OctaveValueLabel);
    osc1OctaveValueLabel.setText("0", juce::dontSendNotification);
    osc1OctaveValueLabel.setJustificationType(juce::Justification::centred);
    osc1OctaveValueLabel.setFont(juce::Font(14.0f, juce::Font::bold));

    // Pulse Width knob
    addAndMakeVisible(osc1PWSlider);
    osc1PWSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    osc1PWSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    osc1PWSlider.setRange(0.01, 0.99, 0.01);
    osc1PWSlider.setValue(0.5);

    addAndMakeVisible(osc1PWLabel);
    osc1PWLabel.setText("PWM", juce::dontSendNotification);
    osc1PWLabel.setJustificationType(juce::Justification::centredLeft);

    osc1PWAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.parameters, "osc1PW", osc1PWSlider);

    // ========== OSCILLATOR 2 CONTROLS ==========
    // Enable button
    addAndMakeVisible(osc2EnableButton);
    osc2EnableButton.setButtonText("ON");
    osc2EnableButton.setClickingTogglesState(true);
    osc2EnableButton.setToggleState(true, juce::dontSendNotification);  // Default: ON
    osc2EnableButton.setColour(juce::TextButton::buttonOnColourId, juce::Colours::red);
    osc2EnableButton.onClick = [this]() {
        audioProcessor.parameters.getParameter("osc2Enabled")->setValueNotifyingHost(
            osc2EnableButton.getToggleState() ? 1.0f : 0.0f);
    };

    // Waveform selector
    addAndMakeVisible(osc2WaveformSelector);
    osc2WaveformSelector.addItem("Sine", 1);
    osc2WaveformSelector.addItem("Sawtooth", 2);
    osc2WaveformSelector.addItem("Square", 3);
    osc2WaveformSelector.addItem("Triangle", 4);
    osc2WaveformSelector.setSelectedId(2);  // Default: Sawtooth

    addAndMakeVisible(osc2WaveformLabel);
    osc2WaveformLabel.setText("Wave", juce::dontSendNotification);
    osc2WaveformLabel.setJustificationType(juce::Justification::centredLeft);

    osc2WaveformAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(
        audioProcessor.parameters, "osc2Waveform", osc2WaveformSelector);

    // Gain knob
    addAndMakeVisible(osc2GainSlider);
    osc2GainSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    osc2GainSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    osc2GainSlider.setRange(0.0, 1.0, 0.01);
    osc2GainSlider.setValue(0.33);

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
        if (currentOctave < 3) {
            audioProcessor.parameters.getParameter("osc2Octave")->setValueNotifyingHost((currentOctave + 1 + 3) / 6.0f);
            osc2OctaveValueLabel.setText(juce::String(currentOctave + 1), juce::dontSendNotification);
        }
    };

    addAndMakeVisible(osc2OctaveDownButton);
    osc2OctaveDownButton.setButtonText("-");
    osc2OctaveDownButton.onClick = [this]() {
        auto* param = audioProcessor.parameters.getRawParameterValue("osc2Octave");
        int currentOctave = (int)param->load();
        if (currentOctave > -3) {
            audioProcessor.parameters.getParameter("osc2Octave")->setValueNotifyingHost((currentOctave - 1 + 3) / 6.0f);
            osc2OctaveValueLabel.setText(juce::String(currentOctave - 1), juce::dontSendNotification);
        }
    };

    addAndMakeVisible(osc2OctaveLabel);
    osc2OctaveLabel.setText("Octave", juce::dontSendNotification);
    osc2OctaveLabel.setJustificationType(juce::Justification::centred);

    // Octave value display (shows current octave: -3 to +3)
    addAndMakeVisible(osc2OctaveValueLabel);
    osc2OctaveValueLabel.setText("0", juce::dontSendNotification);
    osc2OctaveValueLabel.setJustificationType(juce::Justification::centred);
    osc2OctaveValueLabel.setFont(juce::Font(14.0f, juce::Font::bold));

    // Pulse Width knob
    addAndMakeVisible(osc2PWSlider);
    osc2PWSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    osc2PWSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    osc2PWSlider.setRange(0.01, 0.99, 0.01);
    osc2PWSlider.setValue(0.5);

    addAndMakeVisible(osc2PWLabel);
    osc2PWLabel.setText("PWM", juce::dontSendNotification);
    osc2PWLabel.setJustificationType(juce::Justification::centredLeft);

    osc2PWAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.parameters, "osc2PW", osc2PWSlider);

    // ========== OSCILLATOR 3 CONTROLS ==========
    // Enable button
    addAndMakeVisible(osc3EnableButton);
    osc3EnableButton.setButtonText("ON");
    osc3EnableButton.setClickingTogglesState(true);
    osc3EnableButton.setToggleState(true, juce::dontSendNotification);  // Default: ON
    osc3EnableButton.setColour(juce::TextButton::buttonOnColourId, juce::Colours::red);
    osc3EnableButton.onClick = [this]() {
        audioProcessor.parameters.getParameter("osc3Enabled")->setValueNotifyingHost(
            osc3EnableButton.getToggleState() ? 1.0f : 0.0f);
    };

    // Waveform selector
    addAndMakeVisible(osc3WaveformSelector);
    osc3WaveformSelector.addItem("Sine", 1);
    osc3WaveformSelector.addItem("Sawtooth", 2);
    osc3WaveformSelector.addItem("Square", 3);
    osc3WaveformSelector.addItem("Triangle", 4);
    osc3WaveformSelector.setSelectedId(3);  // Default: Square

    addAndMakeVisible(osc3WaveformLabel);
    osc3WaveformLabel.setText("Wave", juce::dontSendNotification);
    osc3WaveformLabel.setJustificationType(juce::Justification::centredLeft);

    osc3WaveformAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(
        audioProcessor.parameters, "osc3Waveform", osc3WaveformSelector);

    // Gain knob
    addAndMakeVisible(osc3GainSlider);
    osc3GainSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    osc3GainSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    osc3GainSlider.setRange(0.0, 1.0, 0.01);
    osc3GainSlider.setValue(0.33);

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
        if (currentOctave < 3) {
            audioProcessor.parameters.getParameter("osc3Octave")->setValueNotifyingHost((currentOctave + 1 + 3) / 6.0f);
            osc3OctaveValueLabel.setText(juce::String(currentOctave + 1), juce::dontSendNotification);
        }
    };

    addAndMakeVisible(osc3OctaveDownButton);
    osc3OctaveDownButton.setButtonText("-");
    osc3OctaveDownButton.onClick = [this]() {
        auto* param = audioProcessor.parameters.getRawParameterValue("osc3Octave");
        int currentOctave = (int)param->load();
        if (currentOctave > -3) {
            audioProcessor.parameters.getParameter("osc3Octave")->setValueNotifyingHost((currentOctave - 1 + 3) / 6.0f);
            osc3OctaveValueLabel.setText(juce::String(currentOctave - 1), juce::dontSendNotification);
        }
    };

    addAndMakeVisible(osc3OctaveLabel);
    osc3OctaveLabel.setText("Octave", juce::dontSendNotification);
    osc3OctaveLabel.setJustificationType(juce::Justification::centred);

    // Octave value display (shows current octave: -3 to +3)
    addAndMakeVisible(osc3OctaveValueLabel);
    osc3OctaveValueLabel.setText("0", juce::dontSendNotification);
    osc3OctaveValueLabel.setJustificationType(juce::Justification::centred);
    osc3OctaveValueLabel.setFont(juce::Font(14.0f, juce::Font::bold));

    // Pulse Width knob
    addAndMakeVisible(osc3PWSlider);
    osc3PWSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    osc3PWSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    osc3PWSlider.setRange(0.01, 0.99, 0.01);
    osc3PWSlider.setValue(0.5);

    addAndMakeVisible(osc3PWLabel);
    osc3PWLabel.setText("PWM", juce::dontSendNotification);
    osc3PWLabel.setJustificationType(juce::Justification::centredLeft);

    osc3PWAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.parameters, "osc3PW", osc3PWSlider);

    // ========== MIXER CONTROLS (NOISE) ==========
    // Noise label
    addAndMakeVisible(noiseLabel);
    noiseLabel.setText("Noise", juce::dontSendNotification);
    noiseLabel.setJustificationType(juce::Justification::centredLeft);

    // Enable button
    addAndMakeVisible(noiseEnableButton);
    noiseEnableButton.setButtonText("ON");
    noiseEnableButton.setClickingTogglesState(true);
    noiseEnableButton.setToggleState(false, juce::dontSendNotification);  // Default: OFF
    noiseEnableButton.setColour(juce::TextButton::buttonOnColourId, juce::Colours::red);
    noiseEnableButton.onClick = [this]() {
        audioProcessor.parameters.getParameter("noiseEnabled")->setValueNotifyingHost(
            noiseEnableButton.getToggleState() ? 1.0f : 0.0f);
    };

    // Noise type selector
    addAndMakeVisible(noiseTypeSelector);
    noiseTypeSelector.addItem("White", 1);
    noiseTypeSelector.addItem("Pink", 2);
    noiseTypeSelector.addItem("Brown", 3);
    noiseTypeSelector.setSelectedId(1);  // Default: White

    addAndMakeVisible(noiseTypeLabel);
    noiseTypeLabel.setText("Type", juce::dontSendNotification);
    noiseTypeLabel.setJustificationType(juce::Justification::centredLeft);

    noiseTypeAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(
        audioProcessor.parameters, "noiseType", noiseTypeSelector);

    // Gain knob
    addAndMakeVisible(noiseGainSlider);
    noiseGainSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    noiseGainSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    noiseGainSlider.setRange(0.0, 1.0, 0.01);
    noiseGainSlider.setValue(0.0);

    addAndMakeVisible(noiseGainLabel);
    noiseGainLabel.setText("Noise", juce::dontSendNotification);
    noiseGainLabel.setJustificationType(juce::Justification::centred);

    noiseGainAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.parameters, "noiseGain", noiseGainSlider);

    // Master Volume knob (bigger)
    addAndMakeVisible(masterVolumeSlider);
    masterVolumeSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    masterVolumeSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    masterVolumeSlider.setRange(0.0, 1.0, 0.01);
    masterVolumeSlider.setValue(0.8);

    addAndMakeVisible(masterVolumeLabel);
    masterVolumeLabel.setText("Master", juce::dontSendNotification);
    masterVolumeLabel.setJustificationType(juce::Justification::centred);

    masterVolumeAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.parameters, "masterVolume", masterVolumeSlider);

    // ========== FILTER CONTROLS ==========
    // Filter Mode buttons (radio button group)
    addAndMakeVisible(filterLowPassButton);
    filterLowPassButton.setButtonText("LP");
    filterLowPassButton.setClickingTogglesState(true);
    filterLowPassButton.setRadioGroupId(1001);
    filterLowPassButton.setToggleState(true, juce::dontSendNotification);  // Default: LowPass
    filterLowPassButton.onClick = [this]() {
        if (filterLowPassButton.getToggleState())
            audioProcessor.parameters.getParameter("filterMode")->setValueNotifyingHost(0.0f);
    };

    addAndMakeVisible(filterBandPassButton);
    filterBandPassButton.setButtonText("BP");
    filterBandPassButton.setClickingTogglesState(true);
    filterBandPassButton.setRadioGroupId(1001);
    filterBandPassButton.onClick = [this]() {
        if (filterBandPassButton.getToggleState())
            audioProcessor.parameters.getParameter("filterMode")->setValueNotifyingHost(0.5f);
    };

    addAndMakeVisible(filterHighPassButton);
    filterHighPassButton.setButtonText("HP");
    filterHighPassButton.setClickingTogglesState(true);
    filterHighPassButton.setRadioGroupId(1001);
    filterHighPassButton.onClick = [this]() {
        if (filterHighPassButton.getToggleState())
            audioProcessor.parameters.getParameter("filterMode")->setValueNotifyingHost(1.0f);
    };

    // Filter Cutoff knob
    addAndMakeVisible(filterCutoffSlider);
    filterCutoffSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    filterCutoffSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 70, 18);
    filterCutoffSlider.setRange(20.0, 12000.0, 0.1);  // Max 12 kHz (typical for analog Moog)
    filterCutoffSlider.setValue(1000.0);
    filterCutoffSlider.setTextValueSuffix(" Hz");
    filterCutoffSlider.setSkewFactorFromMidPoint(1000.0);  // Logarithmic scale

    addAndMakeVisible(filterCutoffLabel);
    filterCutoffLabel.setText("Cutoff", juce::dontSendNotification);
    filterCutoffLabel.setJustificationType(juce::Justification::centred);

    filterCutoffAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.parameters, "filterCutoff", filterCutoffSlider);

    // Filter Resonance knob
    addAndMakeVisible(filterResonanceSlider);
    filterResonanceSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    filterResonanceSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 18);
    filterResonanceSlider.setRange(0.0, 1.0, 0.01);
    filterResonanceSlider.setValue(0.0);
    filterResonanceSlider.setTextValueSuffix(" %");

    addAndMakeVisible(filterResonanceLabel);
    filterResonanceLabel.setText("Resonance", juce::dontSendNotification);
    filterResonanceLabel.setJustificationType(juce::Justification::centred);

    filterResonanceAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.parameters, "filterResonance", filterResonanceSlider);

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

    // ========== LFO 1 ==========
    // LFO 1 Label
    addAndMakeVisible(lfo1Label);
    lfo1Label.setText("LFO 1", juce::dontSendNotification);
    lfo1Label.setJustificationType(juce::Justification::centred);
    lfo1Label.setFont(juce::Font(16.0f, juce::Font::bold));

    // LFO 1 Waveform selector
    addAndMakeVisible(lfo1WaveformSelector);
    lfo1WaveformSelector.addItem("Sine", 1);
    lfo1WaveformSelector.addItem("Triangle", 2);
    lfo1WaveformSelector.addItem("Square", 3);
    lfo1WaveformSelector.addItem("Sawtooth", 4);
    lfo1WaveformSelector.addItem("S&H", 5);
    lfo1WaveformSelector.setSelectedId(1);  // Default: Sine

    addAndMakeVisible(lfo1WaveformLabel);
    lfo1WaveformLabel.setText("Wave", juce::dontSendNotification);
    lfo1WaveformLabel.setJustificationType(juce::Justification::centred);

    lfo1WaveformAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(
        audioProcessor.parameters, "lfo1Waveform", lfo1WaveformSelector);

    // LFO 1 Rate knob
    addAndMakeVisible(lfo1RateSlider);
    lfo1RateSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    lfo1RateSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 18);
    lfo1RateSlider.setRange(0.01, 20.0, 0.01);
    lfo1RateSlider.setValue(2.0);
    lfo1RateSlider.setTextValueSuffix(" Hz");
    lfo1RateSlider.setSkewFactorFromMidPoint(2.0);  // Logarithmic scale

    addAndMakeVisible(lfo1RateLabel);
    lfo1RateLabel.setText("Rate", juce::dontSendNotification);
    lfo1RateLabel.setJustificationType(juce::Justification::centred);

    lfo1RateAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.parameters, "lfo1Rate", lfo1RateSlider);

    // LFO 1 Depth knob
    addAndMakeVisible(lfo1DepthSlider);
    lfo1DepthSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    lfo1DepthSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 18);
    lfo1DepthSlider.setRange(0.0, 1.0, 0.01);
    lfo1DepthSlider.setValue(0.0);
    lfo1DepthSlider.setTextValueSuffix(" %");

    addAndMakeVisible(lfo1DepthLabel);
    lfo1DepthLabel.setText("Depth", juce::dontSendNotification);
    lfo1DepthLabel.setJustificationType(juce::Justification::centred);

    lfo1DepthAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.parameters, "lfo1Depth", lfo1DepthSlider);

    // LFO 1 Destination selector
    addAndMakeVisible(lfo1DestinationSelector);
    lfo1DestinationSelector.addItem("None", 1);
    lfo1DestinationSelector.addItem("Filter Cutoff", 2);
    lfo1DestinationSelector.addItem("Pitch", 3);
    lfo1DestinationSelector.addItem("PWM", 4);
    lfo1DestinationSelector.addItem("Filter Res", 5);
    lfo1DestinationSelector.addItem("Volume", 6);
    lfo1DestinationSelector.setSelectedId(1);  // Default: None

    addAndMakeVisible(lfo1DestinationLabel);
    lfo1DestinationLabel.setText("Destination", juce::dontSendNotification);
    lfo1DestinationLabel.setJustificationType(juce::Justification::centred);

    lfo1DestinationAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(
        audioProcessor.parameters, "lfo1Destination", lfo1DestinationSelector);

    // ========== LFO 2 ==========
    // LFO 2 Label
    addAndMakeVisible(lfo2Label);
    lfo2Label.setText("LFO 2", juce::dontSendNotification);
    lfo2Label.setJustificationType(juce::Justification::centred);
    lfo2Label.setFont(juce::Font(16.0f, juce::Font::bold));

    // LFO 2 Waveform selector
    addAndMakeVisible(lfo2WaveformSelector);
    lfo2WaveformSelector.addItem("Sine", 1);
    lfo2WaveformSelector.addItem("Triangle", 2);
    lfo2WaveformSelector.addItem("Square", 3);
    lfo2WaveformSelector.addItem("Sawtooth", 4);
    lfo2WaveformSelector.addItem("S&H", 5);
    lfo2WaveformSelector.setSelectedId(1);  // Default: Sine

    addAndMakeVisible(lfo2WaveformLabel);
    lfo2WaveformLabel.setText("Wave", juce::dontSendNotification);
    lfo2WaveformLabel.setJustificationType(juce::Justification::centred);

    lfo2WaveformAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(
        audioProcessor.parameters, "lfo2Waveform", lfo2WaveformSelector);

    // LFO 2 Rate knob
    addAndMakeVisible(lfo2RateSlider);
    lfo2RateSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    lfo2RateSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 18);
    lfo2RateSlider.setRange(0.01, 20.0, 0.01);
    lfo2RateSlider.setValue(5.0);
    lfo2RateSlider.setTextValueSuffix(" Hz");
    lfo2RateSlider.setSkewFactorFromMidPoint(2.0);  // Logarithmic scale

    addAndMakeVisible(lfo2RateLabel);
    lfo2RateLabel.setText("Rate", juce::dontSendNotification);
    lfo2RateLabel.setJustificationType(juce::Justification::centred);

    lfo2RateAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.parameters, "lfo2Rate", lfo2RateSlider);

    // LFO 2 Depth knob
    addAndMakeVisible(lfo2DepthSlider);
    lfo2DepthSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    lfo2DepthSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 18);
    lfo2DepthSlider.setRange(0.0, 1.0, 0.01);
    lfo2DepthSlider.setValue(0.0);
    lfo2DepthSlider.setTextValueSuffix(" %");

    addAndMakeVisible(lfo2DepthLabel);
    lfo2DepthLabel.setText("Depth", juce::dontSendNotification);
    lfo2DepthLabel.setJustificationType(juce::Justification::centred);

    lfo2DepthAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.parameters, "lfo2Depth", lfo2DepthSlider);

    // LFO 2 Destination selector
    addAndMakeVisible(lfo2DestinationSelector);
    lfo2DestinationSelector.addItem("None", 1);
    lfo2DestinationSelector.addItem("Filter Cutoff", 2);
    lfo2DestinationSelector.addItem("Pitch", 3);
    lfo2DestinationSelector.addItem("PWM", 4);
    lfo2DestinationSelector.addItem("Filter Res", 5);
    lfo2DestinationSelector.addItem("Volume", 6);
    lfo2DestinationSelector.setSelectedId(1);  // Default: None

    addAndMakeVisible(lfo2DestinationLabel);
    lfo2DestinationLabel.setText("Destination", juce::dontSendNotification);
    lfo2DestinationLabel.setJustificationType(juce::Justification::centred);

    lfo2DestinationAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(
        audioProcessor.parameters, "lfo2Destination", lfo2DestinationSelector);

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
    g.drawFittedText("CLEMMY3", getLocalBounds().removeFromTop(35), juce::Justification::centred, 1);

    // Draw subtitle
    g.setFont(14.0f);
    g.setColour(juce::Colours::lightgrey);
    g.drawFittedText("Phase 6: Dual LFO Modulation", getLocalBounds().removeFromTop(60), juce::Justification::centred, 1);

    // ========== SECTION BORDERS FOR REFERENCE ==========
    g.setColour(juce::Colours::yellow.withAlpha(0.5f));

    // Header section (title + voice mode)
    g.drawRect(0, 0, getWidth(), 132, 2);  // 132px

    // Row 1: Oscillators + Mixer + Filter (reduced by 40px)
    g.drawRect(0, 132, getWidth(), 350, 2);  // 390 - 40 = 350px

    // Row 2: ADSR + LFO placeholder (increased by 40px)
    int keyboardTop = getHeight() - 50;  // Keyboard is 50px tall at bottom
    int row2Top = keyboardTop - 5 - 242;  // 5px spacing + 242px row (202 + 40)
    g.drawRect(0, row2Top, getWidth(), 242, 2);

    // Keyboard section
    g.drawRect(0, keyboardTop, getWidth(), 50, 2);
}

void CLEMMY3AudioProcessorEditor::resized()
{
    auto area = getLocalBounds();

    // ========== TITLE AREA ========== (Total: 132px = 90 + 42 extra)
    area.removeFromTop(75);  // More space for title and subtitle (55 + 20 extra)

    // ========== VOICE MODE SELECTOR ==========
    auto voiceModeArea = area.removeFromTop(35);  // Increased from 30 to 35
    voiceModeArea.reduce(15, 0);
    // "Mode:" label
    voiceModeLabel.setBounds(voiceModeArea.removeFromLeft(50));
    voiceModeArea.removeFromLeft(5);
    // Three buttons: MONO | POLY | UNI
    int modeBtnWidth = 50;
    int modeBtnSpacing = 5;
    voiceModeMonoButton.setBounds(voiceModeArea.removeFromLeft(modeBtnWidth));
    voiceModeArea.removeFromLeft(modeBtnSpacing);
    voiceModePolyButton.setBounds(voiceModeArea.removeFromLeft(modeBtnWidth));
    voiceModeArea.removeFromLeft(modeBtnSpacing);
    voiceModeUnisonButton.setBounds(voiceModeArea.removeFromLeft(modeBtnWidth));

    area.removeFromTop(22);  // More spacing (was 5, now 22 to use extra header space)

    // ========== TOP ROW: OSCILLATORS on left | MIXER + FILTER stacked on right ==========
    auto topRow = area.removeFromTop(350);  // Reduced by 40px (was 390)
    topRow.reduce(15, 0);

    // --- OSCILLATORS SECTION (left side) ---
    auto oscillatorsArea = topRow.removeFromLeft(540);  // Slightly wider for oscillators

    int oscWidth = 165;
    int oscSpacing = 10;

    // Helper lambda for oscillator column layout
    auto layoutOscillator = [&](juce::Rectangle<int> col,
                                juce::TextButton& enableBtn,
                                juce::ComboBox& waveSelector, juce::Label& waveLabel,
                                juce::Slider& detuneSlider, juce::Label& detuneLabel,
                                juce::TextButton& octaveUpBtn, juce::TextButton& octaveDownBtn, juce::Label& octaveLabel, juce::Label& octaveValueLabel,
                                juce::Slider& pwSlider, juce::Label& pwLabel)
    {
        col.removeFromTop(10);  // Extra top padding to lower controls into boxes

        // Waveform selector - centered (no label, just selector)
        waveLabel.setBounds(0, 0, 0, 0);  // Hide Wave label
        auto waveRow = col.removeFromTop(25);
        int waveSelectorWidth = 120;
        int waveMargin = (oscWidth - waveSelectorWidth) / 2;
        waveRow.removeFromLeft(waveMargin);
        waveSelector.setBounds(waveRow.removeFromLeft(waveSelectorWidth));
        col.removeFromTop(5);

        // Enable button (centered)
        auto enableRow = col.removeFromTop(20);
        int enableBtnWidth = 60;
        int enableMargin = (oscWidth - enableBtnWidth) / 2;
        enableRow.removeFromLeft(enableMargin);
        enableBtn.setBounds(enableRow.removeFromLeft(enableBtnWidth));
        col.removeFromTop(10);

        // Detune knob (centered, full width)
        detuneLabel.setBounds(col.removeFromTop(15));
        detuneSlider.setBounds(col.removeFromTop(100));
        col.removeFromTop(10);

        // Octave buttons (horizontal) with value display in center (no label)
        auto octaveRow = col.removeFromTop(25);
        int btnWidth = 28;
        int valWidth = 35;
        int margin = (oscWidth - btnWidth*2 - valWidth) / 2;
        octaveRow.removeFromLeft(margin);
        octaveDownBtn.setBounds(octaveRow.removeFromLeft(btnWidth));
        octaveValueLabel.setBounds(octaveRow.removeFromLeft(valWidth));  // Value display in center
        octaveUpBtn.setBounds(octaveRow.removeFromLeft(btnWidth));
        octaveLabel.setBounds(0, 0, 0, 0);  // Hide octave label
        col.removeFromTop(10);

        // PWM knob below octave buttons (centered)
        pwLabel.setBounds(col.removeFromTop(15));  // PWM label on top
        auto pwmRow = col.removeFromTop(50);  // Height for PWM knob
        int pwKnobSize = 50;
        int pwMargin = (oscWidth - pwKnobSize) / 2;
        pwmRow.removeFromLeft(pwMargin);
        pwSlider.setBounds(pwmRow.removeFromLeft(pwKnobSize).removeFromTop(pwKnobSize));  // Centered PWM knob
    };

    // Layout 3 oscillators side by side
    auto osc1Area = oscillatorsArea.removeFromLeft(oscWidth);
    layoutOscillator(osc1Area, osc1EnableButton,
                    osc1WaveformSelector, osc1WaveformLabel,
                    osc1DetuneSlider, osc1DetuneLabel,
                    osc1OctaveUpButton, osc1OctaveDownButton, osc1OctaveLabel, osc1OctaveValueLabel,
                    osc1PWSlider, osc1PWLabel);

    oscillatorsArea.removeFromLeft(oscSpacing);

    auto osc2Area = oscillatorsArea.removeFromLeft(oscWidth);
    layoutOscillator(osc2Area, osc2EnableButton,
                    osc2WaveformSelector, osc2WaveformLabel,
                    osc2DetuneSlider, osc2DetuneLabel,
                    osc2OctaveUpButton, osc2OctaveDownButton, osc2OctaveLabel, osc2OctaveValueLabel,
                    osc2PWSlider, osc2PWLabel);

    oscillatorsArea.removeFromLeft(oscSpacing);

    auto osc3Area = oscillatorsArea;
    layoutOscillator(osc3Area, osc3EnableButton,
                    osc3WaveformSelector, osc3WaveformLabel,
                    osc3DetuneSlider, osc3DetuneLabel,
                    osc3OctaveUpButton, osc3OctaveDownButton, osc3OctaveLabel, osc3OctaveValueLabel,
                    osc3PWSlider, osc3PWLabel);

    topRow.removeFromLeft(15);  // Spacing before right column

    // --- RIGHT COLUMN: MIXER (top) and FILTER (bottom) stacked vertically ---
    auto rightColumn = topRow;  // All remaining width

    // === MIXER SECTION (top half of right column) ===
    auto mixerArea = rightColumn.removeFromTop(180);  // Increased height for noise section
    mixerArea.removeFromTop(10);  // Top padding to fit in box

    int mixerKnobSize = 70;   // Slightly larger knobs
    int mixerColWidth = 75;   // Width per knob column
    int mixerSpacing = 8;     // Spacing between knobs

    // Row 1: Labels for all 5 knobs
    auto labelRow = mixerArea.removeFromTop(16);
    osc1GainLabel.setBounds(labelRow.removeFromLeft(mixerColWidth));
    labelRow.removeFromLeft(mixerSpacing);
    osc2GainLabel.setBounds(labelRow.removeFromLeft(mixerColWidth));
    labelRow.removeFromLeft(mixerSpacing);
    osc3GainLabel.setBounds(labelRow.removeFromLeft(mixerColWidth));
    labelRow.removeFromLeft(mixerSpacing);
    noiseGainLabel.setBounds(labelRow.removeFromLeft(mixerColWidth));
    labelRow.removeFromLeft(mixerSpacing);
    masterVolumeLabel.setBounds(labelRow.removeFromLeft(mixerColWidth));

    mixerArea.removeFromTop(5);

    // Row 2: All 5 knobs in horizontal row (same size)
    auto knobRow = mixerArea.removeFromTop(mixerKnobSize);
    osc1GainSlider.setBounds(knobRow.removeFromLeft(mixerColWidth));
    knobRow.removeFromLeft(mixerSpacing);
    osc2GainSlider.setBounds(knobRow.removeFromLeft(mixerColWidth));
    knobRow.removeFromLeft(mixerSpacing);
    osc3GainSlider.setBounds(knobRow.removeFromLeft(mixerColWidth));
    knobRow.removeFromLeft(mixerSpacing);
    noiseGainSlider.setBounds(knobRow.removeFromLeft(mixerColWidth));
    knobRow.removeFromLeft(mixerSpacing);
    masterVolumeSlider.setBounds(knobRow.removeFromLeft(mixerColWidth));

    mixerArea.removeFromTop(12);  // More spacing

    // Row 3: Noise label, enable button, and type selector
    auto noiseControlsRow = mixerArea.removeFromTop(22);
    noiseLabel.setBounds(noiseControlsRow.removeFromLeft(48));
    noiseControlsRow.removeFromLeft(3);
    noiseEnableButton.setBounds(noiseControlsRow.removeFromLeft(40));  // Same width as LP/BP/HP
    noiseControlsRow.removeFromLeft(8);
    noiseTypeLabel.setBounds(noiseControlsRow.removeFromLeft(35));
    noiseControlsRow.removeFromLeft(3);
    noiseTypeSelector.setBounds(noiseControlsRow.removeFromLeft(80));  // Narrower dropdown

    rightColumn.removeFromTop(10);  // Spacing between mixer and filter

    // === FILTER SECTION (bottom half of right column) ===
    auto filterArea = rightColumn;  // All remaining height

    filterArea.removeFromTop(25);  // Top padding to fit in box

    // Single row: [LP] [BP] [HP] | Cutoff (big) | Resonance (small)
    auto filterRow = filterArea.removeFromTop(90);

    // Filter mode buttons (LP/BP/HP)
    int btnWidth = 40;
    int btnHeight = 25;
    int btnSpacing = 5;

    filterLowPassButton.setBounds(filterRow.removeFromLeft(btnWidth).removeFromTop(btnHeight));
    filterRow.removeFromLeft(btnSpacing);
    filterBandPassButton.setBounds(filterRow.removeFromLeft(btnWidth).removeFromTop(btnHeight));
    filterRow.removeFromLeft(btnSpacing);
    filterHighPassButton.setBounds(filterRow.removeFromLeft(btnWidth).removeFromTop(btnHeight));

    filterRow.removeFromLeft(15);  // Spacing between buttons and knobs

    // Cutoff knob (larger)
    auto cutoffCol = filterRow.removeFromLeft(110);
    filterCutoffLabel.setBounds(cutoffCol.removeFromTop(16));
    filterCutoffSlider.setBounds(cutoffCol);

    filterRow.removeFromLeft(12);  // Spacing

    // Resonance knob (smaller)
    auto resonanceCol = filterRow.removeFromLeft(85);
    filterResonanceLabel.setBounds(resonanceCol.removeFromTop(16));
    filterResonanceSlider.setBounds(resonanceCol);

    // ========== MIDI KEYBOARD (half height, at very bottom) ==========
    auto keyboardArea = area.removeFromBottom(50);  // Half the previous height, at bottom
    keyboardArea.reduce(10, 0);
    midiKeyboard.setBounds(keyboardArea);

    area.removeFromBottom(5);  // Small spacing above keyboard

    // ========== BOTTOM ROW: ADSR | LFO PLACEHOLDER (snug to keyboard) ==========
    auto bottomRow = area.removeFromBottom(242);  // Increased by 40px (was 202)
    bottomRow.reduce(15, 10);  // Add padding: 15px horizontal, 10px vertical

    // --- ADSR ENVELOPE ---
    auto adsrArea = bottomRow.removeFromLeft(360);

    int adsrSliderWidth = 80;
    int adsrSpacing = 12;

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

    auto releaseArea = adsrArea;
    releaseLabel.setBounds(releaseArea.removeFromTop(15));
    releaseSlider.setBounds(releaseArea);

    // --- LFO SECTION ---
    bottomRow.removeFromLeft(20);  // Spacing between ADSR and LFOs

    // Split remaining space into 2 LFO sections
    int lfoWidth = (bottomRow.getWidth() - 15) / 2;  // 15px spacing between LFOs
    auto lfo1Area = bottomRow.removeFromLeft(lfoWidth);
    bottomRow.removeFromLeft(15);  // Spacing between LFO1 and LFO2
    auto lfo2Area = bottomRow;

    // --- LFO 1 Layout ---
    // LFO 1 Label
    lfo1Label.setBounds(lfo1Area.removeFromTop(20));
    lfo1Area.removeFromTop(5);

    // Waveform selector
    lfo1WaveformLabel.setBounds(lfo1Area.removeFromTop(14));
    lfo1WaveformSelector.setBounds(lfo1Area.removeFromTop(22));
    lfo1Area.removeFromTop(8);

    // Rate and Depth knobs side by side
    auto lfo1KnobsRow = lfo1Area.removeFromTop(85);
    int lfo1KnobWidth = (lfo1KnobsRow.getWidth() - 10) / 2;  // 10px spacing

    auto lfo1RateCol = lfo1KnobsRow.removeFromLeft(lfo1KnobWidth);
    lfo1RateLabel.setBounds(lfo1RateCol.removeFromTop(14));
    lfo1RateSlider.setBounds(lfo1RateCol);

    lfo1KnobsRow.removeFromLeft(10);  // Spacing

    auto lfo1DepthCol = lfo1KnobsRow;
    lfo1DepthLabel.setBounds(lfo1DepthCol.removeFromTop(14));
    lfo1DepthSlider.setBounds(lfo1DepthCol);

    lfo1Area.removeFromTop(8);

    // Destination selector
    lfo1DestinationLabel.setBounds(lfo1Area.removeFromTop(14));
    lfo1DestinationSelector.setBounds(lfo1Area.removeFromTop(22));

    // --- LFO 2 Layout ---
    // LFO 2 Label
    lfo2Label.setBounds(lfo2Area.removeFromTop(20));
    lfo2Area.removeFromTop(5);

    // Waveform selector
    lfo2WaveformLabel.setBounds(lfo2Area.removeFromTop(14));
    lfo2WaveformSelector.setBounds(lfo2Area.removeFromTop(22));
    lfo2Area.removeFromTop(8);

    // Rate and Depth knobs side by side
    auto lfo2KnobsRow = lfo2Area.removeFromTop(85);
    int lfo2KnobWidth = (lfo2KnobsRow.getWidth() - 10) / 2;  // 10px spacing

    auto lfo2RateCol = lfo2KnobsRow.removeFromLeft(lfo2KnobWidth);
    lfo2RateLabel.setBounds(lfo2RateCol.removeFromTop(14));
    lfo2RateSlider.setBounds(lfo2RateCol);

    lfo2KnobsRow.removeFromLeft(10);  // Spacing

    auto lfo2DepthCol = lfo2KnobsRow;
    lfo2DepthLabel.setBounds(lfo2DepthCol.removeFromTop(14));
    lfo2DepthSlider.setBounds(lfo2DepthCol);

    lfo2Area.removeFromTop(8);

    // Destination selector
    lfo2DestinationLabel.setBounds(lfo2Area.removeFromTop(14));
    lfo2DestinationSelector.setBounds(lfo2Area.removeFromTop(22));
}
