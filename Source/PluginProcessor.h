#pragma once

#include <juce_audio_processors/juce_audio_processors.h>

//==============================================================================
/**
    CLEMMY3 - Triple Oscillator Synthesizer
    Phase 0: Test implementation with 440Hz sine wave
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

private:
    //==============================================================================
    // Phase 0: Test sine wave oscillator
    double phase = 0.0;
    bool noteIsOn = false;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(CLEMMY3AudioProcessor)
};
