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

    // Voice Mode parameter (Mono=0, Poly=1, Unison=2)
    params.push_back(std::make_unique<juce::AudioParameterChoice>(
        "voiceMode",
        "Voice Mode",
        juce::StringArray{"Mono", "Poly", "Unison"},
        1));  // Default: Poly

    // Waveform parameter (Sine=0, Sawtooth=1, Square=2, Triangle=3, Noise=4)
    params.push_back(std::make_unique<juce::AudioParameterChoice>(
        "waveform",
        "Waveform",
        juce::StringArray{"Sine", "Sawtooth", "Square", "Triangle", "Noise"},
        0));  // Default: Sine

    // Pulse Width parameter (for Square wave)
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        "pulseWidth",
        "Pulse Width",
        juce::NormalisableRange<float>(0.01f, 0.99f, 0.01f),
        0.5f));  // Default: 50%

    // ADSR Envelope parameters
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        "attack",
        "Attack",
        juce::NormalisableRange<float>(0.001f, 2.0f, 0.001f, 0.3f),  // Skewed for better control
        0.01f));  // Default: 10ms

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        "decay",
        "Decay",
        juce::NormalisableRange<float>(0.001f, 2.0f, 0.001f, 0.3f),
        0.3f));  // Default: 300ms

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        "sustain",
        "Sustain",
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f),
        0.7f));  // Default: 70%

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        "release",
        "Release",
        juce::NormalisableRange<float>(0.001f, 5.0f, 0.001f, 0.3f),
        0.5f));  // Default: 500ms

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
    // Initialize voice manager with sample rate
    voiceManager.setSampleRate(sampleRate);
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
    int voiceModeIndex = parameters.getRawParameterValue("voiceMode")->load();
    int waveformIndex = parameters.getRawParameterValue("waveform")->load();
    float pulseWidth = parameters.getRawParameterValue("pulseWidth")->load();
    float attack = parameters.getRawParameterValue("attack")->load();
    float decay = parameters.getRawParameterValue("decay")->load();
    float sustain = parameters.getRawParameterValue("sustain")->load();
    float release = parameters.getRawParameterValue("release")->load();

    // Update voice manager mode
    voiceManager.setVoiceMode(static_cast<VoiceManager::VoiceMode>(voiceModeIndex));

    // Broadcast parameters to all voices
    voiceManager.setOscillatorWaveform(static_cast<Oscillator::Waveform>(waveformIndex));
    voiceManager.setOscillatorPulseWidth(pulseWidth);
    voiceManager.setEnvelopeParameters(attack, decay, sustain, release);

    // Process MIDI messages (from both sources)
    for (const auto metadata : combinedMidi)
    {
        auto message = metadata.getMessage();

        if (message.isNoteOn())
        {
            int midiNote = message.getNoteNumber();
            float velocity = message.getFloatVelocity();
            voiceManager.noteOn(midiNote, velocity);
        }
        else if (message.isNoteOff())
        {
            int midiNote = message.getNoteNumber();
            voiceManager.noteOff(midiNote);
        }
    }

    // Generate audio samples from voice manager
    for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
    {
        // Get mixed output from all active voices
        float output = voiceManager.processSample() * 0.3f;

        // Output to all channels
        for (int channel = 0; channel < totalNumOutputChannels; ++channel)
        {
            buffer.setSample(channel, sample, output);
        }
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
