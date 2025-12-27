#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
CLEMMY3AudioProcessor::CLEMMY3AudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor(BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       )
#endif
{
}

CLEMMY3AudioProcessor::~CLEMMY3AudioProcessor()
{
}

//==============================================================================
const juce::String CLEMMY3AudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool CLEMMY3AudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool CLEMMY3AudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool CLEMMY3AudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double CLEMMY3AudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int CLEMMY3AudioProcessor::getNumPrograms()
{
    return 1;
}

int CLEMMY3AudioProcessor::getCurrentProgram()
{
    return 0;
}

void CLEMMY3AudioProcessor::setCurrentProgram(int index)
{
}

const juce::String CLEMMY3AudioProcessor::getProgramName(int index)
{
    return {};
}

void CLEMMY3AudioProcessor::changeProgramName(int index, const juce::String& newName)
{
}

//==============================================================================
void CLEMMY3AudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    // Reset phase when playback starts
    phase = 0.0;
    noteIsOn = false;
}

void CLEMMY3AudioProcessor::releaseResources()
{
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool CLEMMY3AudioProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused(layouts);
    return true;
  #else
    // This is where you check if the layout is supported.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void CLEMMY3AudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    // Clear any output channels that didn't contain input data
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear(i, 0, buffer.getNumSamples());

    // Process MIDI messages
    for (const auto metadata : midiMessages)
    {
        auto message = metadata.getMessage();

        if (message.isNoteOn())
        {
            noteIsOn = true;
        }
        else if (message.isNoteOff())
        {
            noteIsOn = false;
        }
    }

    // Generate 440Hz sine wave if note is on
    if (noteIsOn)
    {
        const float frequency = 440.0f;
        const float sampleRate = static_cast<float>(getSampleRate());
        const float phaseIncrement = frequency / sampleRate;

        for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
        {
            float sineValue = std::sin(phase * 2.0f * juce::MathConstants<float>::pi);

            // Output to all channels
            for (int channel = 0; channel < totalNumOutputChannels; ++channel)
            {
                buffer.setSample(channel, sample, sineValue * 0.3f);
            }

            // Increment phase
            phase += phaseIncrement;
            if (phase >= 1.0)
                phase -= 1.0;
        }
    }
    else
    {
        // No note playing, output silence
        buffer.clear();
    }
}

//==============================================================================
bool CLEMMY3AudioProcessor::hasEditor() const
{
    return true;
}

juce::AudioProcessorEditor* CLEMMY3AudioProcessor::createEditor()
{
    return new CLEMMY3AudioProcessorEditor(*this);
}

//==============================================================================
void CLEMMY3AudioProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    // Save plugin state (to be implemented in later phases)
}

void CLEMMY3AudioProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    // Restore plugin state (to be implemented in later phases)
}

//==============================================================================
// This creates new instances of the plugin
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new CLEMMY3AudioProcessor();
}
