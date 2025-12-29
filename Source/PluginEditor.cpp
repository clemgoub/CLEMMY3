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
    setSize(1100, 750);  // Increased width by 200px

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
    noiseGainLabel.setText("Gain", juce::dontSendNotification);
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
    g.drawFittedText("Phase 5: Moog Ladder Filter", getLocalBounds().removeFromTop(70), juce::Justification::centred, 1);

    // Draw section labels
    g.setFont(12.0f);
    g.setColour(juce::Colours::white);

    // Oscillator section labels (positioned above each column)
    g.drawText("OSCILLATOR 1", 20, 100, 200, 20, juce::Justification::centred);
    g.drawText("OSCILLATOR 2", 235, 100, 200, 20, juce::Justification::centred);
    g.drawText("OSCILLATOR 3", 450, 100, 200, 20, juce::Justification::centred);

    // Right column labels (mixer top, filter bottom)
    int rightColX = 570;
    int rightColWidth = 515;
    g.drawText("MIXER", rightColX, 100, rightColWidth, 20, juce::Justification::centred);
    g.drawText("FILTER", rightColX, 250, rightColWidth, 20, juce::Justification::centred);

    // ADSR Envelope section label
    g.drawText("ADSR ENVELOPE", 20, 460, 400, 20, juce::Justification::centred);

    // LFO placeholder
    g.setColour(juce::Colours::grey);
    g.drawText("LFO MODULATION (Coming Soon)", 440, 460, 640, 20, juce::Justification::centred);

    // ========== SECTION BORDERS ==========
    g.setColour(juce::Colours::darkgrey.withAlpha(0.5f));

    // Top row starts at y=70 (after title + voice selector)
    int topRowY = 70;
    int topRowHeight = 360;

    // Oscillator borders (3 columns, each 165px wide with 10px spacing)
    int oscWidth = 165;
    int oscSpacing = 10;
    int oscStartX = 15;

    // Oscillator 1 border
    g.drawRect(oscStartX, topRowY + 25, oscWidth, topRowHeight - 25, 1);

    // Oscillator 2 border
    g.drawRect(oscStartX + oscWidth + oscSpacing, topRowY + 25, oscWidth, topRowHeight - 25, 1);

    // Oscillator 3 border
    g.drawRect(oscStartX + (oscWidth + oscSpacing) * 2, topRowY + 25, oscWidth, topRowHeight - 25, 1);

    // Right column sections (mixer + filter stacked vertically)
    int rightColumnX = oscStartX + 540 + 15;  // After oscillators + spacing
    int rightColumnWidth = 1100 - rightColumnX - 15;  // To right edge minus margin

    // Mixer border (top section of right column)
    int mixerHeight = 140;
    g.drawRect(rightColumnX, topRowY + 25, rightColumnWidth, mixerHeight, 1);

    // Filter border (bottom section of right column, with 10px spacing)
    int filterY = topRowY + 25 + mixerHeight + 10;
    int filterHeight = topRowHeight - 25 - mixerHeight - 10;
    g.drawRect(rightColumnX, filterY, rightColumnWidth, filterHeight, 1);

    // ADSR Envelope border (bottom row left side)
    int adsrY = topRowY + topRowHeight + 10;
    int adsrWidth = 400;
    int adsrHeight = 180;
    g.drawRect(15, adsrY + 25, adsrWidth, adsrHeight, 1);

    // LFO placeholder border (bottom row right side)
    int lfoX = 15 + adsrWidth + 15;
    int lfoWidth = 1100 - lfoX - 15;
    g.drawRect(lfoX, adsrY + 25, lfoWidth, adsrHeight, 1);
}

void CLEMMY3AudioProcessorEditor::resized()
{
    auto area = getLocalBounds();

    // ========== TITLE AREA ==========
    area.removeFromTop(40);  // Reduced title space

    // ========== VOICE MODE SELECTOR ==========
    auto voiceModeArea = area.removeFromTop(25);
    voiceModeArea.reduce(15, 0);
    voiceModeArea.removeFromLeft(80);  // Space for label
    voiceModeSelector.setBounds(voiceModeArea.removeFromLeft(120));

    area.removeFromTop(5);  // Small spacing

    // ========== TOP ROW: OSCILLATORS on left | MIXER + FILTER stacked on right ==========
    auto topRow = area.removeFromTop(360);
    topRow.reduce(15, 0);

    // --- OSCILLATORS SECTION (left side) ---
    auto oscillatorsArea = topRow.removeFromLeft(540);  // Slightly wider for oscillators

    int oscWidth = 165;
    int oscSpacing = 10;

    // Helper lambda for oscillator column layout
    auto layoutOscillator = [&](juce::Rectangle<int> col,
                                juce::ToggleButton& enableBtn,
                                juce::ComboBox& waveSelector, juce::Label& waveLabel,
                                juce::Slider& detuneSlider, juce::Label& detuneLabel,
                                juce::TextButton& octaveUpBtn, juce::TextButton& octaveDownBtn, juce::Label& octaveLabel, juce::Label& octaveValueLabel,
                                juce::Slider& pwSlider, juce::Label& pwLabel)
    {
        // Waveform selector (label on left)
        auto waveRow = col.removeFromTop(25);
        waveLabel.setBounds(waveRow.removeFromLeft(45));
        waveRow.removeFromLeft(3);
        waveSelector.setBounds(waveRow);
        col.removeFromTop(5);

        // Enable button
        enableBtn.setBounds(col.removeFromTop(20));
        col.removeFromTop(10);

        // Detune knob (larger)
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

        // PWM label and knob side by side
        auto pwmRow = col.removeFromTop(70);
        pwLabel.setBounds(pwmRow.removeFromLeft(40));  // PWM label on left
        pwmRow.removeFromLeft(5);
        pwSlider.setBounds(pwmRow);  // Knob takes remaining space
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
    auto mixerArea = rightColumn.removeFromTop(140);  // Reduced height

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

    mixerArea.removeFromTop(8);

    // Row 3: Noise enable and type selector
    noiseEnableButton.setBounds(mixerArea.removeFromTop(18));
    mixerArea.removeFromTop(3);
    auto noiseTypeRow = mixerArea.removeFromTop(20);
    noiseTypeLabel.setBounds(noiseTypeRow.removeFromLeft(40));
    noiseTypeRow.removeFromLeft(5);
    noiseTypeSelector.setBounds(noiseTypeRow.removeFromLeft(120));

    rightColumn.removeFromTop(10);  // Spacing between mixer and filter

    // === FILTER SECTION (bottom half of right column) ===
    auto filterArea = rightColumn;  // All remaining height

    filterArea.removeFromTop(15);  // Top padding

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

    // ========== BOTTOM ROW: ADSR | LFO PLACEHOLDER ==========
    area.removeFromTop(5);  // Less spacing
    auto bottomRow = area.removeFromTop(180);  // Increased from 150 to 180
    bottomRow.reduce(15, 0);

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

    // LFO section placeholder (future implementation)
    // bottomRow has remaining space

    // ========== MIDI KEYBOARD (uses all remaining space, sticks to bottom) ==========
    auto keyboardArea = area;  // Use all remaining space
    keyboardArea.reduce(10, 5);
    midiKeyboard.setBounds(keyboardArea);
}
