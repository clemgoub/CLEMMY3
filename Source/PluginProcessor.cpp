#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "DSP/AudioUtils.h"

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
                       ),
#else
    :
#endif
      parameters(*this, nullptr, "PARAMETERS", createParameterLayout())
{
}

CLEMMY3AudioProcessor::~CLEMMY3AudioProcessor()
{
}

//==============================================================================
juce::AudioProcessorValueTreeState::ParameterLayout CLEMMY3AudioProcessor::createParameterLayout()
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;

    // Waveform parameter (Sine=0, Sawtooth=1, Square=2)
    params.push_back(std::make_unique<juce::AudioParameterChoice>(
        "waveform",
        "Waveform",
        juce::StringArray{"Sine", "Sawtooth", "Square"},
        0));  // Default: Sine

    // Pulse Width parameter (for Square wave)
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        "pulseWidth",
        "Pulse Width",
        juce::NormalisableRange<float>(0.01f, 0.99f, 0.01f),
        0.5f));  // Default: 50%

    return { params.begin(), params.end() };
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

void CLEMMY3AudioProcessor::setCurrentProgram(int)
{
}

const juce::String CLEMMY3AudioProcessor::getProgramName(int)
{
    return {};
}

void CLEMMY3AudioProcessor::changeProgramName(int, const juce::String&)
{
}

//==============================================================================
void CLEMMY3AudioProcessor::prepareToPlay(double sampleRate, int)
{
    // Initialize oscillator with sample rate
    oscillator.setSampleRate(sampleRate);

    // Reset state
    noteIsOn = false;
    currentMidiNote = -1;
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

    // Merge MIDI from virtual keyboard with incoming MIDI
    juce::MidiBuffer virtualKeyboardMidi;
    keyboardState.processNextMidiBuffer(virtualKeyboardMidi, 0, buffer.getNumSamples(), true);

    // Combine both MIDI sources
    juce::MidiBuffer combinedMidi;
    combinedMidi.addEvents(midiMessages, 0, buffer.getNumSamples(), 0);
    combinedMidi.addEvents(virtualKeyboardMidi, 0, buffer.getNumSamples(), 0);

    // Get current parameter values
    int waveformIndex = parameters.getRawParameterValue("waveform")->load();
    float pulseWidth = parameters.getRawParameterValue("pulseWidth")->load();

    // Update oscillator parameters
    oscillator.setWaveform(static_cast<Oscillator::Waveform>(waveformIndex));
    oscillator.setPulseWidth(pulseWidth);

    // Process MIDI messages (from both sources)
    for (const auto metadata : combinedMidi)
    {
        auto message = metadata.getMessage();

        if (message.isNoteOn())
        {
            currentMidiNote = message.getNoteNumber();
            float frequency = AudioUtils::midiNoteToFrequency(currentMidiNote);
            oscillator.setFrequency(frequency);
            noteIsOn = true;
        }
        else if (message.isNoteOff())
        {
            if (message.getNoteNumber() == currentMidiNote)
            {
                noteIsOn = false;
                currentMidiNote = -1;
            }
        }
    }

    // Generate audio samples
    if (noteIsOn)
    {
        for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
        {
            float oscSample = oscillator.processSample();

            // Output to all channels
            for (int channel = 0; channel < totalNumOutputChannels; ++channel)
            {
                buffer.setSample(channel, sample, oscSample * 0.3f);
            }
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
    // Save parameters
    auto state = parameters.copyState();
    std::unique_ptr<juce::XmlElement> xml(state.createXml());
    copyXmlToBinary(*xml, destData);
}

void CLEMMY3AudioProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    // Restore parameters
    std::unique_ptr<juce::XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));

    if (xmlState.get() != nullptr)
    {
        if (xmlState->hasTagName(parameters.state.getType()))
        {
            parameters.replaceState(juce::ValueTree::fromXml(*xmlState));
        }
    }
}

//==============================================================================
// This creates new instances of the plugin
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new CLEMMY3AudioProcessor();
}
