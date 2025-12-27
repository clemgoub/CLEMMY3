# CLEMMY3 Architecture

## System Overview

CLEMMY3 is a triple oscillator subtractive synthesizer plugin implemented in C++ using the JUCE framework.

## Current Status: Phase 0

**Phase 0 Test Implementation:**
- Basic JUCE plugin scaffold
- Test 440Hz sine wave generator
- MIDI note-on/off handling

## Signal Flow (Final Architecture)

```
MIDI Input
    ↓
Voice Manager (8 voices, Mono/Poly/Unison modes)
    ↓
Voice (per voice):
    ├── Oscillator 1 (Sine/Saw/Square + PolyBLEP)
    ├── Oscillator 2 (Sine/Saw/Square + PolyBLEP)
    ├── Oscillator 3 (Sine/Saw/Square + PolyBLEP)
    ├── Noise Generator (White/Pink/Brown)
    ↓
Mixer (per-oscillator gain + noise)
    ↓
Moog Ladder Filter (LP/BP/HP, Cutoff, Resonance)
    ↓
Envelope (ADSR)
    ↓
Voice Output
    ↓
Sum All Voices
    ↓
LFO Modulation (2x LFO)
    ├── LFO 1 → 7 destinations
    ├── LFO 2 → 7 destinations
    ↓
Master Output
```

## Module Descriptions

### Phase 0: Foundation (CURRENT)
- **PluginProcessor:** Main audio processing entry point
- **PluginEditor:** Basic GUI placeholder
- **Test Oscillator:** Simple 440Hz sine wave generator

### Phase 1: Single Oscillator (PLANNED)
- **Oscillator:** Waveform generation (Sine/Saw/Square)
- **AudioUtils:** PolyBLEP anti-aliasing, MIDI conversion

### Phase 2: Envelope (PLANNED)
- **Envelope:** ADSR envelope generator with state machine

### Phase 3: Polyphony (PLANNED)
- **Voice:** Container for oscillator + envelope
- **VoiceManager:** Voice allocation, stealing (LRU), mode control

### Phase 4: Triple Oscillator (PLANNED)
- **Voice Enhancement:** 3 oscillators per voice with mixing

### Phase 5: Filter (PLANNED)
- **MoogFilter:** 4-pole Moog ladder filter implementation

### Phase 6: LFO (PLANNED)
- **LFO:** Low-frequency oscillator with 5 waveforms
- **ModulationRouter:** Modulation destination routing

### Phase 7: Noise (PLANNED)
- **NoiseGenerator:** White/Pink/Brown noise source

### Phase 8: GUI (PLANNED)
- **UI Components:** Full graphical interface

### Phase 9: Presets (PLANNED)
- **Preset System:** Save/load functionality

## Threading Model

### Audio Thread (Real-time)
- `PluginProcessor::processBlock()` - MIDI processing and audio generation
- All DSP components (Oscillator, Envelope, Filter, LFO, etc.)
- **Requirements:** Lock-free, no allocations, deterministic timing

### Message Thread (Non-real-time)
- `PluginEditor` - GUI rendering and user interaction
- Parameter updates
- File I/O (presets)

### Communication
- Parameters: JUCE AudioProcessorValueTreeState (thread-safe)
- Lock-free: std::atomic for flags and simple values

## Memory Management

- **Pre-allocated:** All DSP buffers and voice arrays allocated in `prepareToPlay()`
- **No allocations in audio thread:** Use std::array instead of std::vector
- **RAII:** Smart pointers for UI components

## Performance Targets

- **CPU Usage:** < 5% at 64 sample buffer, 44.1kHz
- **Polyphony:** 8 voices
- **Latency:** Minimal (buffer size dependent)
- **Sample Rates:** 44.1, 48, 88.2, 96 kHz

## File Structure

```
CLEMMY3/
├── Source/
│   ├── PluginProcessor.h/cpp     [Main audio processor]
│   ├── PluginEditor.h/cpp        [GUI]
│   └── DSP/                      [To be created in Phase 1+]
├── Docs/
│   ├── ARCHITECTURE.md           [This file]
│   ├── DEVELOPMENT_PLAN.md       [Implementation roadmap]
│   └── PYTHON_REFERENCE.md       [To be created]
└── Resources/
    ├── Presets/
    └── Images/
```

## Build System

- **CMake:** Cross-platform build configuration
- **JUCE:** Framework providing audio plugin infrastructure
- **C++17:** Language standard
- **Formats:** VST3, AU, Standalone

---

*This document will be updated as each phase is completed.*
