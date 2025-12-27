# Phase 1 Validation Checklist

## Testing CLEMMY3 - Single Oscillator Engine

The standalone app is now running with the new oscillator! Here's how to test Phase 1.

---

## 🎹 Basic Functionality Tests

### Test 1: Different Pitches
**Expected:** Notes now play different pitches!

1. Play MIDI notes from low to high on your keyboard
2. **Listen for:** Each note should be a different pitch
3. **Verify:** A4 (MIDI note 69) = 440Hz
4. **Result:** ✅ Different notes = different frequencies (not all 440Hz anymore!)

### Test 2: Sine Waveform
**Default waveform - pure tone**

1. Ensure waveform is set to "Sine" (default)
2. Play a note
3. **Listen for:** Clean, pure tone (like a tuning fork)
4. **Characteristics:**
   - No overtones or harmonics
   - Smooth, mellow sound
   - Good reference for testing other waveforms

### Test 3: Sawtooth Waveform
**Bright, buzzy waveform with PolyBLEP anti-aliasing**

1. Change waveform to "Sawtooth"
   - In DAW: Automate "Waveform" parameter to value 1
   - In standalone: Will need generic UI (see note below)
2. Play a note
3. **Listen for:**
   - Bright, buzzy, "analog synth" sound
   - Rich in harmonics
   - No harsh aliasing or digital artifacts
   - Smooth, warm character

**PolyBLEP Validation:**
- Play high notes (above C6 / MIDI 84)
- Should remain smooth without aliasing "foldover" artifacts
- Compare to naive sawtooth (if you had one) - PolyBLEP sounds much cleaner

### Test 4: Square Waveform (50% Pulse Width)
**Hollow, clarinet-like tone**

1. Change waveform to "Square"
   - In DAW: Automate "Waveform" parameter to value 2
2. Pulse Width should be at default 50%
3. Play a note
4. **Listen for:**
   - Hollow, "woody" sound (like a clarinet)
   - Only odd harmonics (1st, 3rd, 5th, etc.)
   - Thinner than sawtooth but richer than sine

### Test 5: Pulse Width Modulation (PWM)
**Timbral variation on square wave**

1. Keep waveform on "Square"
2. Adjust Pulse Width parameter:
   - **10% (0.1):** Very thin, nasal sound
   - **30% (0.3):** Thin but musical
   - **50% (0.5):** Classic square wave
   - **70% (0.7):** Getting thicker
   - **90% (0.9):** Very thin again (mirror of 10%)
3. **Listen for:** Timbral changes as you sweep the parameter
4. **Note:** Extreme values (near 0.01 or 0.99) should still sound good (no clicks)

---

## 🔬 Technical Validation

### Audio Quality Checks

**Spectrum Analyzer (if available):**
- **Sine:** Only fundamental frequency visible
- **Sawtooth:** Full harmonic series (1st, 2nd, 3rd, 4th...)
- **Square (50%):** Only odd harmonics (1st, 3rd, 5th, 7th...)

**Aliasing Test:**
1. Set to Sawtooth
2. Play highest notes on keyboard (C7, C8)
3. **Listen for:** NO harsh, inharmonic "foldover" artifacts
4. **PolyBLEP should:** Keep sound smooth and musical even at high frequencies

**Phase Continuity:**
1. Play a note (any waveform)
2. While note is playing, change waveform rapidly
3. **Expected:** Smooth transition, no clicks or pops
4. **Result:** Phase is continuous across waveform changes

**Frequency Accuracy:**
1. Play A4 (MIDI note 69)
2. Use tuner app or spectrum analyzer
3. **Expected:** Exactly 440Hz
4. Test other notes:
   - C4 (MIDI 60) = 261.63 Hz
   - E4 (MIDI 64) = 329.63 Hz
   - C5 (MIDI 72) = 523.25 Hz

---

## 🎛️ Parameter Testing

### Accessing Parameters

**In DAW:**
- Load CLEMMY3 as VST3/AU
- Open automation lane
- You should see:
  - `Waveform` (Sine=0, Sawtooth=1, Square=2)
  - `Pulse Width` (0.01 - 0.99)

**In Standalone (macOS):**
Since we don't have a custom GUI yet, use the generic JUCE parameter window:
1. Menu: View → Show Plugin Parameters (or similar)
2. OR right-click on the plugin window
3. Generic controls should appear

### Parameter Automation (DAW)

1. Create MIDI clip with sustained note
2. **Test 1 - Waveform Sweep:**
   - Automate waveform: 0 → 1 → 2 → 1 → 0
   - Should smoothly transition between sine/saw/square
   - No clicks or glitches

3. **Test 2 - PWM Automation:**
   - Set waveform to Square (2)
   - Automate Pulse Width: 0.1 → 0.9 → 0.1
   - Creates classic PWM chorus effect
   - Sound should morph smoothly

---

## 📋 Phase 1 Completion Checklist

### Core Functionality
- [ ] Different MIDI notes produce different pitches
- [ ] Sine waveform is pure and clean
- [ ] Sawtooth is bright and buzzy
- [ ] Square is hollow and clarinet-like
- [ ] Pulse width changes square wave timbre
- [ ] No audio glitches or dropouts

### Waveform Quality
- [ ] **Sine:** Pure tone, no harmonics
- [ ] **Sawtooth:** No aliasing artifacts (PolyBLEP working)
- [ ] **Square:** Proper odd-harmonic character
- [ ] All waveforms sound musical and professional

### PolyBLEP Anti-Aliasing
- [ ] High notes (C6+) on sawtooth remain smooth
- [ ] No harsh digital artifacts
- [ ] Waveforms sound "analog" and warm
- [ ] Works across full MIDI range (0-127)

### MIDI & Pitch
- [ ] Correct pitch for all MIDI notes
- [ ] A4 (MIDI 69) = 440Hz
- [ ] Note-on starts sound immediately
- [ ] Note-off stops sound immediately
- [ ] No stuck notes

### Parameters
- [ ] Waveform parameter switches between 3 waveforms
- [ ] Pulse Width parameter affects square wave (0.01-0.99)
- [ ] Parameter changes don't cause clicks
- [ ] Automation works in DAW
- [ ] Preset save/load works (ValueTreeState)

### Edge Cases
- [ ] Rapid note retriggering works
- [ ] Changing waveform while note playing = smooth
- [ ] Extreme pulse width values (0.01, 0.99) sound okay
- [ ] Works at different sample rates (44.1k, 48k, 96k)
- [ ] Works at different buffer sizes (64, 128, 512)

### Performance
- [ ] CPU usage reasonable (< 2% for single voice)
- [ ] No audio dropouts under normal use
- [ ] Stable over extended playback

---

## 🐛 Known Limitations (Will Fix in Later Phases)

These are expected and will be addressed:

- **Monophonic only:** Can only play one note at a time
- **No envelope:** Notes start/stop instantly (no fade in/out)
- **No filter:** Can't change brightness/tone
- **No modulation:** No vibrato, tremolo, or PWM automation from LFO
- **Minimal GUI:** Using generic JUCE parameter window
- **Only 1 oscillator:** Not 3 oscillators yet

---

## 🎯 Success Criteria

Phase 1 is complete when:

1. ✅ All three waveforms sound professional and musical
2. ✅ PolyBLEP eliminates aliasing artifacts
3. ✅ Different MIDI notes produce correct frequencies
4. ✅ Pulse width modulation works smoothly
5. ✅ No audio glitches, clicks, or artifacts
6. ✅ Parameters are automatable in DAW

---

## 🚀 What's Next: Phase 2 - ADSR Envelope

After validating Phase 1, we'll add:
- Attack, Decay, Sustain, Release envelope
- Smooth note-on/off transitions
- Velocity sensitivity
- Proper voice lifecycle management

This will make notes fade in/out naturally instead of starting/stopping instantly.

---

**Test the plugin and let me know how it sounds!**
