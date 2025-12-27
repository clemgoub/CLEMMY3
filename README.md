# CLEMMY3

A vibe-coded triple oscillator subtractive synthesizer plug-in, free for everyone to have fun with!

## Description

C++/JUCE port of the [Python Triple Oscillator](https://github.com/clemgoub/TripleOscillator) synthesizer, creating a professional VST3/AU plugin for use in any DAW.

**Status:** 🚧 Phase 0 - Foundation & Project Setup

## Features (Planned)

- 🎹 Triple oscillator architecture (Sine/Sawtooth/Square waveforms)
- 🎚️ 8-voice polyphony with Mono/Poly/Unison modes
- 🔊 Moog ladder filter (LP/BP/HP)
- 📊 Dual LFO system with multiple destinations
- 🎵 ADSR envelope generator
- 🎲 White/Pink/Brown noise generator
- 💾 Preset system
- 🖥️ Professional GUI

## Current Phase: Phase 0

**Implemented:**
- ✅ JUCE project structure
- ✅ Basic plugin scaffold
- ✅ Test 440Hz sine wave generator
- ✅ MIDI note-on/off handling
- ✅ Documentation framework

**Status:** Ready to build and test

## Building

### Prerequisites

- CMake 3.15+
- C++17 compiler (Xcode 12+, Visual Studio 2019+, or GCC 9+)
- JUCE Framework 8.0+ installed at `~/JUCE`

### Build Instructions

```bash
# Configure
mkdir build
cd build
cmake ..

# Build
cmake --build . --config Release

# The plugins will be automatically installed to:
# macOS: ~/Library/Audio/Plug-Ins/VST3/ and ~/Library/Audio/Plug-Ins/Components/
# Windows: C:\Program Files\Common Files\VST3\
# Linux: ~/.vst3/
```

For detailed build instructions, see [BUILD_GUIDE.md](Docs/BUILD_GUIDE.md)

## Development

See [DEVELOPMENT_PLAN.md](DEVELOPMENT_PLAN.md) for the complete implementation roadmap.

## Project Structure

```
CLEMMY3/
├── Source/              # C++ source code
│   ├── PluginProcessor  # Main audio engine
│   └── PluginEditor     # GUI (basic placeholder)
├── Docs/                # Documentation
│   ├── ARCHITECTURE.md  # System design
│   ├── BUILD_GUIDE.md   # Build instructions
│   └── PYTHON_REFERENCE.md  # Python→C++ mapping
├── Resources/           # Plugin assets
│   ├── Presets/
│   └── Images/
└── reference-python/    # Original Python implementation
```

## Testing

1. **Standalone App:**
   ```bash
   open build/CLEMMY3_artefacts/Release/Standalone/CLEMMY3.app
   ```

2. **In DAW:**
   - Load VST3 or AU in your DAW
   - Play MIDI notes
   - Should hear 440Hz sine wave

## License

See [LICENSE](LICENSE)

## Credits

- Original Python implementation: [clemgoub/TripleOscillator](https://github.com/clemgoub/TripleOscillator)
- Built with [JUCE Framework](https://juce.com)

---

**Phase Progress:** 0/9 (0% Complete)
