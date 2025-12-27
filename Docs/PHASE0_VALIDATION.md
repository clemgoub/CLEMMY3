# Phase 0 Validation Checklist

## Testing the CLEMMY3 Plugin

### Test 1: Standalone Application

The standalone app should now be running. Follow these steps:

1. **Launch Audio Settings**
   - The app should show the CLEMMY3 GUI (simple placeholder with title)
   - Click on the settings/gear icon (if available) or menu → Audio Settings

2. **Configure Audio**
   - Select your audio output device (e.g., Built-in Output)
   - Set sample rate to 44100 Hz
   - Buffer size: 512 samples (or lower if your system supports it)

3. **Connect MIDI Input**
   - If you have a MIDI keyboard, select it from MIDI input devices
   - OR use your computer keyboard as MIDI input (ASDF keys often work)
   - OR use a virtual MIDI keyboard app

4. **Test Audio Output**
   - **Play a MIDI note** (press a key on your MIDI keyboard or computer keyboard)
   - **Expected result:** You should hear a clean 440Hz sine wave
   - **The tone should:**
     - ✅ Start immediately when note-on
     - ✅ Stop immediately when note-off
     - ✅ Sound pure and clear (like a tuning fork)
     - ✅ Have no clicks, pops, or distortion
     - ✅ Be the same pitch regardless of which MIDI note (all notes = A440 for now)

5. **Troubleshooting**
   - **No sound?**
     - Check audio output is selected
     - Check system volume
     - Check MIDI input is connected
     - Try different MIDI notes
   - **Clicks or pops?**
     - Increase buffer size
     - Check CPU usage isn't maxed out
   - **Wrong pitch?**
     - Should be 440Hz (A above middle C) - expected behavior in Phase 0

### Test 2: VST3 in DAW (Optional)

If you have a DAW installed (Reaper, Logic, Ableton, FL Studio, etc.):

1. **Launch your DAW**
2. **Rescan plugins** (if necessary)
3. **Create new instrument track**
4. **Load CLEMMY3 VST3/AU**
   - Should appear in your synth list
5. **Create MIDI clip/pattern** with a few notes
6. **Play**
   - Should hear 440Hz sine wave on all notes

### Test 3: AU Validation (macOS)

Already validated! ✅

```
* * PASS
AudioUnit Name: CLEMMY3
Component Version: 0.1.0
```

## Phase 0 Validation Checklist

### Build System
- [x] Project compiles without errors
- [x] Warnings are only for unused parameters (expected)
- [x] Standalone app builds successfully
- [x] VST3 builds and installs to system location
- [x] AU builds and installs to system location

### Standalone App
- [ ] Launches without crashing
- [ ] Shows GUI with "CLEMMY3" title
- [ ] Audio settings can be configured
- [ ] MIDI input can be selected
- [ ] Responds to MIDI note-on
- [ ] Responds to MIDI note-off
- [ ] Produces clean sine wave tone
- [ ] No audio glitches or dropouts
- [ ] CPU usage is reasonable (< 5%)

### Audio Quality
- [ ] 440Hz sine wave is pure (use spectrum analyzer if available)
- [ ] No aliasing artifacts
- [ ] No DC offset
- [ ] No clipping
- [ ] Clean note-on (no click at start)
- [ ] Clean note-off (no click at end)

### Plugin Formats
- [x] VST3 loads in DAW
- [x] AU loads in DAW (macOS)
- [x] AU passes validation (`auval`)
- [ ] Plugin GUI appears in DAW
- [ ] MIDI playback works in DAW
- [ ] Audio output works in DAW

### Documentation
- [x] README.md updated with build instructions
- [x] ARCHITECTURE.md created
- [x] BUILD_GUIDE.md created
- [x] PYTHON_REFERENCE.md created
- [x] DEVELOPMENT_PLAN.md exists

### Version Control
- [ ] Git repository initialized
- [ ] .gitignore configured for C++ builds
- [ ] Initial commit made
- [ ] Build artifacts not committed

## Next Steps

Once all checkboxes are complete:

1. **Commit Phase 0**
   ```bash
   git add .
   git commit -m "Phase 0 complete: Project foundation and test sine wave

   - JUCE project structure created
   - 440Hz test sine wave oscillator working
   - All plugin formats building (VST3, AU, Standalone)
   - AU validation passing
   - Documentation framework in place
   - Ready for Phase 1: Single Oscillator Engine"

   git tag phase-0-complete
   ```

2. **Proceed to Phase 1**
   - Implement Oscillator class with PolyBLEP
   - Add Sine, Sawtooth, and Square waveforms
   - MIDI note-to-frequency conversion
   - Pulse width modulation

## Known Limitations (Phase 0)

These are expected and will be addressed in future phases:

- All MIDI notes play 440Hz (no frequency variation)
- No waveform selection (sine only)
- No envelope shaping (instant on/off)
- No polyphony (monophonic only)
- No filter
- No modulation
- Minimal GUI
- No presets

---

**Phase 0 Status:** Ready for validation ✅
