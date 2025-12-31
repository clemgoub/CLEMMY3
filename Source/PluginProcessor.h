#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include "DSP/VoiceManager.h"

//==============================================================================
/**
    CLEMMY3 - Triple Oscillator Synthesizer
    Phase 3: Voice Polyphony & Management
*/
class CLEMMY3AudioProcessor : public juce::AudioProcessor
{
public:
    //==============================================================================
    CLEMMY3AudioProcessor();
    ~CLEMMY3AudioProcessor() override;

    //==============================================================================
    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

#ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported(const BusesLayout& layouts) const override;
#endif

    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram(int index) override;
    const juce::String getProgramName(int index) override;
    void changeProgramName(int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation(juce::MemoryBlock& destData) override;
    void setStateInformation(const void* data, int sizeInBytes) override;

    //==============================================================================
    // Parameters (public for editor access)
    juce::AudioProcessorValueTreeState parameters;

    // MIDI keyboard state for virtual keyboard
    juce::MidiKeyboardState& getKeyboardState() { return keyboardState; }

private:
    juce::MidiKeyboardState keyboardState;
    //==============================================================================
    // Phase 3: Polyphonic voice management
    VoiceManager voiceManager;

    // Host tempo for LFO MIDI sync
    float currentBPM = 120.0f;          // Tempo from host

    juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(CLEMMY3AudioProcessor)
};
