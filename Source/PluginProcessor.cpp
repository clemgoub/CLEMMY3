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

    // ==================== OSCILLATOR 1 PARAMETERS ====================
    params.push_back(std::make_unique<juce::AudioParameterBool>(
        "osc1Enabled", "Osc 1 Enabled", true));

    params.push_back(std::make_unique<juce::AudioParameterChoice>(
        "osc1Waveform", "Osc 1 Waveform",
        juce::StringArray{"Sine", "Sawtooth", "Square", "Triangle"},
        0));  // Default: Sine

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        "osc1Gain", "Osc 1 Gain",
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f),
        0.33f));  // Default: 33%

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        "osc1Detune", "Osc 1 Detune",
        juce::NormalisableRange<float>(-100.0f, 100.0f, 0.1f),
        0.0f));  // Default: 0 cents

    params.push_back(std::make_unique<juce::AudioParameterInt>(
        "osc1Octave", "Osc 1 Octave",
        -3, 3, 0));  // Default: 0 octaves

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        "osc1PW", "Osc 1 Pulse Width",
        juce::NormalisableRange<float>(0.01f, 0.99f, 0.01f),
        0.5f));  // Default: 50%

    // ==================== OSCILLATOR 2 PARAMETERS ====================
    params.push_back(std::make_unique<juce::AudioParameterBool>(
        "osc2Enabled", "Osc 2 Enabled", true));

    params.push_back(std::make_unique<juce::AudioParameterChoice>(
        "osc2Waveform", "Osc 2 Waveform",
        juce::StringArray{"Sine", "Sawtooth", "Square", "Triangle"},
        1));  // Default: Sawtooth

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        "osc2Gain", "Osc 2 Gain",
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f),
        0.33f));  // Default: 33%

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        "osc2Detune", "Osc 2 Detune",
        juce::NormalisableRange<float>(-100.0f, 100.0f, 0.1f),
        0.0f));  // Default: 0 cents

    params.push_back(std::make_unique<juce::AudioParameterInt>(
        "osc2Octave", "Osc 2 Octave",
        -3, 3, 0));  // Default: 0 octaves

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        "osc2PW", "Osc 2 Pulse Width",
        juce::NormalisableRange<float>(0.01f, 0.99f, 0.01f),
        0.5f));  // Default: 50%

    // ==================== OSCILLATOR 3 PARAMETERS ====================
    params.push_back(std::make_unique<juce::AudioParameterBool>(
        "osc3Enabled", "Osc 3 Enabled", true));

    params.push_back(std::make_unique<juce::AudioParameterChoice>(
        "osc3Waveform", "Osc 3 Waveform",
        juce::StringArray{"Sine", "Sawtooth", "Square", "Triangle"},
        2));  // Default: Square

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        "osc3Gain", "Osc 3 Gain",
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f),
        0.33f));  // Default: 33%

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        "osc3Detune", "Osc 3 Detune",
        juce::NormalisableRange<float>(-100.0f, 100.0f, 0.1f),
        0.0f));  // Default: 0 cents

    params.push_back(std::make_unique<juce::AudioParameterInt>(
        "osc3Octave", "Osc 3 Octave",
        -3, 3, 0));  // Default: 0 octaves

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        "osc3PW", "Osc 3 Pulse Width",
        juce::NormalisableRange<float>(0.01f, 0.99f, 0.01f),
        0.5f));  // Default: 50%

    // ==================== ADSR ENVELOPE PARAMETERS ====================
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        "attack", "Attack",
        juce::NormalisableRange<float>(0.001f, 2.0f, 0.001f, 0.3f),
        0.01f));  // Default: 10ms

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        "decay", "Decay",
        juce::NormalisableRange<float>(0.001f, 2.0f, 0.001f, 0.3f),
        0.3f));  // Default: 300ms

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        "sustain", "Sustain",
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f),
        0.7f));  // Default: 70%

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        "release", "Release",
        juce::NormalisableRange<float>(0.001f, 5.0f, 0.001f, 0.3f),
        0.5f));  // Default: 500ms

    // ==================== NOISE PARAMETERS ====================
    params.push_back(std::make_unique<juce::AudioParameterBool>(
        "noiseEnabled", "Noise Enabled", false));  // Default: OFF

    params.push_back(std::make_unique<juce::AudioParameterChoice>(
        "noiseType", "Noise Type",
        juce::StringArray{"White", "Pink", "Brown"},
        0));  // Default: White

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        "noiseGain", "Noise Gain",
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f),
        0.0f));  // Default: 0%

    // ==================== MASTER VOLUME ====================
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        "masterVolume", "Master Volume",
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f),
        0.8f));  // Default: 80%

    // ==================== FILTER PARAMETERS ====================
    params.push_back(std::make_unique<juce::AudioParameterChoice>(
        "filterMode", "Filter Mode",
        juce::StringArray{"LowPass", "BandPass", "HighPass"},
        0));  // Default: LowPass

    // Logarithmic cutoff range (20 Hz - 12 kHz)
    // Max 12 kHz is typical for analog Moog filters (Minimoog, Prophet-5, etc.)
    // Middle value = 1000 Hz for good default
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        "filterCutoff", "Filter Cutoff",
        juce::NormalisableRange<float>(20.0f, 12000.0f, 0.1f, 0.25f),  // 0.25 = logarithmic skew
        1000.0f));  // Default: 1000 Hz

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        "filterResonance", "Filter Resonance",
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f),
        0.0f));  // Default: 0% (no resonance)

    // ==================== LFO 1 PARAMETERS ====================
    params.push_back(std::make_unique<juce::AudioParameterChoice>(
        "lfo1Waveform", "LFO 1 Waveform",
        juce::StringArray{"Sine", "Triangle", "Square", "Sawtooth", "S&H"},
        0));  // Default: Sine

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        "lfo1Rate", "LFO 1 Rate",
        juce::NormalisableRange<float>(0.01f, 20.0f, 0.01f, 0.3f),  // Logarithmic
        2.0f));  // Default: 2 Hz

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        "lfo1Depth", "LFO 1 Depth",
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f),
        0.0f));  // Default: 0% (off)

    params.push_back(std::make_unique<juce::AudioParameterChoice>(
        "lfo1Destination", "LFO 1 Destination",
        juce::StringArray{"None", "Filter Cutoff", "Pitch", "PWM", "Filter Res", "Volume"},
        0));  // Default: None

    // ==================== LFO 2 PARAMETERS ====================
    params.push_back(std::make_unique<juce::AudioParameterChoice>(
        "lfo2Waveform", "LFO 2 Waveform",
        juce::StringArray{"Sine", "Triangle", "Square", "Sawtooth", "S&H"},
        0));  // Default: Sine

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        "lfo2Rate", "LFO 2 Rate",
        juce::NormalisableRange<float>(0.01f, 20.0f, 0.01f, 0.3f),  // Logarithmic
        4.0f));  // Default: 4 Hz

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        "lfo2Depth", "LFO 2 Depth",
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f),
        0.0f));  // Default: 0% (off)

    params.push_back(std::make_unique<juce::AudioParameterChoice>(
        "lfo2Destination", "LFO 2 Destination",
        juce::StringArray{"None", "Filter Cutoff", "Pitch", "PWM", "Filter Res", "Volume"},
        0));  // Default: None

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

    // Oscillator 1 parameters
    bool osc1Enabled = parameters.getRawParameterValue("osc1Enabled")->load() > 0.5f;
    int osc1Waveform = parameters.getRawParameterValue("osc1Waveform")->load();
    float osc1Gain = parameters.getRawParameterValue("osc1Gain")->load();
    float osc1Detune = parameters.getRawParameterValue("osc1Detune")->load();
    int osc1Octave = static_cast<int>(parameters.getRawParameterValue("osc1Octave")->load());
    float osc1PW = parameters.getRawParameterValue("osc1PW")->load();

    // Oscillator 2 parameters
    bool osc2Enabled = parameters.getRawParameterValue("osc2Enabled")->load() > 0.5f;
    int osc2Waveform = parameters.getRawParameterValue("osc2Waveform")->load();
    float osc2Gain = parameters.getRawParameterValue("osc2Gain")->load();
    float osc2Detune = parameters.getRawParameterValue("osc2Detune")->load();
    int osc2Octave = static_cast<int>(parameters.getRawParameterValue("osc2Octave")->load());
    float osc2PW = parameters.getRawParameterValue("osc2PW")->load();

    // Oscillator 3 parameters
    bool osc3Enabled = parameters.getRawParameterValue("osc3Enabled")->load() > 0.5f;
    int osc3Waveform = parameters.getRawParameterValue("osc3Waveform")->load();
    float osc3Gain = parameters.getRawParameterValue("osc3Gain")->load();
    float osc3Detune = parameters.getRawParameterValue("osc3Detune")->load();
    int osc3Octave = static_cast<int>(parameters.getRawParameterValue("osc3Octave")->load());
    float osc3PW = parameters.getRawParameterValue("osc3PW")->load();

    // ADSR Envelope parameters
    float attack = parameters.getRawParameterValue("attack")->load();
    float decay = parameters.getRawParameterValue("decay")->load();
    float sustain = parameters.getRawParameterValue("sustain")->load();
    float release = parameters.getRawParameterValue("release")->load();

    // Noise parameters
    bool noiseEnabled = parameters.getRawParameterValue("noiseEnabled")->load() > 0.5f;
    int noiseTypeIndex = parameters.getRawParameterValue("noiseType")->load();
    float noiseGain = parameters.getRawParameterValue("noiseGain")->load();

    // Filter parameters
    int filterModeIndex = parameters.getRawParameterValue("filterMode")->load();
    float filterCutoff = parameters.getRawParameterValue("filterCutoff")->load();
    float filterResonance = parameters.getRawParameterValue("filterResonance")->load();

    // Update voice manager mode
    voiceManager.setVoiceMode(static_cast<VoiceManager::VoiceMode>(voiceModeIndex));

    // Broadcast oscillator 1 parameters to all voices
    voiceManager.setOscillatorEnabled(0, osc1Enabled);
    voiceManager.setOscillatorWaveform(0, static_cast<Oscillator::Waveform>(osc1Waveform));
    voiceManager.setOscillatorGain(0, osc1Gain);
    voiceManager.setOscillatorDetune(0, osc1Detune);
    voiceManager.setOscillatorOctave(0, osc1Octave);
    voiceManager.setOscillatorPulseWidth(0, osc1PW);

    // Broadcast oscillator 2 parameters to all voices
    voiceManager.setOscillatorEnabled(1, osc2Enabled);
    voiceManager.setOscillatorWaveform(1, static_cast<Oscillator::Waveform>(osc2Waveform));
    voiceManager.setOscillatorGain(1, osc2Gain);
    voiceManager.setOscillatorDetune(1, osc2Detune);
    voiceManager.setOscillatorOctave(1, osc2Octave);
    voiceManager.setOscillatorPulseWidth(1, osc2PW);

    // Broadcast oscillator 3 parameters to all voices
    voiceManager.setOscillatorEnabled(2, osc3Enabled);
    voiceManager.setOscillatorWaveform(2, static_cast<Oscillator::Waveform>(osc3Waveform));
    voiceManager.setOscillatorGain(2, osc3Gain);
    voiceManager.setOscillatorDetune(2, osc3Detune);
    voiceManager.setOscillatorOctave(2, osc3Octave);
    voiceManager.setOscillatorPulseWidth(2, osc3PW);

    // Broadcast envelope parameters
    voiceManager.setEnvelopeParameters(attack, decay, sustain, release);

    // Broadcast noise parameters
    voiceManager.setNoiseEnabled(noiseEnabled);
    voiceManager.setNoiseType(static_cast<NoiseGenerator::NoiseType>(noiseTypeIndex));
    voiceManager.setNoiseGain(noiseGain);

    // Broadcast filter parameters
    voiceManager.setFilterMode(static_cast<MoogFilter::Mode>(filterModeIndex));
    voiceManager.setFilterCutoff(filterCutoff);
    voiceManager.setFilterResonance(filterResonance);

    // LFO parameters
    int lfo1WaveformIndex = parameters.getRawParameterValue("lfo1Waveform")->load();
    float lfo1Rate = parameters.getRawParameterValue("lfo1Rate")->load();
    float lfo1Depth = parameters.getRawParameterValue("lfo1Depth")->load();
    int lfo1Destination = parameters.getRawParameterValue("lfo1Destination")->load();

    int lfo2WaveformIndex = parameters.getRawParameterValue("lfo2Waveform")->load();
    float lfo2Rate = parameters.getRawParameterValue("lfo2Rate")->load();
    float lfo2Depth = parameters.getRawParameterValue("lfo2Depth")->load();
    int lfo2Destination = parameters.getRawParameterValue("lfo2Destination")->load();

    // Broadcast LFO parameters
    voiceManager.setLFO1Waveform(static_cast<LFO::Waveform>(lfo1WaveformIndex));
    voiceManager.setLFO1Rate(lfo1Rate);
    voiceManager.setLFO1Depth(lfo1Depth);
    voiceManager.setLFO1Destination(lfo1Destination);

    voiceManager.setLFO2Waveform(static_cast<LFO::Waveform>(lfo2WaveformIndex));
    voiceManager.setLFO2Rate(lfo2Rate);
    voiceManager.setLFO2Depth(lfo2Depth);
    voiceManager.setLFO2Destination(lfo2Destination);

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

    // Read master volume parameter
    float masterVolume = parameters.getRawParameterValue("masterVolume")->load();

    // Generate audio samples from voice manager
    for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
    {
        // Get mixed output from all active voices
        float output = voiceManager.processSample() * 0.3f * masterVolume;

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
