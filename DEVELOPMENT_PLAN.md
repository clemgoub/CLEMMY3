# CLEMMY3 VST3/AU Plugin - Comprehensive Development Plan

## Executive Summary

**Project:** Port of Python Triple Oscillator synthesizer to professional VST3/AU plugin
**Source:** https://github.com/clemgoub/TripleOscillator
**Technology Stack:** C++17, JUCE Framework 7.x, CMake
**Target Platforms:** macOS (AU/VST3), Windows (VST3), Linux (VST3)
**Plugin Formats:** VST3, AU, Standalone

---

## Development Philosophy

### Iterative Development Approach
- Build incrementally with validation at each step
- Each phase produces a working, testable plugin
- Validate against Python reference implementation continuously
- Maintain audio quality and performance as top priorities

### Quality Gates
Every phase must pass:
1. **Compilation Gate:** Code builds without warnings
2. **Functional Gate:** Feature works as specified
3. **Audio Gate:** No artifacts, glitches, or quality degradation
4. **Performance Gate:** Real-time capable (< 5% CPU at 64 sample buffer)
5. **Stability Gate:** No crashes, memory leaks, or undefined behavior

---

## Project Structure

```
CLEMMY3/
├── README.md                          # Project overview, build instructions
├── LICENSE                            # License file
├── CMakeLists.txt                     # CMake build configuration
├── .gitignore                         # C++ build artifacts, IDE files
│
├── Docs/
│   ├── DEVELOPMENT_PLAN.md            # This file - comprehensive roadmap
│   ├── ARCHITECTURE.md                # System design, signal flow diagrams
│   ├── PYTHON_REFERENCE.md            # Python→C++ mapping documentation
│   ├── BUILD_GUIDE.md                 # Platform-specific build instructions
│   ├── TESTING_STRATEGY.md            # Testing procedures and validation
│   └── API_REFERENCE.md               # C++ class documentation
│
├── Source/
│   ├── PluginProcessor.h/cpp          # Main audio processing (JUCE entry point)
│   ├── PluginEditor.h/cpp             # GUI implementation
│   │
│   ├── DSP/                           # Digital Signal Processing components
│   │   ├── Oscillator.h/cpp           # Waveform generation (Sine/Saw/Square + PolyBLEP)
│   │   ├── Envelope.h/cpp             # ADSR envelope generator
│   │   ├── Voice.h/cpp                # Single voice (3 oscillators + envelope)
│   │   ├── VoiceManager.h/cpp         # Polyphony, voice allocation, stealing
│   │   ├── MoogFilter.h/cpp           # 4-pole Moog ladder filter (LP/BP/HP)
│   │   ├── LFO.h/cpp                  # Low-frequency oscillator (5 waveforms)
│   │   ├── NoiseGenerator.h/cpp       # White/Pink/Brown noise
│   │   └── AudioUtils.h/cpp           # Shared utilities (PolyBLEP, conversions)
│   │
│   ├── Parameters/                    # Parameter management
│   │   ├── SynthParameters.h/cpp      # All synth parameters and ranges
│   │   └── ParameterHelpers.h/cpp     # Value smoothing, mapping utilities
│   │
│   └── UI/                            # User interface components
│       ├── Components/
│       │   ├── OscillatorPanel.h/cpp
│       │   ├── FilterPanel.h/cpp
│       │   ├── LFOPanel.h/cpp
│       │   ├── EnvelopePanel.h/cpp
│       │   └── PresetBrowser.h/cpp
│       └── LookAndFeel/
│           └── CustomLookAndFeel.h/cpp
│
├── Resources/                         # Plugin assets
│   ├── Presets/
│   │   ├── Init.vstpreset
│   │   └── Factory/                   # Factory preset library
│   └── Images/
│       └── (UI graphics, backgrounds)
│
└── Tests/                             # Unit tests (optional but recommended)
    └── DSP/
        ├── OscillatorTests.cpp
        ├── EnvelopeTests.cpp
        └── FilterTests.cpp
```

---

## Phase 0: Foundation & Project Setup

### Objectives
- Establish working development environment
- Create buildable JUCE project skeleton
- Verify toolchain with minimal test implementation
- Set up version control and documentation structure

### Prerequisites
- C++17 compatible compiler (Xcode 12+, Visual Studio 2019+, GCC 9+, Clang 10+)
- CMake 3.15 or higher
- DAW for testing (Reaper recommended - free evaluation)
- JUCE Framework 7.x

### Task Breakdown

#### 1. Environment Setup
- [ ] Install JUCE Framework
  - Download from https://juce.com/get-juce/download
  - Extract to `~/JUCE` (macOS/Linux) or `C:\JUCE` (Windows)
  - Set `JUCE_DIR` environment variable
- [ ] Install build tools
  - macOS: Xcode Command Line Tools
  - Windows: Visual Studio 2019+ with C++ workload
  - Linux: `build-essential`, `libasound2-dev`, `libfreetype6-dev`, etc.
- [ ] Install CMake (3.15+)
- [ ] Clone and familiarize with Python reference repo

#### 2. Project Initialization
- [ ] Create JUCE Audio Plugin project using Projucer or CMake template
  - **Plugin Name:** "CLEMMY3"
  - **Manufacturer:** Your name/company
  - **Plugin Code:** "Clm3" (4-char unique ID)
  - **Manufacturer Code:** "Yxyz" (4-char unique ID)
  - **Plugin Formats:** VST3, AU, Standalone
  - **MIDI Input:** Enabled
  - **MIDI Output:** Disabled
  - **Plugin is a Synth:** Yes
  - **Plugin wants MIDI input:** Yes
- [ ] Configure CMakeLists.txt for cross-platform builds
- [ ] Set up .gitignore for C++ builds
  - Build directories
  - IDE project files
  - Binary outputs
  - User-specific settings

#### 3. Initial Documentation
- [ ] Create `Docs/ARCHITECTURE.md` with initial signal flow diagram
- [ ] Create `Docs/BUILD_GUIDE.md` with platform-specific instructions
- [ ] Create `Docs/PYTHON_REFERENCE.md` with line number mappings
- [ ] Update README.md with project goals and status

#### 4. Test Implementation - 440Hz Sine Wave
Implement minimal synthesizer in `PluginProcessor.cpp`:

```cpp
void PluginProcessor::processBlock(juce::AudioBuffer<float>& buffer,
                                     juce::MidiBuffer& midiMessages)
{
    buffer.clear();

    // Simple test: 440Hz sine wave
    const float frequency = 440.0f;
    const float sampleRate = getSampleRate();
    const float phaseIncrement = frequency / sampleRate;

    bool noteIsOn = false;
    for (const auto metadata : midiMessages)
    {
        auto message = metadata.getMessage();
        if (message.isNoteOn())
            noteIsOn = true;
        else if (message.isNoteOff())
            noteIsOn = false;
    }

    if (!noteIsOn)
        return;

    for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
    {
        float sineValue = std::sin(phase * 2.0f * juce::MathConstants<float>::pi);

        for (int channel = 0; channel < buffer.getNumChannels(); ++channel)
            buffer.setSample(channel, sample, sineValue * 0.3f);

        phase += phaseIncrement;
        if (phase >= 1.0f)
            phase -= 1.0f;
    }
}
```

#### 5. Build and Test
- [ ] Build project for your platform
- [ ] Run standalone app
- [ ] Load VST3 in DAW
- [ ] Test MIDI input responds
- [ ] Verify 440Hz sine wave with spectrum analyzer
- [ ] Check for audio glitches, dropouts, or instability

### Validation Checklist
- [ ] Project compiles without errors or warnings
- [ ] Standalone application launches successfully
- [ ] VST3 loads in DAW without errors
- [ ] AU loads in Logic/GarageBand (macOS only)
- [ ] MIDI note-on triggers audio
- [ ] 440Hz sine wave is pure (spectrum shows only fundamental)
- [ ] No audio glitches, clicks, or dropouts
- [ ] CPU usage reasonable (< 1% idle)

### Deliverables
- Working JUCE project structure
- Buildable code producing test sine wave
- Complete documentation framework
- Git repository initialized with initial commit

### Python Reference
N/A (initial project setup)

### Common Issues & Solutions

**Issue:** JUCE not found by CMake
**Solution:** Set `JUCE_DIR` environment variable to JUCE installation path

**Issue:** VST3 doesn't show up in DAW
**Solution:** Check plugin is copied to correct location:
- macOS: `~/Library/Audio/Plug-Ins/VST3/`
- Windows: `C:\Program Files\Common Files\VST3\`
- Linux: `~/.vst3/`

**Issue:** AU validation fails (macOS)
**Solution:** Run `auval -v aumu Clm3 Yxyz` to see detailed validation errors

---

## Phase 1: Single Oscillator Engine

### Objectives
- Implement one high-quality oscillator
- Support Sine, Sawtooth, and Square waveforms
- Add PolyBLEP anti-aliasing for Sawtooth and Square
- MIDI note-to-frequency conversion
- Pulse width modulation for Square wave

### Prerequisites
- Phase 0 complete and validated
- Understanding of aliasing and PolyBLEP algorithm
- Reference Python implementation: `sine_generator_qt.py:640-680` (PolyBLEP), `3555-3615` (waveform generation)

### Task Breakdown

#### 1. Create AudioUtils Module
**File:** `Source/DSP/AudioUtils.h/cpp`

Implement utility functions:
- [ ] `midiNoteToFrequency(int midiNote)` - MIDI to Hz conversion
- [ ] `polyBLEP(double t, double dt)` - Anti-aliasing function
- [ ] `wrapPhase(double& phase)` - Phase wrapping helper

```cpp
// AudioUtils.h
#pragma once

namespace AudioUtils
{
    // Convert MIDI note number to frequency in Hz
    inline float midiNoteToFrequency(int midiNote)
    {
        return 440.0f * std::pow(2.0f, (midiNote - 69) / 12.0f);
    }

    // PolyBLEP anti-aliasing (Polynomal Band-Limited Step)
    // t: normalized phase (0-1), dt: normalized frequency
    inline float polyBLEP(double t, double dt)
    {
        // Port from Python: sine_generator_qt.py:640-680
        if (t < dt)
        {
            t /= dt;
            return t + t - t * t - 1.0f;
        }
        else if (t > 1.0 - dt)
        {
            t = (t - 1.0) / dt;
            return t * t + t + t + 1.0f;
        }
        return 0.0f;
    }

    // Wrap phase to 0-1 range
    inline void wrapPhase(double& phase)
    {
        while (phase >= 1.0)
            phase -= 1.0;
        while (phase < 0.0)
            phase += 1.0;
    }
}
```

#### 2. Implement Oscillator Class
**File:** `Source/DSP/Oscillator.h/cpp`

- [ ] Define Oscillator class interface
- [ ] Implement sine wave generation
- [ ] Implement sawtooth with PolyBLEP
- [ ] Implement square wave with PolyBLEP and PWM
- [ ] Add parameter setters (frequency, waveform, pulse width)

```cpp
// Oscillator.h
#pragma once
#include <JuceHeader.h>

class Oscillator
{
public:
    enum class Waveform
    {
        Sine,
        Sawtooth,
        Square
    };

    Oscillator();

    void setSampleRate(double sampleRate);
    void setFrequency(float frequency);
    void setWaveform(Waveform waveform);
    void setPulseWidth(float pw);  // 0.01 - 0.99 for Square wave

    float processSample();
    void reset();  // Reset phase to 0

private:
    double phase = 0.0;
    double phaseIncrement = 0.0;
    double sampleRate = 44100.0;

    float frequency = 440.0f;
    Waveform waveform = Waveform::Sine;
    float pulseWidth = 0.5f;

    float generateSine();
    float generateSawtooth();
    float generateSquare();

    void updatePhaseIncrement();
};
```

Key implementation details:
- Phase increment: `phaseIncrement = frequency / sampleRate`
- Phase runs from 0.0 to 1.0 (not 0 to 2π for PolyBLEP compatibility)
- PolyBLEP applied at discontinuities only

#### 3. Integrate into PluginProcessor
**File:** `Source/PluginProcessor.h/cpp`

- [ ] Add Oscillator member variable
- [ ] Handle MIDI note-on/off events
- [ ] Update oscillator frequency from MIDI note
- [ ] Generate audio samples in processBlock

```cpp
// In PluginProcessor.h
private:
    Oscillator oscillator;
    bool noteIsOn = false;

// In PluginProcessor.cpp
void PluginProcessor::processBlock(juce::AudioBuffer<float>& buffer,
                                     juce::MidiBuffer& midiMessages)
{
    buffer.clear();

    // Process MIDI events
    for (const auto metadata : midiMessages)
    {
        auto message = metadata.getMessage();

        if (message.isNoteOn())
        {
            float frequency = AudioUtils::midiNoteToFrequency(message.getNoteNumber());
            oscillator.setFrequency(frequency);
            noteIsOn = true;
        }
        else if (message.isNoteOff())
        {
            noteIsOn = false;
        }
    }

    if (!noteIsOn)
        return;

    // Generate audio samples
    for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
    {
        float oscillatorSample = oscillator.processSample();

        for (int channel = 0; channel < buffer.getNumChannels(); ++channel)
            buffer.setSample(channel, sample, oscillatorSample * 0.3f);
    }
}
```

#### 4. Add Basic Parameters
- [ ] Create waveform parameter (Choice: Sine/Sawtooth/Square)
- [ ] Create pulse width parameter (Float: 0.01 - 0.99, default 0.5)
- [ ] Connect parameters to oscillator

### Validation Checklist

**Sine Wave:**
- [ ] Pure sine tone (spectrum analyzer shows only fundamental frequency)
- [ ] No harmonics or noise
- [ ] Correct pitch for all MIDI notes (verify A4 = 440Hz)

**Sawtooth Wave:**
- [ ] Bright, buzzy timbre
- [ ] No aliasing artifacts in high frequencies
- [ ] PolyBLEP smooths discontinuities
- [ ] Spectrum shows falling harmonic series

**Square Wave:**
- [ ] Hollow, clarinet-like timbre
- [ ] No aliasing artifacts
- [ ] Pulse width modulation changes timbre
- [ ] At 50% duty cycle, only odd harmonics present

**General:**
- [ ] No clicks when changing waveform
- [ ] No clicks when changing frequency
- [ ] Phase is continuous when parameters change
- [ ] Works across full MIDI range (0-127)
- [ ] No audio dropouts or glitches

### Python Reference
- `sine_generator_qt.py:640-680` - PolyBLEP implementation
- `sine_generator_qt.py:3555-3615` - `generate_waveform()`
- `sine_generator_qt.py:3679-3750` - `process_oscillator()`

### Key Differences from Python
- **No NumPy:** Use scalar per-sample processing instead of vectorized arrays
- **Phase Range:** Use 0.0-1.0 instead of 0-2π for PolyBLEP
- **Memory:** Pre-allocate all state variables (no dynamic allocation in audio thread)

---

## Phase 2: ADSR Envelope

### Objectives
- Implement smooth ADSR envelope generator
- Support velocity sensitivity
- Handle rapid note retriggering
- Prevent clicks with minimum attack time

### Prerequisites
- Phase 1 complete and validated
- Understanding of envelope state machines
- Reference Python implementation: `sine_generator_qt.py:180-260`

### Task Breakdown

#### 1. Implement Envelope Class
**File:** `Source/DSP/Envelope.h/cpp`

```cpp
class Envelope
{
public:
    enum class Phase
    {
        Idle,
        Attack,
        Decay,
        Sustain,
        Release
    };

    Envelope();

    void setSampleRate(double sampleRate);
    void setParameters(float attack, float decay, float sustain, float release);

    void noteOn(float velocity = 1.0f);
    void noteOff();
    void reset();

    float processSample();  // Returns envelope level 0.0 - 1.0
    bool isActive() const { return currentPhase != Phase::Idle; }
    Phase getCurrentPhase() const { return currentPhase; }

private:
    Phase currentPhase = Phase::Idle;
    float currentLevel = 0.0f;
    float targetLevel = 0.0f;

    // Time parameters (in seconds)
    float attackTime = 0.01f;
    float decayTime = 0.3f;
    float sustainLevel = 0.7f;
    float releaseTime = 0.5f;

    // Calculated rates (per-sample increments)
    float attackRate = 0.0f;
    float decayRate = 0.0f;
    float releaseRate = 0.0f;

    float velocity = 1.0f;
    double sampleRate = 44100.0;

    void calculateRates();
    void enterPhase(Phase newPhase);
};
```

#### 2. Envelope State Machine Logic

Implement state transitions:
- **Idle → Attack:** On note-on
- **Attack → Decay:** When level reaches 1.0
- **Decay → Sustain:** When level reaches sustain level
- **Sustain → Release:** On note-off
- **Release → Idle:** When level reaches 0.0
- **Any Phase → Attack:** On note retrigger

```cpp
float Envelope::processSample()
{
    switch (currentPhase)
    {
        case Phase::Idle:
            return 0.0f;

        case Phase::Attack:
            currentLevel += attackRate;
            if (currentLevel >= 1.0f)
            {
                currentLevel = 1.0f;
                enterPhase(Phase::Decay);
            }
            break;

        case Phase::Decay:
            currentLevel -= decayRate;
            if (currentLevel <= sustainLevel)
            {
                currentLevel = sustainLevel;
                enterPhase(Phase::Sustain);
            }
            break;

        case Phase::Sustain:
            currentLevel = sustainLevel;
            break;

        case Phase::Release:
            currentLevel -= releaseRate;
            if (currentLevel <= 0.0f)
            {
                currentLevel = 0.0f;
                enterPhase(Phase::Idle);
            }
            break;
    }

    return currentLevel * velocity;
}

void Envelope::calculateRates()
{
    // Minimum attack time to prevent clicks
    const float minAttackTime = 0.005f;  // 5ms
    float safeAttackTime = std::max(attackTime, minAttackTime);

    // Calculate per-sample increments
    attackRate = 1.0f / (safeAttackTime * sampleRate);
    decayRate = (1.0f - sustainLevel) / (decayTime * sampleRate);
    releaseRate = sustainLevel / (releaseTime * sampleRate);
}
```

#### 3. Integrate with PluginProcessor

Update processBlock to apply envelope:

```cpp
void PluginProcessor::processBlock(juce::AudioBuffer<float>& buffer,
                                     juce::MidiBuffer& midiMessages)
{
    buffer.clear();

    for (const auto metadata : midiMessages)
    {
        auto message = metadata.getMessage();

        if (message.isNoteOn())
        {
            float frequency = AudioUtils::midiNoteToFrequency(message.getNoteNumber());
            oscillator.setFrequency(frequency);
            envelope.noteOn(message.getFloatVelocity());
        }
        else if (message.isNoteOff())
        {
            envelope.noteOff();
        }
    }

    for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
    {
        float oscSample = oscillator.processSample();
        float envLevel = envelope.processSample();
        float output = oscSample * envLevel;

        for (int channel = 0; channel < buffer.getNumChannels(); ++channel)
            buffer.setSample(channel, sample, output);
    }
}
```

#### 4. Add Envelope Parameters
- [ ] Attack time (0.001 - 2.0 seconds, default 0.01)
- [ ] Decay time (0.001 - 2.0 seconds, default 0.3)
- [ ] Sustain level (0.0 - 1.0, default 0.7)
- [ ] Release time (0.001 - 5.0 seconds, default 0.5)

Use logarithmic scaling for time parameters.

### Validation Checklist

**Attack Phase:**
- [ ] Attack time affects onset speed correctly
- [ ] No clicks even with very fast attack (< 10ms)
- [ ] Attack ramp is smooth and linear
- [ ] Velocity affects final attack level

**Decay Phase:**
- [ ] Decay time affects decay speed correctly
- [ ] Decay reaches sustain level accurately
- [ ] Smooth transition from attack to decay

**Sustain Phase:**
- [ ] Sustain level holds steady
- [ ] Sustain level parameter changes level correctly
- [ ] Long notes maintain sustain without drift

**Release Phase:**
- [ ] Release time affects fade-out speed correctly
- [ ] Release works when triggered during any phase
- [ ] Release from sustain is smooth
- [ ] Release reaches zero without clicks

**Edge Cases:**
- [ ] Rapid note retriggering works (no stuck notes)
- [ ] Note-off during attack phase transitions to release
- [ ] Multiple note-ons retrigger envelope correctly
- [ ] Very short notes (< 50ms) behave correctly

### Python Reference
- `sine_generator_qt.py:180-260` - Complete EnvelopeGenerator class
- State machine logic and phase transitions
- Rate calculation from time parameters

---

## Phase 3: Voice Polyphony & Voice Management

### Objectives
- Implement polyphonic voice architecture (8 voices)
- Voice allocation and stealing (LRU algorithm)
- Support MONO, POLY, and UNISON modes
- Proper voice lifecycle management

### Prerequisites
- Phases 1-2 complete and validated
- Understanding of voice stealing algorithms
- Reference Python implementation: `sine_generator_qt.py:100-177` (Voice), `4050-4140` (MIDI), `4230-4260` (stealing)

### Task Breakdown

#### 1. Create Voice Class
**File:** `Source/DSP/Voice.h/cpp`

Encapsulate single voice (oscillator + envelope):

```cpp
class Voice
{
public:
    Voice();

    void setSampleRate(double sampleRate);

    // Voice lifecycle
    void noteOn(int midiNote, float velocity, float unisonDetune = 0.0f);
    void noteOff();
    void reset();

    // Parameter updates
    void setOscillatorWaveform(Oscillator::Waveform waveform);
    void setOscillatorPulseWidth(float pw);
    void setEnvelopeParameters(float attack, float decay, float sustain, float release);

    // Audio generation
    float processSample();

    // Voice state queries
    bool isActive() const;
    bool isSounding() const;  // Has audible output
    int getCurrentNote() const { return currentMidiNote; }
    int getAge() const { return age; }

    void incrementAge() { ++age; }
    void resetAge() { age = 0; }

private:
    Oscillator oscillator;
    Envelope envelope;

    int currentMidiNote = -1;  // -1 = voice is free
    int age = 0;               // For voice stealing (LRU)
    float unisonDetune = 0.0f; // Unison detuning in cents

    void updateFrequency();
};
```

Implementation notes:
- `isActive()`: Returns true if envelope is not idle
- `isSounding()`: Returns true if envelope level > threshold (e.g., 0.001)
- Age increments every audio callback for voice stealing

#### 2. Create VoiceManager Class
**File:** `Source/DSP/VoiceManager.h/cpp`

Manages voice pool and allocation:

```cpp
class VoiceManager
{
public:
    enum class VoiceMode
    {
        Mono,      // Single voice, last note priority
        Poly,      // Up to MAX_VOICES polyphony
        Unison     // All voices play same note, detuned
    };

    static constexpr int MAX_VOICES = 8;

    VoiceManager();

    void setSampleRate(double sampleRate);
    void setVoiceMode(VoiceMode mode);

    // MIDI handling
    void noteOn(int midiNote, float velocity);
    void noteOff(int midiNote);
    void allNotesOff();
    void allSoundOff();  // Immediate silence

    // Parameter broadcasting to all voices
    void setOscillatorWaveform(Oscillator::Waveform waveform);
    void setOscillatorPulseWidth(float pw);
    void setEnvelopeParameters(float attack, float decay, float sustain, float release);

    // Audio generation
    float processSample();

    int getNumActiveVoices() const;

private:
    std::array<Voice, MAX_VOICES> voices;
    VoiceMode voiceMode = VoiceMode::Poly;

    // Voice allocation
    Voice* findFreeVoice();
    Voice* findVoicePlayingNote(int midiNote);
    Voice* stealVoice();

    // Mode-specific allocation
    void allocateMonoVoice(int midiNote, float velocity);
    void allocatePolyVoice(int midiNote, float velocity);
    void allocateUnisonVoices(int midiNote, float velocity);

    // Unison detuning
    float calculateUnisonDetune(int voiceIndex) const;

    void incrementAllAges();
};
```

#### 3. Voice Stealing Algorithm (LRU)

Least Recently Used voice stealing:

```cpp
Voice* VoiceManager::stealVoice()
{
    Voice* oldest = nullptr;
    int maxAge = -1;

    for (auto& voice : voices)
    {
        if (voice.isActive() && voice.getAge() > maxAge)
        {
            maxAge = voice.getAge();
            oldest = &voice;
        }
    }

    // If no active voices found (shouldn't happen), steal first
    return oldest ? oldest : &voices[0];
}

Voice* VoiceManager::findFreeVoice()
{
    for (auto& voice : voices)
    {
        if (!voice.isActive())
            return &voice;
    }

    return nullptr;  // No free voices
}
```

#### 4. Voice Mode Implementations

**MONO Mode:**
- Only one voice active at a time
- New notes retrigger envelope on same voice
- Last note priority (most recent note takes precedence)

```cpp
void VoiceManager::allocateMonoVoice(int midiNote, float velocity)
{
    // Always use first voice for mono
    voices[0].noteOn(midiNote, velocity);
    voices[0].resetAge();
}
```

**POLY Mode:**
- Up to MAX_VOICES simultaneous notes
- Voice stealing when all voices busy
- Each note gets independent voice

```cpp
void VoiceManager::allocatePolyVoice(int midiNote, float velocity)
{
    Voice* voice = findFreeVoice();

    if (!voice)
        voice = stealVoice();

    voice->noteOn(midiNote, velocity);
    voice->resetAge();
}
```

**UNISON Mode:**
- All 8 voices play same note
- Each voice slightly detuned for chorus effect
- Symmetric detuning distribution

```cpp
void VoiceManager::allocateUnisonVoices(int midiNote, float velocity)
{
    for (int i = 0; i < MAX_VOICES; ++i)
    {
        float detune = calculateUnisonDetune(i);
        voices[i].noteOn(midiNote, velocity, detune);
        voices[i].resetAge();
    }
}

float VoiceManager::calculateUnisonDetune(int voiceIndex) const
{
    // Symmetric spread: ±20 cents
    const float maxDetune = 20.0f;
    const float step = (2.0f * maxDetune) / (MAX_VOICES - 1);
    return -maxDetune + (step * voiceIndex);
}
```

#### 5. Audio Rendering

Sum all active voices with normalization:

```cpp
float VoiceManager::processSample()
{
    float sum = 0.0f;
    int activeVoices = 0;

    for (auto& voice : voices)
    {
        if (voice.isActive())
        {
            sum += voice.processSample();
            ++activeVoices;
            voice.incrementAge();
        }
    }

    // Normalize to prevent clipping in unison mode
    if (activeVoices > 0)
        return sum / std::sqrt(static_cast<float>(activeVoices));

    return 0.0f;
}
```

#### 6. Integrate with PluginProcessor

Replace single oscillator with VoiceManager:

```cpp
// In PluginProcessor.h
private:
    VoiceManager voiceManager;

// In processBlock()
void PluginProcessor::processBlock(juce::AudioBuffer<float>& buffer,
                                     juce::MidiBuffer& midiMessages)
{
    buffer.clear();

    for (const auto metadata : midiMessages)
    {
        auto message = metadata.getMessage();

        if (message.isNoteOn())
            voiceManager.noteOn(message.getNoteNumber(), message.getFloatVelocity());
        else if (message.isNoteOff())
            voiceManager.noteOff(message.getNoteNumber());
        else if (message.isAllNotesOff())
            voiceManager.allNotesOff();
    }

    for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
    {
        float output = voiceManager.processSample();

        for (int channel = 0; channel < buffer.getNumChannels(); ++channel)
            buffer.setSample(channel, sample, output);
    }
}
```

#### 7. Add Voice Mode Parameter
- [ ] Voice mode selector (Mono/Poly/Unison)

### Validation Checklist

**POLY Mode:**
- [ ] Can play up to 8 simultaneous notes
- [ ] 9th note steals oldest voice (verify with sustained chord)
- [ ] Each voice has independent envelope
- [ ] Note-offs release correct voice
- [ ] No stuck notes or orphaned voices
- [ ] Voice stealing is inaudible (smooth transition)

**MONO Mode:**
- [ ] Only one note sounds at a time
- [ ] New notes retrigger envelope
- [ ] Smooth transition between notes (legato)
- [ ] Last note priority works correctly

**UNISON Mode:**
- [ ] Single note triggers all 8 voices
- [ ] Chorus/detune effect is audible
- [ ] Detuning is symmetric (no pitch drift)
- [ ] Overall pitch matches input note
- [ ] No excessive volume (normalization working)
- [ ] Rich, thick timbre

**Edge Cases:**
- [ ] Rapid note repetition doesn't cause glitches
- [ ] Switching modes during playback doesn't crash
- [ ] All notes off MIDI message works
- [ ] High note density doesn't cause dropouts

### Python Reference
- `sine_generator_qt.py:100-177` - Voice class implementation
- `sine_generator_qt.py:4050-4140` - `handle_midi_note_on/off()`
- `sine_generator_qt.py:4230-4260` - `steal_voice()` LRU algorithm
- `sine_generator_qt.py:998-1020` - `reallocate_voice_pool()`

---

## Phase 4: Triple Oscillator Architecture

### Objectives
- Upgrade Voice to contain 3 independent oscillators
- Per-oscillator controls (gain, detune, octave, waveform)
- Post-mixer envelope architecture
- Oscillator enable/disable switches

### Prerequisites
- Phase 3 complete and validated
- Understanding of mixing and level management
- Reference Python implementation: `sine_generator_qt.py:3812-3814` (3-osc processing)

### Task Breakdown

#### 1. Update Voice Class for 3 Oscillators

```cpp
class Voice
{
public:
    static constexpr int NUM_OSCILLATORS = 3;

    // ... previous interface ...

    // Per-oscillator parameters
    void setOscillatorEnabled(int oscIndex, bool enabled);
    void setOscillatorWaveform(int oscIndex, Oscillator::Waveform waveform);
    void setOscillatorGain(int oscIndex, float gain);
    void setOscillatorDetune(int oscIndex, float cents);
    void setOscillatorOctave(int oscIndex, int octaveOffset);  // -3 to +3
    void setOscillatorPulseWidth(int oscIndex, float pw);

private:
    std::array<Oscillator, NUM_OSCILLATORS> oscillators;
    Envelope envelope;

    struct OscillatorSettings
    {
        bool enabled = true;
        float gain = 0.33f;
        float detuneCents = 0.0f;
        int octaveOffset = 0;
    };

    std::array<OscillatorSettings, NUM_OSCILLATORS> oscSettings;

    int currentMidiNote = -1;
    int age = 0;
    float unisonDetune = 0.0f;

    void updateOscillatorFrequencies();
    float mixOscillators();
};
```

#### 2. Implement Oscillator Frequency Calculation

Apply MIDI note, octave offset, detune, and unison detune:

```cpp
void Voice::noteOn(int midiNote, float velocity, float unisonDetune)
{
    this->currentMidiNote = midiNote;
    this->unisonDetune = unisonDetune;

    updateOscillatorFrequencies();
    envelope.noteOn(velocity);
}

void Voice::updateOscillatorFrequencies()
{
    if (currentMidiNote < 0)
        return;

    float baseFreq = AudioUtils::midiNoteToFrequency(currentMidiNote);

    for (int i = 0; i < NUM_OSCILLATORS; ++i)
    {
        // Apply octave offset
        float octaveMultiplier = std::pow(2.0f, oscSettings[i].octaveOffset);

        // Apply detune (cents) + unison detune
        float totalDetuneCents = oscSettings[i].detuneCents + unisonDetune;
        float detuneMultiplier = std::pow(2.0f, totalDetuneCents / 1200.0f);

        float finalFreq = baseFreq * octaveMultiplier * detuneMultiplier;
        oscillators[i].setFrequency(finalFreq);
    }
}
```

#### 3. Implement Oscillator Mixing

Post-mixer envelope architecture (mix first, then apply envelope):

```cpp
float Voice::processSample()
{
    if (!isActive())
        return 0.0f;

    // Mix oscillators with individual gains
    float mix = mixOscillators();

    // Apply single envelope to mixed signal
    float envLevel = envelope.processSample();

    return mix * envLevel;
}

float Voice::mixOscillators()
{
    float sum = 0.0f;

    for (int i = 0; i < NUM_OSCILLATORS; ++i)
    {
        if (oscSettings[i].enabled)
        {
            float oscSample = oscillators[i].processSample();
            sum += oscSample * oscSettings[i].gain;
        }
    }

    return sum;
}
```

#### 4. Update VoiceManager Parameter Broadcasting

Add methods to set per-oscillator parameters across all voices:

```cpp
void VoiceManager::setOscillatorEnabled(int oscIndex, bool enabled)
{
    for (auto& voice : voices)
        voice.setOscillatorEnabled(oscIndex, enabled);
}

void VoiceManager::setOscillatorWaveform(int oscIndex, Oscillator::Waveform waveform)
{
    for (auto& voice : voices)
        voice.setOscillatorWaveform(oscIndex, waveform);
}

// ... similar for other per-oscillator parameters ...
```

#### 5. Add Triple Oscillator Parameters

For each of 3 oscillators:
- [ ] Enabled (on/off)
- [ ] Waveform (Sine/Sawtooth/Square)
- [ ] Detune (±100 cents, default 0)
- [ ] Octave (-3 to +3, default 0)
- [ ] Pulse Width (1% - 99%, default 50%)
- [ ] Gain (0 - 100%, default 33%)

Total: 18 new parameters

### Validation Checklist

**Oscillator Mixing:**
- [ ] All 3 oscillators audible when enabled and gain > 0
- [ ] Individual oscillator gains work correctly
- [ ] Disabling oscillator removes it from mix
- [ ] No clicks when enabling/disabling oscillators
- [ ] Mix levels sum correctly (no unexpected volume changes)

**Detuning:**
- [ ] Detune creates beating/chorus effect between oscillators
- [ ] Positive detune raises pitch, negative lowers
- [ ] Detune range ±100 cents works correctly
- [ ] Unison mode + oscillator detune creates super-thick sound

**Octave Offsets:**
- [ ] -3 octaves = 1/8 frequency
- [ ] +3 octaves = 8x frequency
- [ ] Octave changes are immediate and glitch-free
- [ ] Can create complex timbres (e.g., osc1 +0, osc2 +1, osc3 +2)

**Pulse Width Modulation:**
- [ ] PWM changes square wave timbre
- [ ] 50% duty cycle = classic square wave
- [ ] Extreme values (10%, 90%) create thin, nasal tones
- [ ] PWM works independently per oscillator

**Complex Configurations:**
- [ ] 3 different waveforms simultaneously (e.g., sine + saw + square)
- [ ] 3 different octaves simultaneously
- [ ] All oscillators detuned differently
- [ ] Unison mode with complex oscillator setup

### Python Reference
- `sine_generator_qt.py:3812-3814` - Processing 3 oscillators
- `sine_generator_qt.py:2467-2470` - `apply_octave()`
- `sine_generator_qt.py:2463-2465` - `apply_detune()`

---

## Phase 5: Moog Ladder Filter

### Objectives
- Implement 4-pole Moog ladder filter
- Support LP (lowpass), BP (bandpass), HP (highpass) modes
- Resonance control with self-oscillation prevention
- Smooth cutoff frequency modulation

### Prerequisites
- Phase 4 complete and validated
- Understanding of filter topologies and resonance
- Reference Python implementation: `sine_generator_qt.py:370-440`

### Task Breakdown

#### 1. Implement MoogFilter Class
**File:** `Source/DSP/MoogFilter.h/cpp`

```cpp
class MoogFilter
{
public:
    enum class Mode
    {
        LowPass,
        BandPass,
        HighPass
    };

    MoogFilter();

    void setSampleRate(double sampleRate);
    void setMode(Mode mode);
    void setCutoff(float cutoffHz);
    void setResonance(float resonance);  // 0.0 - 1.0

    float processSample(float input);
    void reset();

private:
    Mode mode = Mode::LowPass;
    double sampleRate = 44100.0;

    // 4 cascaded one-pole filters
    float stage1 = 0.0f;
    float stage2 = 0.0f;
    float stage3 = 0.0f;
    float stage4 = 0.0f;

    // Parameters
    float cutoffFreq = 1000.0f;
    float resonance = 0.0f;

    // Cached coefficients
    float g = 0.0f;              // Cutoff coefficient
    float feedbackGain = 0.0f;   // Resonance feedback amount

    bool coefficientsNeedUpdate = true;

    void updateCoefficients();
};
```

#### 2. Implement Filter Algorithm

Based on simplified Moog topology:

```cpp
float MoogFilter::processSample(float input)
{
    if (coefficientsNeedUpdate)
        updateCoefficients();

    // Feedback from appropriate stage based on mode
    float feedbackSource = (mode == Mode::LowPass) ? stage4 : stage2;

    // Apply resonance feedback
    float inputWithFeedback = input - feedbackSource * feedbackGain;

    // Soft clipping for non-linearity (tanh approximation or actual tanh)
    inputWithFeedback = std::tanh(inputWithFeedback);

    // 4 cascaded one-pole lowpass filters
    stage1 += g * (inputWithFeedback - stage1);
    stage2 += g * (stage1 - stage2);
    stage3 += g * (stage2 - stage3);
    stage4 += g * (stage3 - stage4);

    // Output selection based on mode
    switch (mode)
    {
        case Mode::LowPass:
            return stage4;  // 24dB/octave slope

        case Mode::BandPass:
            return stage2;  // 12dB/octave slope

        case Mode::HighPass:
            return input - stage4;  // Subtraction for highpass
    }
}

void MoogFilter::updateCoefficients()
{
    // Normalized cutoff (prevent > Nyquist)
    float normalizedCutoff = std::clamp(cutoffFreq / sampleRate,
                                        20.0f / sampleRate,
                                        0.45f);

    // One-pole coefficient
    g = std::tan(juce::MathConstants<float>::pi * normalizedCutoff);
    g = g / (1.0f + g);  // Normalized

    // Resonance to feedback gain (0.0 - 4.0 range for self-oscillation)
    feedbackGain = resonance * 3.5f;

    coefficientsNeedUpdate = false;
}
```

#### 3. Integrate Filter into Voice

Add filter after oscillator mixing, before envelope:

```cpp
class Voice
{
private:
    std::array<Oscillator, NUM_OSCILLATORS> oscillators;
    MoogFilter filter;
    Envelope envelope;

public:
    void setFilterMode(MoogFilter::Mode mode);
    void setFilterCutoff(float cutoffHz);
    void setFilterResonance(float resonance);

    float processSample()
    {
        if (!isActive())
            return 0.0f;

        // Mix oscillators
        float mix = mixOscillators();

        // Apply filter
        float filtered = filter.processSample(mix);

        // Apply envelope
        float envLevel = envelope.processSample();

        return filtered * envLevel;
    }
};
```

#### 4. Add Filter Parameters
- [ ] Filter mode (LP/BP/HP)
- [ ] Cutoff frequency (20 Hz - 20 kHz, logarithmic scale)
- [ ] Resonance (0 - 100%)

### Validation Checklist

**Lowpass Mode:**
- [ ] Removes high frequencies smoothly
- [ ] Cutoff sweeps produce classic filter sweep sound
- [ ] 24dB/octave rolloff (steep)
- [ ] Low cutoff creates dark, muffled tone

**Bandpass Mode:**
- [ ] Isolates mid-range frequencies
- [ ] Sweeping cutoff creates "wah" effect
- [ ] Thinner sound than lowpass

**Highpass Mode:**
- [ ] Removes bass frequencies
- [ ] High cutoff creates thin, tinny tone
- [ ] Useful for removing sub-bass

**Resonance:**
- [ ] Low resonance: Subtle peak at cutoff
- [ ] Medium resonance: Pronounced peak, vocal quality
- [ ] High resonance: Nearly self-oscillating, metallic
- [ ] No instability or runaway oscillation
- [ ] Resonance works in all filter modes

**Stability:**
- [ ] Filter never explodes or produces inf/NaN
- [ ] Rapid cutoff changes don't cause clicks
- [ ] Filter mode switching is glitch-free
- [ ] Works across full cutoff range (20 Hz - 20 kHz)

### Python Reference
- `sine_generator_qt.py:370-440` - MoogLadderFilter class
- Coefficient calculation and state update logic
- Resonance feedback implementation

### Performance Optimization
- Pre-calculate coefficients only when parameters change
- Use fast tanh approximation if performance critical
- Ensure filter state doesn't denormalize (add DC offset removal if needed)

---

## Phase 6: Dual LFO System

### Objectives
- Implement 2 independent LFO generators
- Support 5 waveforms (Sine, Triangle, Square, Sawtooth, Random)
- Free-running and tempo-synced modes
- 7 modulation destinations with depth and mix controls
- Proper modulation combining when both LFOs target same destination

### Prerequisites
- Phase 5 complete and validated
- Understanding of modulation routing and combining
- Reference Python implementation: `sine_generator_qt.py:580-630` (LFO), `3687-3740` (modulation)

### Task Breakdown

#### 1. Implement LFO Class
**File:** `Source/DSP/LFO.h/cpp`

```cpp
class LFO
{
public:
    enum class Waveform
    {
        Sine,
        Triangle,
        Square,
        Sawtooth,
        Random
    };

    enum class RateMode
    {
        Free,   // Hz-based
        Sync    // Tempo-synced
    };

    LFO();

    void setSampleRate(double sampleRate);
    void setWaveform(Waveform waveform);
    void setRateMode(RateMode mode);
    void setRateHz(float hz);             // 0.1 - 20 Hz for Free mode
    void setSyncDivision(float division); // 0.0625 (1/16) to 4.0 (4/1)
    void setBPM(float bpm);

    float processSample();  // Returns -1.0 to +1.0
    void reset();

private:
    Waveform waveform = Waveform::Sine;
    RateMode rateMode = RateMode::Free;

    double phase = 0.0;
    double phaseIncrement = 0.0;
    double sampleRate = 44100.0;

    float rateHz = 2.0f;
    float syncDivision = 0.25f;  // 1/4 note
    float bpm = 120.0f;

    // For random waveform
    float currentRandomValue = 0.0f;
    float nextRandomValue = 0.0f;

    void updatePhaseIncrement();
    float generateWaveform();
    float generateRandom();
};
```

#### 2. Implement LFO Waveforms

```cpp
float LFO::generateWaveform()
{
    switch (waveform)
    {
        case Waveform::Sine:
            return std::sin(phase * 2.0f * juce::MathConstants<float>::pi);

        case Waveform::Triangle:
        {
            float tri = 2.0f * std::abs(2.0f * phase - 1.0f) - 1.0f;
            return tri;
        }

        case Waveform::Square:
            return (phase < 0.5f) ? 1.0f : -1.0f;

        case Waveform::Sawtooth:
            return 2.0f * phase - 1.0f;

        case Waveform::Random:
            return generateRandom();
    }
}

float LFO::generateRandom()
{
    // Sample & hold random at each cycle
    if (phase < phaseIncrement)  // Cycle just restarted
    {
        currentRandomValue = nextRandomValue;
        nextRandomValue = juce::Random::getSystemRandom().nextFloat() * 2.0f - 1.0f;
    }

    return currentRandomValue;
}

float LFO::processSample()
{
    float value = generateWaveform();

    phase += phaseIncrement;
    AudioUtils::wrapPhase(phase);

    return value;
}
```

#### 3. Implement Tempo Sync

```cpp
void LFO::updatePhaseIncrement()
{
    if (rateMode == RateMode::Free)
    {
        phaseIncrement = rateHz / sampleRate;
    }
    else  // Sync mode
    {
        // Convert BPM and division to Hz
        // Division: 0.0625 = 1/16, 0.25 = 1/4, 1.0 = 1/1 (whole note), etc.
        float beatsPerSecond = bpm / 60.0f;
        float cyclesPerBeat = syncDivision;
        float hz = beatsPerSecond * cyclesPerBeat;

        phaseIncrement = hz / sampleRate;
    }
}
```

#### 4. Create Modulation System

**File:** `Source/DSP/ModulationRouter.h/cpp`

```cpp
class ModulationRouter
{
public:
    enum class Destination
    {
        None,
        AllOscsPitch,      // Vibrato
        FilterCutoff,      // Filter sweep
        AllOscsVolume,     // Tremolo
        Osc1PulseWidth,    // PWM
        Osc2PulseWidth,
        Osc3PulseWidth
    };

    struct ModulationAmount
    {
        Destination destination = Destination::None;
        float depth = 0.0f;  // 0.0 - 1.0
        float mix = 1.0f;    // 0.0 - 1.0 (dry/wet)
    };

    // Calculate modulation value for destination
    static float calculatePitchModulation(float lfoSignal, float depth, float mix);
    static float calculateFilterModulation(float lfoSignal, float baseCutoff, float depth, float mix);
    static float calculateVolumeModulation(float lfoSignal, float depth, float mix);
    static float calculatePWModulation(float lfoSignal, float basePW, float depth, float mix);
};
```

#### 5. Implement Modulation Calculations

```cpp
float ModulationRouter::calculatePitchModulation(float lfoSignal, float depth, float mix)
{
    // ±5% pitch modulation at max depth
    float maxPitchMod = 0.05f;
    float modAmount = lfoSignal * maxPitchMod * depth;

    // Convert to frequency multiplier
    float pitchMultiplier = 1.0f + modAmount;

    // Apply mix (blend with unmodulated)
    return 1.0f + (pitchMultiplier - 1.0f) * mix;
}

float ModulationRouter::calculateFilterModulation(float lfoSignal, float baseCutoff,
                                                   float depth, float mix)
{
    // ±2 octaves modulation at max depth
    float octaveMod = lfoSignal * 2.0f * depth;
    float cutoffMultiplier = std::pow(2.0f, octaveMod);
    float modulatedCutoff = baseCutoff * cutoffMultiplier;

    // Apply mix
    return baseCutoff + (modulatedCutoff - baseCutoff) * mix;
}

float ModulationRouter::calculateVolumeModulation(float lfoSignal, float depth, float mix)
{
    // 0.0 - 1.0 tremolo
    float modAmount = (lfoSignal + 1.0f) / 2.0f;  // Convert -1/+1 to 0/1
    float volumeMultiplier = 1.0f - (depth * (1.0f - modAmount));

    // Apply mix
    return 1.0f + (volumeMultiplier - 1.0f) * mix;
}

float ModulationRouter::calculatePWModulation(float lfoSignal, float basePW,
                                               float depth, float mix)
{
    // ±0.3 pulse width modulation
    float pwMod = lfoSignal * 0.3f * depth;
    float modulatedPW = std::clamp(basePW + pwMod, 0.01f, 0.99f);

    // Apply mix
    return basePW + (modulatedPW - basePW) * mix;
}
```

#### 6. Integrate LFOs into Voice Manager

```cpp
class VoiceManager
{
private:
    std::array<LFO, 2> lfos;
    std::array<ModulationRouter::ModulationAmount, 2> lfoModulation;

public:
    void setLFOWaveform(int lfoIndex, LFO::Waveform waveform);
    void setLFORateMode(int lfoIndex, LFO::RateMode mode);
    void setLFORateHz(int lfoIndex, float hz);
    void setLFODestination(int lfoIndex, ModulationRouter::Destination dest);
    void setLFODepth(int lfoIndex, float depth);
    void setLFOMix(int lfoIndex, float mix);

    float processSample()
    {
        // Generate LFO signals
        float lfo1Signal = lfos[0].processSample();
        float lfo2Signal = lfos[1].processSample();

        // Apply modulations to voices
        applyModulations(lfo1Signal, lfo2Signal);

        // Sum voice outputs
        return sumVoices();
    }

private:
    void applyModulations(float lfo1Signal, float lfo2Signal);
    float sumVoices();
};
```

#### 7. Handle Modulation Combining

When both LFOs target the same destination:

```cpp
void VoiceManager::applyModulations(float lfo1Signal, float lfo2Signal)
{
    // Calculate modulations from each LFO
    auto& mod1 = lfoModulation[0];
    auto& mod2 = lfoModulation[1];

    // Check for pitch modulation
    float pitchMod1 = 1.0f;
    float pitchMod2 = 1.0f;

    if (mod1.destination == ModulationRouter::Destination::AllOscsPitch)
        pitchMod1 = ModulationRouter::calculatePitchModulation(lfo1Signal, mod1.depth, mod1.mix);

    if (mod2.destination == ModulationRouter::Destination::AllOscsPitch)
        pitchMod2 = ModulationRouter::calculatePitchModulation(lfo2Signal, mod2.depth, mod2.mix);

    // Combine multiplicatively for pitch
    float combinedPitchMod = pitchMod1 * pitchMod2;

    // Apply to all voices
    for (auto& voice : voices)
        voice.applyPitchModulation(combinedPitchMod);

    // Similar for other destinations...
}
```

#### 8. Add LFO Parameters

For each of 2 LFOs:
- [ ] Waveform (Sine/Triangle/Square/Sawtooth/Random)
- [ ] Rate mode (Free/Sync)
- [ ] Rate Hz (0.1 - 20 Hz, logarithmic)
- [ ] Sync division (1/16 to 4/1)
- [ ] Destination (7 choices)
- [ ] Depth (0 - 100%)
- [ ] Mix (0 - 100%)

Total: 14 new parameters

### Validation Checklist

**LFO Waveforms:**
- [ ] Sine: Smooth, musical vibrato/tremolo
- [ ] Triangle: Linear ramps, sharper than sine
- [ ] Square: Stepped switching between values
- [ ] Sawtooth: Rising/falling ramps
- [ ] Random: Sample & hold, stepped random values

**Rate Modes:**
- [ ] Free mode: Hz control works (0.1 - 20 Hz range)
- [ ] Sync mode: Locks to DAW tempo
- [ ] Tempo changes affect synced LFO rate
- [ ] Division control works (1/16, 1/8, 1/4, 1/2, 1/1, 2/1, 4/1)

**Modulation Destinations:**
- [ ] **All Oscs Pitch:** Creates vibrato effect
- [ ] **Filter Cutoff:** Auto-wah, sweeping filter
- [ ] **All Oscs Volume:** Tremolo effect
- [ ] **Osc 1/2/3 Pulse Width:** PWM timbre changes
- [ ] Depth control adjusts modulation intensity
- [ ] Mix control blends dry/wet

**Dual LFO Combining:**
- [ ] Both LFOs can target same destination
- [ ] Modulations combine correctly (multiplicative for pitch, additive for others)
- [ ] No excessive values or clipping
- [ ] Complex modulation patterns possible

**Performance:**
- [ ] LFOs don't cause CPU spikes
- [ ] Waveform changes are glitch-free
- [ ] Destination changes are glitch-free

### Python Reference
- `sine_generator_qt.py:580-630` - LFOGenerator class
- `sine_generator_qt.py:3687-3740` - `apply_lfo_modulation()`
- `sine_generator_qt.py:3825-3841` - Dual LFO signal generation

---

## Phase 7: Noise Generator

### Objectives
- Add white/pink/brown noise source
- Independent noise envelope
- Mix noise with oscillator output

### Prerequisites
- Phase 6 complete and validated
- Understanding of noise algorithms
- Reference Python implementation: `sine_generator_qt.py:490-577`

### Task Breakdown

#### 1. Implement NoiseGenerator Class
**File:** `Source/DSP/NoiseGenerator.h/cpp`

```cpp
class NoiseGenerator
{
public:
    enum class NoiseType
    {
        White,
        Pink,
        Brown
    };

    NoiseGenerator();

    void setSampleRate(double sampleRate);
    void setType(NoiseType type);
    void setEnabled(bool enabled);
    void setGain(float gain);
    void setEnvelopeParameters(float attack, float decay, float sustain, float release);

    void trigger(float velocity = 1.0f);
    void release();

    float processSample();

private:
    NoiseType type = NoiseType::White;
    bool enabled = false;
    float gain = 0.5f;

    Envelope envelope;

    // Pink noise state (Paul Kellet algorithm)
    std::array<float, 5> pinkState = {0.0f};

    // Brown noise state
    float brownState = 0.0f;

    juce::Random random;

    float generateWhite();
    float generatePink();
    float generateBrown();
};
```

#### 2. Implement Noise Algorithms

```cpp
float NoiseGenerator::generateWhite()
{
    // Uniform random -1.0 to +1.0
    return random.nextFloat() * 2.0f - 1.0f;
}

float NoiseGenerator::generatePink()
{
    // Paul Kellet's refined method
    float white = generateWhite();

    pinkState[0] = 0.99886f * pinkState[0] + white * 0.0555179f;
    pinkState[1] = 0.99332f * pinkState[1] + white * 0.0750759f;
    pinkState[2] = 0.96900f * pinkState[2] + white * 0.1538520f;
    pinkState[3] = 0.86650f * pinkState[3] + white * 0.3104856f;
    pinkState[4] = 0.55000f * pinkState[4] + white * 0.5329522f;

    float pink = pinkState[0] + pinkState[1] + pinkState[2] +
                 pinkState[3] + pinkState[4];

    return pink * 0.11f;  // Normalize
}

float NoiseGenerator::generateBrown()
{
    // Brown noise via integration (random walk)
    float white = generateWhite();
    brownState = (brownState + white * 0.02f) * 0.99f;

    return brownState;
}

float NoiseGenerator::processSample()
{
    if (!enabled)
        return 0.0f;

    float noise = 0.0f;

    switch (type)
    {
        case NoiseType::White: noise = generateWhite(); break;
        case NoiseType::Pink:  noise = generatePink();  break;
        case NoiseType::Brown: noise = generateBrown(); break;
    }

    float envLevel = envelope.processSample();

    return noise * gain * envLevel;
}
```

#### 3. Integrate into Voice

Add noise generator to each voice:

```cpp
class Voice
{
private:
    std::array<Oscillator, NUM_OSCILLATORS> oscillators;
    MoogFilter filter;
    NoiseGenerator noiseGenerator;
    Envelope envelope;

public:
    void setNoiseEnabled(bool enabled);
    void setNoiseType(NoiseGenerator::NoiseType type);
    void setNoiseGain(float gain);

    void noteOn(int midiNote, float velocity, float unisonDetune = 0.0f)
    {
        // ... existing code ...

        // Trigger noise envelope
        noiseGenerator.trigger(velocity);
    }

    void noteOff()
    {
        envelope.noteOff();
        noiseGenerator.release();
    }

    float processSample()
    {
        if (!isActive())
            return 0.0f;

        // Mix oscillators
        float oscMix = mixOscillators();

        // Add noise
        float noiseSample = noiseGenerator.processSample();
        float mix = oscMix + noiseSample;

        // Apply filter
        float filtered = filter.processSample(mix);

        // Apply envelope
        float envLevel = envelope.processSample();

        return filtered * envLevel;
    }
};
```

#### 4. Add Noise Parameters
- [ ] Noise enabled (on/off)
- [ ] Noise type (White/Pink/Brown)
- [ ] Noise gain (0 - 100%)

### Validation Checklist

**White Noise:**
- [ ] Bright, hissy sound
- [ ] Equal energy across all frequencies
- [ ] Useful for hi-hats, cymbals, breath sounds

**Pink Noise:**
- [ ] Balanced, natural sound
- [ ] Equal energy per octave
- [ ] Less harsh than white noise

**Brown Noise:**
- [ ] Dark, rumbly sound
- [ ] Energy concentrated in low frequencies
- [ ] Useful for wind, ocean sounds

**Integration:**
- [ ] Noise has independent envelope
- [ ] Noise gain controls level in mix
- [ ] Noise + oscillators create complex timbres
- [ ] Enabling/disabling noise is glitch-free
- [ ] Noise is filtered along with oscillators

### Python Reference
- `sine_generator_qt.py:490-577` - NoiseGenerator class
- Paul Kellet pink noise algorithm

---

## Phase 8: Professional GUI

### Objectives
- Create professional, intuitive user interface
- All parameters controllable from UI
- Visual feedback (LED indicators, waveform displays)
- Resizable window (optional)
- Match Python version layout philosophy

### Prerequisites
- Phases 1-7 complete and validated
- Familiarity with JUCE GUI components
- Reference Python GUI for layout ideas

### Task Breakdown

#### 1. Design UI Layout

Create mockup or wireframe:
- Top bar: Voice mode, MIDI info
- Oscillator section: 3 columns for 3 oscillators
- Mixer section: Per-oscillator gains + master volume
- Filter section: Mode buttons, cutoff, resonance
- Envelope section: ADSR sliders with visual display
- LFO section: 2 LFO panels side-by-side
- Noise section: Type, gain controls
- Preset section: Browser, save/load buttons

#### 2. Implement Main PluginEditor
**File:** `Source/PluginEditor.h/cpp`

```cpp
class PluginEditor : public juce::AudioProcessorEditor
{
public:
    PluginEditor(PluginProcessor& processor);
    ~PluginEditor() override;

    void paint(juce::Graphics& g) override;
    void resized() override;

private:
    PluginProcessor& processorRef;

    // UI Components
    TopBarComponent topBar;
    OscillatorSection oscillatorSection;
    MixerSection mixerSection;
    FilterSection filterSection;
    EnvelopeSection envelopeSection;
    LFOSection lfoSection;
    NoiseSection noiseSection;
    PresetSection presetSection;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PluginEditor)
};
```

#### 3. Create Custom Components

**OscillatorPanel.h/cpp:**
```cpp
class OscillatorPanel : public juce::Component
{
public:
    OscillatorPanel(int oscIndex, juce::AudioProcessorValueTreeState& state);

    void paint(juce::Graphics& g) override;
    void resized() override;

private:
    int oscIndex;

    juce::ToggleButton enableButton;
    juce::ComboBox waveformSelector;
    juce::Slider detuneSlider;
    juce::Slider octaveSlider;
    juce::Slider pulseWidthSlider;
    juce::Slider gainSlider;

    // Parameter attachments
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> enableAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> waveformAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> detuneAttachment;
    // ... etc
};
```

**FilterPanel.h/cpp:**
- Mode buttons (LP/BP/HP)
- Cutoff knob with frequency display
- Resonance knob

**EnvelopePanel.h/cpp:**
- ADSR sliders
- Visual envelope curve display
- Numerical value labels

**LFOPanel.h/cpp:**
- Waveform selector
- Rate mode selector
- Rate knob (changes label based on mode)
- Destination selector
- Depth and mix knobs
- LED indicator pulsing with LFO

#### 4. Implement Parameter Attachments

Use JUCE's built-in parameter attachment system:

```cpp
class FilterSection : public juce::Component
{
private:
    juce::Slider cutoffSlider;
    juce::Slider resonanceSlider;
    juce::ComboBox modeSelector;

    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> cutoffAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> resonanceAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> modeAttachment;

public:
    FilterSection(juce::AudioProcessorValueTreeState& state)
    {
        // Configure slider
        cutoffSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
        cutoffSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 80, 20);
        addAndMakeVisible(cutoffSlider);

        // Attach to parameter
        cutoffAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
            state, "filterCutoff", cutoffSlider);

        // ... similar for other controls
    }
};
```

#### 5. Create Custom LookAndFeel
**File:** `Source/UI/LookAndFeel/CustomLookAndFeel.h/cpp`

Customize appearance:
- Rotary knob design
- Slider track colors
- Button styles
- Font choices
- Color scheme

```cpp
class CustomLookAndFeel : public juce::LookAndFeel_V4
{
public:
    CustomLookAndFeel()
    {
        setColour(juce::Slider::thumbColourId, juce::Colours::orange);
        setColour(juce::Slider::rotarySliderFillColourId, juce::Colours::lightblue);
        // ... more customization
    }

    void drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height,
                          float sliderPos, float rotaryStartAngle, float rotaryEndAngle,
                          juce::Slider& slider) override
    {
        // Custom rotary knob rendering
    }
};
```

#### 6. Add Visual Feedback

**LED Indicator Component:**
```cpp
class LEDIndicator : public juce::Component, public juce::Timer
{
public:
    LEDIndicator()
    {
        startTimer(50);  // Update at 20 Hz
    }

    void timerCallback() override
    {
        // Read LFO value from processor
        brightness = getLFOValue();  // 0.0 - 1.0
        repaint();
    }

    void paint(juce::Graphics& g) override
    {
        auto colour = juce::Colours::green.withBrightness(brightness);
        g.setColour(colour);
        g.fillEllipse(getLocalBounds().toFloat());
    }

private:
    float brightness = 0.0f;
};
```

**Envelope Visualizer:**
- Draw ADSR curve based on current parameters
- Highlight current phase during playback

#### 7. Layout Management

Implement resized() for responsive layout:

```cpp
void PluginEditor::resized()
{
    auto area = getLocalBounds();

    topBar.setBounds(area.removeFromTop(40));

    auto oscillatorArea = area.removeFromTop(200);
    oscillatorSection.setBounds(oscillatorArea);

    auto controlsArea = area.removeFromTop(250);
    filterSection.setBounds(controlsArea.removeFromLeft(200));
    envelopeSection.setBounds(controlsArea.removeFromLeft(300));
    mixerSection.setBounds(controlsArea);

    auto modulationArea = area.removeFromTop(150);
    lfoSection.setBounds(modulationArea.removeFromLeft(400));
    noiseSection.setBounds(modulationArea);

    presetSection.setBounds(area);
}
```

#### 8. Implement Resizable Window (Optional)

```cpp
PluginEditor::PluginEditor(PluginProcessor& p)
    : AudioProcessorEditor(&p), processorRef(p)
{
    setSize(800, 600);
    setResizable(true, true);
    setResizeLimits(600, 450, 1200, 900);
    getConstrainer()->setFixedAspectRatio(800.0 / 600.0);
}
```

### Validation Checklist

**Functionality:**
- [ ] All parameters controllable from UI
- [ ] Parameter changes update audio engine
- [ ] Automation from DAW updates UI
- [ ] UI doesn't block audio thread
- [ ] Smooth parameter updates (no zipper noise)

**Visual Design:**
- [ ] Professional, clean appearance
- [ ] Consistent spacing and alignment
- [ ] Readable labels and value displays
- [ ] Color scheme is pleasant
- [ ] Controls are appropriately sized

**Usability:**
- [ ] Logical layout (grouped by function)
- [ ] Important controls are prominent
- [ ] Tooltips for complex parameters (optional)
- [ ] Visual feedback for LFOs
- [ ] Envelope visualizer updates correctly

**Performance:**
- [ ] UI is responsive (no lag)
- [ ] Resizing is smooth
- [ ] No flickering or tearing
- [ ] Low CPU usage for rendering

**Cross-Platform:**
- [ ] Looks correct on macOS
- [ ] Looks correct on Windows
- [ ] Looks correct on Linux (if supported)
- [ ] Works with different display scaling

---

## Phase 9: Preset System

### Objectives
- Save and load presets
- Preset browser UI
- Factory preset library
- DAW automation state persistence

### Prerequisites
- Phase 8 complete and validated
- Understanding of JUCE ValueTree state management

### Task Breakdown

#### 1. Implement ValueTreeState Parameter Layout
**File:** `Source/PluginProcessor.cpp`

```cpp
juce::AudioProcessorValueTreeState::ParameterLayout
PluginProcessor::createParameterLayout()
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;

    // Voice mode
    params.push_back(std::make_unique<juce::AudioParameterChoice>(
        "voiceMode", "Voice Mode",
        juce::StringArray{"Mono", "Poly", "Unison"}, 1));

    // Oscillators (3x)
    for (int i = 0; i < 3; ++i)
    {
        auto prefix = "osc" + juce::String(i + 1);

        params.push_back(std::make_unique<juce::AudioParameterBool>(
            prefix + "Enabled", "Osc " + juce::String(i + 1) + " Enabled", true));

        params.push_back(std::make_unique<juce::AudioParameterChoice>(
            prefix + "Waveform", "Osc " + juce::String(i + 1) + " Waveform",
            juce::StringArray{"Sine", "Sawtooth", "Square"}, 1));

        params.push_back(std::make_unique<juce::AudioParameterFloat>(
            prefix + "Detune", "Osc " + juce::String(i + 1) + " Detune",
            juce::NormalisableRange<float>(-100.0f, 100.0f, 0.1f), 0.0f));

        // ... all other osc parameters
    }

    // Envelope
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        "envAttack", "Attack",
        juce::NormalisableRange<float>(0.001f, 2.0f, 0.001f, 0.3f), 0.01f));

    // ... all other parameters

    return { params.begin(), params.end() };
}
```

#### 2. Implement Preset Save/Load

```cpp
void PluginProcessor::savePreset(const juce::File& file)
{
    auto state = parameters.copyState();
    auto xml = state.createXml();

    if (xml != nullptr)
        xml->writeTo(file);
}

void PluginProcessor::loadPreset(const juce::File& file)
{
    auto xml = juce::parseXML(file);

    if (xml != nullptr)
    {
        auto state = juce::ValueTree::fromXml(*xml);

        if (state.isValid())
            parameters.replaceState(state);
    }
}
```

#### 3. Create Preset Browser UI
**File:** `Source/UI/Components/PresetBrowser.h/cpp`

```cpp
class PresetBrowser : public juce::Component,
                      public juce::FileBrowserListener
{
public:
    PresetBrowser(PluginProcessor& processor);

    void paint(juce::Graphics& g) override;
    void resized() override;

    void selectionChanged() override;
    void fileClicked(const juce::File& file, const juce::MouseEvent&) override;
    void fileDoubleClicked(const juce::File& file) override;

private:
    PluginProcessor& processorRef;

    juce::File presetDirectory;
    juce::WildcardFileFilter fileFilter;
    juce::DirectoryContentsList directoryList;
    juce::FileListComponent fileList;

    juce::TextButton loadButton;
    juce::TextButton saveButton;
    juce::TextButton deleteButton;

    void loadPreset(const juce::File& file);
    void savePresetDialog();
    void deletePreset();

    juce::File getPresetDirectory();
};
```

#### 4. Create Factory Presets

Port presets from Python repository:

**Resources/Presets/Factory/**
- Init.vstpreset (default initialized state)
- Bass/
  - SubBass.vstpreset
  - DeepBass.vstpreset
- Lead/
  - BrightLead.vstpreset
  - SoloLead.vstpreset
- Pad/
  - WarmPad.vstpreset
  - EtherealPad.vstpreset
- FX/
  - SweepFX.vstpreset

Create XML files manually or export from plugin:

```xml
<?xml version="1.0" encoding="UTF-8"?>
<Preset>
  <voiceMode>1</voiceMode>  <!-- Poly -->
  <osc1Enabled>1</osc1Enabled>
  <osc1Waveform>1</osc1Waveform>  <!-- Sawtooth -->
  <osc1Detune>0.0</osc1Detune>
  <osc1Octave>0</osc1Octave>
  <osc1PulseWidth>0.5</osc1PulseWidth>
  <osc1Gain>0.7</osc1Gain>
  <!-- ... all other parameters ... -->
</Preset>
```

#### 5. Implement State Persistence

Override DAW state methods:

```cpp
void PluginProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    auto state = parameters.copyState();
    auto xml = state.createXml();
    copyXmlToBinary(*xml, destData);
}

void PluginProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    auto xml = getXmlFromBinary(data, sizeInBytes);

    if (xml != nullptr)
    {
        auto state = juce::ValueTree::fromXml(*xml);

        if (state.isValid())
            parameters.replaceState(state);
    }
}
```

#### 6. Add Preset Management Features

**Preset Metadata:**
- Preset name
- Author
- Category
- Description
- Tags

**Enhanced Save Dialog:**
```cpp
void PresetBrowser::savePresetDialog()
{
    juce::AlertWindow window("Save Preset", "Enter preset name:",
                             juce::AlertWindow::NoIcon);

    window.addTextEditor("presetName", "New Preset");
    window.addTextEditor("author", juce::SystemStats::getFullUserName());
    window.addComboBox("category", {"Bass", "Lead", "Pad", "FX", "Other"});

    window.addButton("Save", 1, juce::KeyPress(juce::KeyPress::returnKey));
    window.addButton("Cancel", 0, juce::KeyPress(juce::KeyPress::escapeKey));

    if (window.runModalLoop() == 1)
    {
        auto name = window.getTextEditorContents("presetName");
        // Save preset with metadata...
    }
}
```

### Validation Checklist

**Preset Functionality:**
- [ ] Presets save all parameter values
- [ ] Presets load correctly
- [ ] Factory presets included with plugin
- [ ] Preset browser lists available presets
- [ ] Preset categories/organization works

**DAW Integration:**
- [ ] Plugin state saved with DAW project
- [ ] Plugin state loaded with DAW project
- [ ] Automation data persists
- [ ] Preset changes update DAW's "modified" state

**User Experience:**
- [ ] Save dialog is intuitive
- [ ] Preset names are displayed clearly
- [ ] Easy to navigate preset library
- [ ] Can preview presets before loading (optional)
- [ ] Can delete unwanted presets

**Robustness:**
- [ ] Handles corrupt preset files gracefully
- [ ] Handles missing preset directory
- [ ] Validates preset data before loading
- [ ] Doesn't crash on malformed XML

---

## Cross-Cutting Concerns

### Performance Optimization

#### Memory Management
- Pre-allocate all audio buffers
- Avoid `new`/`delete` in audio thread
- Use `std::array` instead of `std::vector` where possible
- Pool allocations for voices

#### CPU Optimization
- Profile with Instruments (macOS) or VTune (Windows)
- Optimize hot loops in processSample() methods
- Consider SIMD for parallel operations (optional)
- Use fast approximations (e.g., fast sin, tanh)
- Cache computed values (coefficients, rates)

#### Real-Time Safety
- No locks in audio thread
- No memory allocation in audio thread
- No file I/O in audio thread
- No blocking operations in audio thread

### Testing Strategy

#### Unit Testing (Optional)
Create tests for DSP components:
- Oscillator frequency accuracy
- Envelope timing accuracy
- Filter frequency response
- LFO waveform correctness

#### Integration Testing
- Load in multiple DAWs (Reaper, Ableton, Logic, FL Studio)
- Test with different buffer sizes (64, 128, 256, 512, 1024)
- Test with different sample rates (44.1, 48, 88.2, 96 kHz)
- Stress test with maximum polyphony

#### Validation Testing
- A/B comparison with Python version
- Spectrum analysis for waveform accuracy
- Oscilloscope for visual verification
- CPU profiling under load

### Documentation

#### Code Documentation
- Doxygen comments for all public APIs
- Explain complex algorithms (PolyBLEP, filter topology)
- Document parameter ranges and units
- Include references to Python implementation

#### User Documentation
- Quick start guide
- Parameter reference
- Preset guide
- Troubleshooting section

### Version Control

#### Git Workflow
- Commit after each phase completion
- Use descriptive commit messages
- Tag releases (v0.1.0-phase1, v0.2.0-phase2, etc.)
- Create branches for experimental features

#### Code Review
- Review own code before committing
- Check for memory leaks with valgrind/Instruments
- Verify no warnings in Release build
- Test on clean machine before tagging

---

## Deployment & Distribution

### Building Release Versions

#### macOS
- Build universal binary (Apple Silicon + Intel)
- Code sign and notarize
- AU validation with `auval`
- VST3 validation

#### Windows
- Build for x64 and x86 (optional)
- Code sign with certificate
- Test on Windows 10 and 11
- VST3 validation

#### Linux
- Build for common distros
- Package as .deb or .rpm (optional)
- Test on Ubuntu, Fedora

### Packaging
- Create installer (optional)
- Include factory presets
- Include README and license
- Include manual (PDF)

### Distribution
- GitHub releases
- Website download
- Plugin directories (KVR, VST4Free)

---

## Risk Mitigation

### Common Pitfalls

**Aliasing:**
- Always use PolyBLEP for non-bandlimited waveforms
- Test at high MIDI notes (> 96) for aliasing

**Denormal Numbers:**
- Add DC offset removal if filter states denormalize
- Use flush-to-zero mode if available

**Parameter Smoothing:**
- Smooth abrupt parameter changes to avoid zipper noise
- Use linear interpolation for real-time parameter updates

**Voice Stealing Artifacts:**
- Ensure envelope is always in release before stealing
- Consider stealing release-phase voices first

**Thread Safety:**
- Never share non-atomic state between audio and UI threads
- Use `std::atomic` or lock-free queues for communication

### Debugging Tools

**Audio Thread Debugging:**
- Use `juce::Logger` to file (not stdout in audio thread)
- Add assertions for invalid states
- Visualize buffer contents with oscilloscope plugin

**Memory Debugging:**
- Run with AddressSanitizer
- Use Valgrind on Linux
- Instruments on macOS

---

## Success Criteria

### Minimum Viable Product (MVP)
Phase 6 complete represents MVP:
- All synthesis features working
- Usable via DAW automation (no GUI required)
- Sounds identical to Python version
- Ready for testing and feedback

### Feature Complete
Phase 9 complete represents feature parity:
- Professional UI
- Preset system
- All features from Python version
- Ready for public release

### Stretch Goals
Post-Phase 9 enhancements:
- Additional oscillator waveforms (wavetables)
- More filter types (SVF, comb, formant)
- Effects section (reverb, delay, chorus, distortion)
- MPE support (MIDI Polyphonic Expression)
- Microtuning/alternate tuning support
- Arpeggiator
- Step sequencer
- Additional LFO destinations
- Envelope followers
- Per-voice filter option

---

## Development Timeline Approach

Instead of fixed time estimates, use **effort-based phases**:

1. **Quick Wins:** Phases 0, 1, 2, 7 (foundational, straightforward)
2. **Medium Effort:** Phases 3, 4, 5 (requires careful implementation)
3. **High Effort:** Phases 6, 8, 9 (complex features, many components)

Focus on completing one phase before moving to the next. Validate thoroughly at each step.

---

## Next Steps

### Immediate Actions
1. Set up development environment
2. Install JUCE framework
3. Create initial project skeleton
4. Read this plan thoroughly
5. Familiarize yourself with Python reference implementation

### Phase Checklist
Before starting each phase:
- [ ] Read phase description completely
- [ ] Review Python reference line numbers
- [ ] Understand the phase objectives
- [ ] Set up test cases for validation

During phase:
- [ ] Implement features incrementally
- [ ] Test frequently
- [ ] Compare with Python version
- [ ] Document any deviations

After completing phase:
- [ ] Validate against all checklist items
- [ ] Update documentation
- [ ] Git commit with descriptive message
- [ ] Tag if milestone reached

---

## Resources

### JUCE Learning
- Official tutorials: https://juce.com/learn/tutorials
- Documentation: https://docs.juce.com/
- Forum: https://forum.juce.com/
- YouTube: TheAudioProgrammer, The Audio Programmer

### Books
- "Audio Plugin Development" by Will Pirkle
- "Designing Audio Effect Plugins in C++" by Will Pirkle
- "The Audio Programming Book" edited by Boulanger & Lazzarini

### DSP Resources
- Julius O. Smith's books: https://ccrma.stanford.edu/~jos/
- "Designing Sound" by Andy Farnell
- Musicdsp.org archive

### C++ Resources
- "A Tour of C++" by Bjarne Stroustrup
- cppreference.com
- C++ Core Guidelines

---

## Appendix: Python → C++ Mapping Reference

| Component | Python Location | C++ Location | Notes |
|-----------|----------------|--------------|-------|
| Envelope | `sine_generator_qt.py:180-260` | `DSP/Envelope.h/cpp` | State machine logic |
| PolyBLEP | `sine_generator_qt.py:640-680` | `DSP/AudioUtils.cpp` | Convert from vectorized |
| LFO | `sine_generator_qt.py:580-630` | `DSP/LFO.h/cpp` | 5 waveforms |
| Noise | `sine_generator_qt.py:490-577` | `DSP/NoiseGenerator.h/cpp` | Paul Kellet pink noise |
| Moog Filter | `sine_generator_qt.py:370-440` | `DSP/MoogFilter.h/cpp` | 4-pole topology |
| Voice | `sine_generator_qt.py:100-177` | `DSP/Voice.h/cpp` | Container class |
| Audio Callback | `sine_generator_qt.py:3783-3864` | `PluginProcessor::processBlock()` | Main loop |
| MIDI Handling | `sine_generator_qt.py:4050-4140` | `PluginProcessor::processBlock()` | Note on/off |
| Waveform Gen | `sine_generator_qt.py:3555-3615` | `Oscillator::processSample()` | Sine/Saw/Square |
| LFO Mod | `sine_generator_qt.py:3687-3740` | `ModulationRouter` | Modulation application |
| Voice Stealing | `sine_generator_qt.py:4230-4260` | `VoiceManager::stealVoice()` | LRU algorithm |

---

**Ready to build a professional synthesizer plugin!**

*This comprehensive plan serves as your complete development roadmap. Update it as you progress, and use it to track completion of each phase.*
