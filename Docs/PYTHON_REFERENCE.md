# Python → C++ Reference Mapping

This document maps Python implementation line numbers to C++ components for the CLEMMY3 port.

**Python Repository:** https://github.com/clemgoub/TripleOscillator
**Main File:** `sine_generator_qt.py`

---

## Component Mapping

### Envelope Generator
**Python:** `sine_generator_qt.py:180-260`
**C++:** `Source/DSP/Envelope.h/cpp` (Phase 2)

**Key Elements:**
- State machine: Idle → Attack → Decay → Sustain → Release
- Rate calculation from time parameters
- Velocity handling

### PolyBLEP Anti-Aliasing
**Python:** `sine_generator_qt.py:640-680`
**C++:** `Source/DSP/AudioUtils.cpp::polyBLEP()` (Phase 1)

**Key Differences:**
- Convert from NumPy vectorized to scalar per-sample
- Phase range: 0.0-1.0 (not 0-2π)

### LFO Generator
**Python:** `sine_generator_qt.py:580-630`
**C++:** `Source/DSP/LFO.h/cpp` (Phase 6)

**Features:**
- 5 waveforms: Sine, Triangle, Square, Sawtooth, Random
- Free-running and tempo-synced modes

### Noise Generator
**Python:** `sine_generator_qt.py:490-577`
**C++:** `Source/DSP/NoiseGenerator.h/cpp` (Phase 7)

**Algorithms:**
- White noise: Uniform random
- Pink noise: Paul Kellet algorithm
- Brown noise: Integration method

### Moog Ladder Filter
**Python:** `sine_generator_qt.py:370-440`
**C++:** `Source/DSP/MoogFilter.h/cpp` (Phase 5)

**Topology:**
- 4-pole cascaded one-pole filters
- 3 modes: LowPass, BandPass, HighPass
- Resonance feedback with tanh saturation

### Voice Management
**Python:** `sine_generator_qt.py:100-177`
**C++:** `Source/DSP/Voice.h/cpp` (Phase 3)

**Responsibilities:**
- Single voice container
- Oscillator + envelope lifecycle

### Audio Callback
**Python:** `sine_generator_qt.py:3783-3864`
**C++:** `PluginProcessor::processBlock()` (All phases)

**Main Loop:**
- Process MIDI events
- Generate audio samples
- Apply modulations

### MIDI Handling
**Python:** `sine_generator_qt.py:4050-4140`
**C++:** `PluginProcessor::processBlock()` MIDI section (Phase 3)

**Functions:**
- Note-on: Voice allocation
- Note-off: Voice release
- All notes off handling

### Waveform Generation
**Python:** `sine_generator_qt.py:3555-3615` - `generate_waveform()`
**C++:** `Oscillator::processSample()` (Phase 1)

**Waveforms:**
- Sine: `std::sin(phase * 2π)`
- Sawtooth: `2 * phase - 1` + PolyBLEP
- Square: Conditional + PolyBLEP

### LFO Modulation
**Python:** `sine_generator_qt.py:3687-3740` - `apply_lfo_modulation()`
**C++:** `ModulationRouter` (Phase 6)

**Destinations:**
- All Oscs Pitch (vibrato)
- Filter Cutoff (auto-wah)
- All Oscs Volume (tremolo)
- Osc 1/2/3 Pulse Width (PWM)

### Voice Stealing
**Python:** `sine_generator_qt.py:4230-4260` - `steal_voice()`
**C++:** `VoiceManager::stealVoice()` (Phase 3)

**Algorithm:**
- Least Recently Used (LRU)
- Track voice age
- Steal oldest active voice

### Oscillator Processing
**Python:** `sine_generator_qt.py:3679-3750` - `process_oscillator()`
**C++:** `Voice::processSample()` (Phase 4)

**Processing:**
- Mix 3 oscillators
- Apply filter
- Apply envelope

### Triple Oscillator Mixing
**Python:** `sine_generator_qt.py:3812-3814`
**C++:** `Voice::mixOscillators()` (Phase 4)

**Features:**
- Per-oscillator gain
- Enable/disable switches

### Octave Offset
**Python:** `sine_generator_qt.py:2467-2470` - `apply_octave()`
**C++:** `Voice::updateOscillatorFrequencies()` (Phase 4)

**Calculation:**
```cpp
float octaveMultiplier = std::pow(2.0f, octaveOffset);
```

### Detune
**Python:** `sine_generator_qt.py:2463-2465` - `apply_detune()`
**C++:** `Voice::updateOscillatorFrequencies()` (Phase 4)

**Calculation:**
```cpp
float detuneMultiplier = std::pow(2.0f, detuneCents / 1200.0f);
```

### Voice Pool Reallocation
**Python:** `sine_generator_qt.py:998-1020` - `reallocate_voice_pool()`
**C++:** `VoiceManager` mode switching (Phase 3)

**Modes:**
- Mono: Single voice
- Poly: Up to 8 voices
- Unison: All voices on same note

---

## Key Architectural Differences

### Memory Model
| Python | C++ |
|--------|-----|
| Garbage collected | Manual memory management (RAII) |
| Dynamic typing | Static typing |
| Lists/arrays grow dynamically | Pre-allocated arrays (`std::array`) |

### Processing Model
| Python | C++ |
|--------|-----|
| NumPy vectorized (512 samples at once) | Scalar per-sample loop |
| `lfo_signal = np.sin(phase)` | `for (sample...) { sin(phase); phase += inc; }` |

### Audio Buffer
| Python (sounddevice) | C++ (JUCE) |
|---------------------|-----------|
| NumPy array | `juce::AudioBuffer<float>` |
| `data[:, 0] = samples` | `buffer.setSample(ch, sample, value)` |

### Parameter Updates
| Python | C++ |
|--------|-----|
| Direct slider callbacks | `AudioProcessorValueTreeState` |
| Immediate updates | Thread-safe parameter updates |

---

## Phase-by-Phase Reference

### Phase 0: Foundation
- No Python equivalent (project setup)

### Phase 1: Single Oscillator
- `sine_generator_qt.py:640-680` - PolyBLEP
- `sine_generator_qt.py:3555-3615` - Waveform generation

### Phase 2: Envelope
- `sine_generator_qt.py:180-260` - Complete envelope implementation

### Phase 3: Polyphony
- `sine_generator_qt.py:100-177` - Voice class
- `sine_generator_qt.py:4050-4140` - MIDI handling
- `sine_generator_qt.py:4230-4260` - Voice stealing

### Phase 4: Triple Oscillator
- `sine_generator_qt.py:3812-3814` - 3-oscillator processing
- `sine_generator_qt.py:2467-2470` - Octave offset
- `sine_generator_qt.py:2463-2465` - Detune

### Phase 5: Filter
- `sine_generator_qt.py:370-440` - Moog filter

### Phase 6: LFO
- `sine_generator_qt.py:580-630` - LFO generator
- `sine_generator_qt.py:3687-3740` - LFO modulation

### Phase 7: Noise
- `sine_generator_qt.py:490-577` - Noise generator

### Phase 8: GUI
- Reference Python Qt UI for layout ideas
- JUCE components replace Qt widgets

### Phase 9: Presets
- `Presets/` folder - Factory preset examples

---

*This document will be updated as implementation progresses.*
