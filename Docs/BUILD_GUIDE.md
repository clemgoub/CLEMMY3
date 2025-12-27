# CLEMMY3 Build Guide

## Prerequisites

### All Platforms
- CMake 3.15 or higher
- JUCE Framework 8.0+ installed at `~/JUCE`
- C++17 compatible compiler

### macOS
- Xcode 12+ with Command Line Tools
- macOS 10.13 or later

**Installation:**
```bash
xcode-select --install
brew install cmake
```

### Windows
- Visual Studio 2019 or later (with C++ workload)
- Windows 10 or later

**Installation:**
- Download Visual Studio Community from Microsoft
- Install CMake from cmake.org

### Linux
- GCC 9+ or Clang 10+
- Build tools and development libraries

**Installation (Ubuntu/Debian):**
```bash
sudo apt update
sudo apt install build-essential cmake libasound2-dev \
    libfreetype6-dev libx11-dev libxrandr-dev libxinerama-dev \
    libxcursor-dev libgl1-mesa-dev
```

## Building the Project

### Step 1: Clone the Repository
```bash
git clone <your-repo-url>
cd CLEMMY3
```

### Step 2: Configure with CMake
```bash
mkdir build
cd build
cmake ..
```

**Note:** If JUCE is not at `~/JUCE`, set the environment variable:
```bash
export JUCE_DIR=/path/to/JUCE  # macOS/Linux
set JUCE_DIR=C:\path\to\JUCE   # Windows
```

### Step 3: Build
```bash
cmake --build . --config Release
```

Or for faster builds using all CPU cores:
```bash
cmake --build . --config Release -j$(nproc)  # Linux
cmake --build . --config Release -j$(sysctl -n hw.ncpu)  # macOS
```

### Step 4: Install Plugins

**macOS:**
- AU: `~/Library/Audio/Plug-Ins/Components/CLEMMY3.component`
- VST3: `~/Library/Audio/Plug-Ins/VST3/CLEMMY3.vst3`

**Windows:**
- VST3: `C:\Program Files\Common Files\VST3\CLEMMY3.vst3`

**Linux:**
- VST3: `~/.vst3/CLEMMY3.vst3`

Plugins are automatically copied by CMake when `COPY_PLUGIN_AFTER_BUILD` is enabled.

## Development Build

For faster iteration during development:

```bash
mkdir build-debug
cd build-debug
cmake -DCMAKE_BUILD_TYPE=Debug ..
cmake --build .
```

## IDE Integration

### Xcode (macOS)
```bash
mkdir build-xcode
cd build-xcode
cmake -G Xcode ..
open CLEMMY3.xcodeproj
```

### Visual Studio (Windows)
```bash
mkdir build-vs
cd build-vs
cmake -G "Visual Studio 17 2022" ..
```
Open the generated `.sln` file in Visual Studio.

### CLion / VS Code
Open the project folder directly - both IDEs support CMake projects.

## Troubleshooting

### JUCE not found
**Error:** `Could not find JUCE`

**Solution:** Set `JUCE_DIR` environment variable:
```bash
export JUCE_DIR=~/JUCE
```

### AU Validation Fails (macOS)
**Error:** Plugin doesn't appear in DAW

**Solution:** Validate the AU:
```bash
auval -v aumu Clm3 Clmy
```

Check console output for errors.

### VST3 Not Loading
**Error:** VST3 doesn't appear in plugin list

**Solution:**
1. Check plugin is copied to correct directory
2. Rescan plugins in your DAW
3. Check file permissions

### Build Errors
**Error:** Compilation failures

**Solution:**
1. Ensure C++17 support: `cmake .. -DCMAKE_CXX_STANDARD=17`
2. Clean build directory: `rm -rf build && mkdir build`
3. Update JUCE to latest version

## Testing

### Standalone App
```bash
./build/CLEMMY3_artefacts/Release/Standalone/CLEMMY3.app  # macOS
./build/CLEMMY3_artefacts/Release/Standalone/CLEMMY3      # Linux
```

### Load in DAW
1. Launch your DAW (Reaper, Ableton, Logic, etc.)
2. Rescan plugins
3. Create new instrument track
4. Load CLEMMY3
5. Play MIDI notes to test

## Clean Build

To start fresh:
```bash
rm -rf build
mkdir build
cd build
cmake ..
cmake --build . --config Release
```

---

*For development workflow and implementation details, see DEVELOPMENT_PLAN.md*
