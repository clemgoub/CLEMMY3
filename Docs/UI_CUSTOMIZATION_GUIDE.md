# CLEMMY3 UI Customization Guide

**Learn how to modify and customize the CLEMMY3 plugin GUI**

This guide teaches you how to experiment with the GUI layout, control sizes, colors, and visual styling in the CLEMMY3 synthesizer plugin. The GUI is built using JUCE 8.0.12, a professional C++ framework for audio plugin development.

---

## Table of Contents

1. [GUI Architecture Overview](#gui-architecture-overview)
2. [Current Layout Structure](#current-layout-structure)
3. [The Layout System: Rectangle Slicing](#the-layout-system-rectangle-slicing)
4. [Modifying Control Sizes and Positions](#modifying-control-sizes-and-positions)
5. [Changing Control Types (Knobs, Sliders, Buttons)](#changing-control-types-knobs-sliders-buttons)
6. [Customizing Colors, Fonts, and Visual Styling](#customizing-colors-fonts-and-visual-styling)
7. [Common Layout Modifications](#common-layout-modifications)
8. [Debugging Layout Issues](#debugging-layout-issues)
9. [JUCE Documentation References](#juce-documentation-references)

---

## GUI Architecture Overview

The CLEMMY3 GUI is defined in two files:

- **Source/PluginEditor.h**: Declares all GUI components and parameter attachments
- **Source/PluginEditor.cpp**: Implements the GUI (initialization, layout, drawing)

### Key Methods in PluginEditor.cpp

```cpp
CLEMMY3AudioProcessorEditor::CLEMMY3AudioProcessorEditor(CLEMMY3AudioProcessor& p)
    : AudioProcessorEditor(&p), audioProcessor(p), midiKeyboard(...)
{
    // 1. Initialize all controls (setSliderStyle, setButtonText, etc.)
    // 2. Add controls to the window (addAndMakeVisible)
    // 3. Create parameter attachments (links controls to backend parameters)

    setSize(1100, 750);  // Current window size
}
```

```cpp
void CLEMMY3AudioProcessorEditor::paint(juce::Graphics& g)
{
    // Draw backgrounds, section labels, titles, borders
    // Called automatically when window needs to redraw
}
```

```cpp
void CLEMMY3AudioProcessorEditor::resized()
{
    // Position all controls (setBounds calls)
    // Called automatically when window is resized
}
```

---

## Current Layout Structure

### Window Layout (1100×750px)

```
┌─────────────────────────────────────────────────────────────────────────┐
│                              CLEMMY3                                     │ ← Title (40px)
│                    Phase 5: Moog Ladder Filter                          │
├─────────────────────────────────┬───────────────────────────────────────┤
│  Voice Mode: [Mono/Poly/Unison] │                                       │ ← 25px
├──────┬──────┬──────┬────────────┴──────────────────────────────────────┤
│ OSC1 │ OSC2 │ OSC3 │              MIXER (horizontal knobs)             │ ← Top Row
│165px │165px │165px │   ┌──────────────────────────────────────┐       │   (360px)
│      │      │      │   │ Osc.1   Osc.2   Osc.3                │       │
│Wave  │Wave  │Wave  │   │  ○       ○       ○                   │       │
│Enable│Enable│Enable│   │                                       │       │
│      │      │      │   │ Noise: [Enable] [Type▼] [Gain]       │       │
│Detune│Detune│Detune│   │ ○ Gain     ○ Master                  │       │
│  ○   │  ○   │  ○   │   └──────────────────────────────────────┘       │
│      │      │      │              FILTER                                │
│ - 0 +│ - 0 +│ - 0 +│   ┌──────────────────────────────────────┐       │
│      │      │      │   │    [LP]  [BP]  [HP]                  │       │
│PWM ○ │PWM ○ │PWM ○ │   │                                       │       │
│      │      │      │   │  Cutoff       Resonance               │       │
│      │      │      │   │    ○             ○                    │       │
│      │      │      │   └──────────────────────────────────────┘       │
├──────┴──────┴──────┴───────────────────────────────────────────────────┤
│    ADSR ENVELOPE         │        LFO MODULATION (Coming Soon)        │ ← Bottom Row
│ Attack Decay Sustain Release                                           │   (180px)
│    |      |      |      |                                              │
├────────────────────────────────────────────────────────────────────────┤
│                          MIDI Keyboard                                 │ ← Remaining
└────────────────────────────────────────────────────────────────────────┘   space
```

### Section Dimensions

- **Window**: 1100×750px
- **Title area**: 40px height
- **Voice mode selector**: 25px height
- **Top row** (Oscillators + Mixer/Filter): 360px height
  - **Oscillators**: 3 columns × 165px width + 10px spacing = 540px total
  - **Right column** (Mixer + Filter stacked): ~515px width
    - **Mixer section**: 180px height (top)
    - **Filter section**: ~160px height (bottom)
- **Bottom row** (ADSR + LFO placeholder): 180px height
- **MIDI Keyboard**: Remaining vertical space (~145px)

---

## The Layout System: Rectangle Slicing

JUCE uses a **rectangle-slicing pattern** for layouts. You start with a rectangle representing the available space, then "remove" portions of it to position controls.

### Basic Concept

```cpp
void resized()
{
    auto area = getLocalBounds();  // Get entire window as rectangle

    // Remove 100px from the top, assign it to a control
    myButton.setBounds(area.removeFromTop(100));

    // The 'area' rectangle is now 100px shorter
    // Continue removing portions for other controls
}
```

### Key Layout Methods

| Method | Description | Example |
|--------|-------------|---------|
| `removeFromTop(height)` | Remove horizontal strip from top | `area.removeFromTop(50)` |
| `removeFromBottom(height)` | Remove horizontal strip from bottom | `area.removeFromBottom(30)` |
| `removeFromLeft(width)` | Remove vertical strip from left | `area.removeFromLeft(200)` |
| `removeFromRight(width)` | Remove vertical strip from right | `area.removeFromRight(150)` |
| `reduce(horizontal, vertical)` | Shrink rectangle (add margins) | `area.reduce(20, 10)` |
| `setBounds(x, y, width, height)` | Set position directly | `button.setBounds(10, 20, 100, 30)` |
| `setBounds(Rectangle<int>)` | Set position from rectangle | `button.setBounds(area.removeFromTop(30))` |

### Current CLEMMY3 Layout Implementation

```cpp
void CLEMMY3AudioProcessorEditor::resized()
{
    auto area = getLocalBounds();

    // 1. Title area (40px)
    area.removeFromTop(40);

    // 2. Voice mode selector (25px)
    auto voiceModeArea = area.removeFromTop(25);
    voiceModeArea.reduce(15, 0);
    voiceModeArea.removeFromLeft(80);
    voiceModeSelector.setBounds(voiceModeArea.removeFromLeft(120));

    area.removeFromTop(5);  // Spacing

    // 3. TOP ROW: Oscillators (left) + Mixer/Filter (right stacked)
    auto topRow = area.removeFromTop(360);
    topRow.reduce(15, 0);  // Left/right margins

    // --- OSCILLATORS (3 columns, 165px each) ---
    auto oscillatorsArea = topRow.removeFromLeft(540);

    int oscWidth = 165;
    int oscSpacing = 10;

    // Layout oscillator 1
    auto osc1Area = oscillatorsArea.removeFromLeft(oscWidth);
    // ... position osc1 controls in osc1Area ...

    oscillatorsArea.removeFromLeft(oscSpacing);

    // Layout oscillator 2
    auto osc2Area = oscillatorsArea.removeFromLeft(oscWidth);
    // ... position osc2 controls ...

    oscillatorsArea.removeFromLeft(oscSpacing);

    // Layout oscillator 3
    auto osc3Area = oscillatorsArea.removeFromLeft(oscWidth);
    // ... position osc3 controls ...

    topRow.removeFromLeft(15);  // Spacing

    // --- RIGHT COLUMN: Mixer (top) + Filter (bottom) stacked ---
    auto rightColumn = topRow;  // All remaining width

    // MIXER SECTION (top 180px)
    auto mixerArea = rightColumn.removeFromTop(180);

    // Mixer knobs arranged horizontally
    int mixerKnobSize = 65;
    int mixerColWidth = 85;
    int mixerSpacing = 8;

    // Labels row
    auto labelRow = mixerArea.removeFromTop(16);
    osc1GainLabel.setBounds(labelRow.removeFromLeft(mixerColWidth));
    labelRow.removeFromLeft(mixerSpacing);
    osc2GainLabel.setBounds(labelRow.removeFromLeft(mixerColWidth));
    labelRow.removeFromLeft(mixerSpacing);
    osc3GainLabel.setBounds(labelRow.removeFromLeft(mixerColWidth));

    mixerArea.removeFromTop(5);

    // Knobs row (horizontal)
    auto knobRow = mixerArea.removeFromTop(mixerKnobSize);
    osc1GainSlider.setBounds(knobRow.removeFromLeft(mixerColWidth));
    knobRow.removeFromLeft(mixerSpacing);
    osc2GainSlider.setBounds(knobRow.removeFromLeft(mixerColWidth));
    knobRow.removeFromLeft(mixerSpacing);
    osc3GainSlider.setBounds(knobRow.removeFromLeft(mixerColWidth));

    // ... noise and master volume controls below ...

    rightColumn.removeFromTop(10);  // Spacing

    // FILTER SECTION (remaining height)
    auto filterArea = rightColumn;

    // Filter mode buttons (LP/BP/HP) in horizontal row
    auto filterModeRow = filterArea.removeFromTop(30);
    // ... position filter mode buttons centered ...

    // Cutoff and Resonance knobs side by side
    auto filterKnobsRow = filterArea;
    auto cutoffArea = filterKnobsRow.removeFromLeft(120);
    filterCutoffLabel.setBounds(cutoffArea.removeFromTop(18));
    filterCutoffSlider.setBounds(cutoffArea);
    // ... similar for resonance ...

    // 4. BOTTOM ROW: ADSR + LFO placeholder
    area.removeFromTop(5);
    auto bottomRow = area.removeFromTop(180);
    bottomRow.reduce(15, 0);

    auto adsrArea = bottomRow.removeFromLeft(360);
    // ... position ADSR sliders (vertical) ...

    // 5. MIDI Keyboard (all remaining space)
    auto keyboardArea = area;
    keyboardArea.reduce(10, 5);
    midiKeyboard.setBounds(keyboardArea);
}
```

---

## Modifying Control Sizes and Positions

### Changing Window Size

**File**: Source/PluginEditor.cpp, constructor (line ~510)

```cpp
CLEMMY3AudioProcessorEditor::CLEMMY3AudioProcessorEditor(CLEMMY3AudioProcessor& p)
{
    // ... initialization code ...

    setSize(1100, 750);  // ← CHANGE THESE VALUES
    //      ^^^^  ^^^
    //      width height
}
```

**Example**: Make window wider for more controls
```cpp
setSize(1300, 750);  // 200px wider
```

### Changing Section Heights

**File**: Source/PluginEditor.cpp, resized() method (around line 560-570)

```cpp
// Top row height (oscillators + mixer/filter)
auto topRow = area.removeFromTop(360);  // ← CHANGE THIS

// Bottom row height (ADSR + LFO)
auto bottomRow = area.removeFromTop(180);  // ← CHANGE THIS
```

**Example**: More space for oscillators
```cpp
auto topRow = area.removeFromTop(420);  // 60px taller
auto bottomRow = area.removeFromTop(150);  // 30px shorter
```

### Changing Oscillator Column Width

**File**: Source/PluginEditor.cpp, resized() method (around line 580)

```cpp
int oscWidth = 165;      // ← Oscillator column width
int oscSpacing = 10;     // ← Space between oscillators
```

**Example**: Wider oscillator columns
```cpp
int oscWidth = 180;      // 15px wider
int oscSpacing = 15;     // More spacing
```

### Changing Mixer Layout

**File**: Source/PluginEditor.cpp, resized() method (around line 705)

```cpp
// Mixer section height
auto mixerArea = rightColumn.removeFromTop(180);  // ← Mixer height

// Mixer knob sizes
int mixerKnobSize = 65;   // ← Knob diameter
int mixerColWidth = 85;   // ← Column width per knob
int mixerSpacing = 8;     // ← Spacing between knobs
```

**Example**: Larger mixer knobs
```cpp
auto mixerArea = rightColumn.removeFromTop(200);  // More height
int mixerKnobSize = 75;   // Larger knobs
int mixerColWidth = 95;   // Wider columns
```

### Removing Text Boxes from Knobs

Mixer knobs currently have no text boxes (NoTextBox). To change this:

**File**: Source/PluginEditor.cpp, constructor (around line 56, 156, 256, 355, 369)

```cpp
// Current (no text box):
osc1GainSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);

// With text box below:
osc1GainSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 18);

// With text box on right:
osc1GainSlider.setTextBoxStyle(juce::Slider::TextBoxRight, false, 50, 18);
```

---

## Changing Control Types (Knobs, Sliders, Buttons)

### Knobs (Rotary Sliders)

**Current Implementation** (e.g., osc1DetuneSlider):

```cpp
// In constructor:
addAndMakeVisible(osc1DetuneSlider);
osc1DetuneSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
osc1DetuneSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 18);
osc1DetuneSlider.setRange(-100.0, 100.0, 1.0);
osc1DetuneSlider.setValue(0.0);
osc1DetuneSlider.setTextValueSuffix(" ct");
```

**Available Rotary Styles**:
- `RotaryHorizontalVerticalDrag`: Drag horizontally or vertically
- `RotaryHorizontalDrag`: Drag horizontally only
- `RotaryVerticalDrag`: Drag vertically only

### Vertical Sliders (ADSR)

**Current Implementation** (e.g., attackSlider):

```cpp
// In constructor:
addAndMakeVisible(attackSlider);
attackSlider.setSliderStyle(juce::Slider::LinearVertical);
attackSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 18);
attackSlider.setRange(0.001, 2.0, 0.001);
attackSlider.setValue(0.01);
attackSlider.setTextValueSuffix(" s");
attackSlider.setSkewFactorFromMidPoint(0.1);  // Logarithmic scale
```

### Toggle Buttons (Filter Mode: LP/BP/HP)

**Current Implementation** (radio button group):

```cpp
// In constructor:
addAndMakeVisible(filterLowPassButton);
filterLowPassButton.setButtonText("LP");
filterLowPassButton.setClickingTogglesState(true);
filterLowPassButton.setRadioGroupId(1001);  // All 3 buttons share this ID
filterLowPassButton.setToggleState(true, juce::dontSendNotification);
filterLowPassButton.onClick = [this]() {
    if (filterLowPassButton.getToggleState())
        audioProcessor.parameters.getParameter("filterMode")->setValueNotifyingHost(0.0f);
};
// Similar for BP and HP buttons
```

**To change to dropdown instead**:
1. Replace 3 TextButtons with 1 ComboBox
2. Add items: `filterModeSelector.addItem("Low Pass", 1);`
3. Create ComboBoxAttachment instead of onClick handlers

### ComboBox (Dropdown Selectors)

**Current Implementation** (e.g., osc1WaveformSelector):

```cpp
// In constructor:
addAndMakeVisible(osc1WaveformSelector);
osc1WaveformSelector.addItem("Sine", 1);
osc1WaveformSelector.addItem("Sawtooth", 2);
osc1WaveformSelector.addItem("Square", 3);
osc1WaveformSelector.addItem("Triangle", 4);
osc1WaveformSelector.addItem("Noise", 5);
osc1WaveformSelector.setSelectedId(1);  // Default: Sine
```

---

## Customizing Colors, Fonts, and Visual Styling

All visual drawing happens in the **paint()** method.

### Current paint() Implementation

**File**: Source/PluginEditor.cpp (line ~522)

```cpp
void CLEMMY3AudioProcessorEditor::paint(juce::Graphics& g)
{
    // 1. Background
    g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));

    // 2. Title
    g.setColour(juce::Colours::white);
    g.setFont(28.0f);
    g.drawFittedText("CLEMMY3", getLocalBounds().removeFromTop(50),
                     juce::Justification::centred, 1);

    // 3. Subtitle
    g.setFont(14.0f);
    g.setColour(juce::Colours::lightgrey);
    g.drawFittedText("Phase 5: Moog Ladder Filter", getLocalBounds().removeFromTop(70),
                     juce::Justification::centred, 1);

    // 4. Section labels
    g.setFont(12.0f);
    g.setColour(juce::Colours::white);

    g.drawText("OSCILLATOR 1", 20, 100, 200, 20, juce::Justification::centred);
    g.drawText("OSCILLATOR 2", 235, 100, 200, 20, juce::Justification::centred);
    g.drawText("OSCILLATOR 3", 450, 100, 200, 20, juce::Justification::centred);

    int rightColX = 570;
    int rightColWidth = 515;
    g.drawText("MIXER", rightColX, 100, rightColWidth, 20, juce::Justification::centred);
    g.drawText("FILTER", rightColX, 285, rightColWidth, 20, juce::Justification::centred);

    g.drawText("ADSR ENVELOPE", 20, 460, 400, 20, juce::Justification::centred);

    g.setColour(juce::Colours::grey);
    g.drawText("LFO MODULATION (Coming Soon)", 440, 460, 640, 20,
               juce::Justification::centred);

    // 5. SECTION BORDERS
    g.setColour(juce::Colours::darkgrey.withAlpha(0.5f));

    int topRowY = 70;
    int topRowHeight = 360;

    // Oscillator borders (3 rectangles)
    int oscWidth = 165;
    int oscSpacing = 10;
    int oscStartX = 15;

    g.drawRect(oscStartX, topRowY + 25, oscWidth, topRowHeight - 25, 1);
    g.drawRect(oscStartX + oscWidth + oscSpacing, topRowY + 25, oscWidth, topRowHeight - 25, 1);
    g.drawRect(oscStartX + (oscWidth + oscSpacing) * 2, topRowY + 25, oscWidth, topRowHeight - 25, 1);

    // Mixer border (top half of right column)
    int mixerHeight = 180;
    g.drawRect(rightColX, topRowY + 25, rightColWidth, mixerHeight, 1);

    // Filter border (bottom half of right column)
    int filterY = topRowY + 25 + mixerHeight + 10;
    int filterHeight = topRowHeight - 25 - mixerHeight - 10;
    g.drawRect(rightColX, filterY, rightColWidth, filterHeight, 1);

    // ADSR border
    int adsrY = topRowY + topRowHeight + 10;
    g.drawRect(15, adsrY + 25, 400, 180, 1);

    // LFO placeholder border
    g.drawRect(430, adsrY + 25, 655, 180, 1);
}
```

### Changing Background Color

```cpp
// Current: Uses default dark background
g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));

// Solid color
g.fillAll(juce::Colours::black);

// Custom RGB color
g.fillAll(juce::Colour(25, 25, 35));  // Dark blue-grey

// Gradient background
g.setGradientFill(juce::ColourGradient(
    juce::Colour(40, 40, 50), 0, 0,           // Top color
    juce::Colour(20, 20, 30), 0, getHeight(), // Bottom color
    false));  // isRadial = false for linear
g.fillRect(getLocalBounds());
```

### Changing Section Border Color and Thickness

```cpp
// Current: Semi-transparent dark grey, 1px thick
g.setColour(juce::Colours::darkgrey.withAlpha(0.5f));
g.drawRect(oscStartX, topRowY + 25, oscWidth, topRowHeight - 25, 1);
//                                                                 ^ thickness

// Brighter, thicker borders
g.setColour(juce::Colours::grey);
g.drawRect(oscStartX, topRowY + 25, oscWidth, topRowHeight - 25, 2);

// Colored borders (cyan)
g.setColour(juce::Colours::cyan.withAlpha(0.6f));
g.drawRect(oscStartX, topRowY + 25, oscWidth, topRowHeight - 25, 1);
```

### Changing Title Font and Color

```cpp
// Current:
g.setColour(juce::Colours::white);
g.setFont(28.0f);
g.drawFittedText("CLEMMY3", getLocalBounds().removeFromTop(50),
                 juce::Justification::centred, 1);

// Larger, bold, colored
g.setColour(juce::Colours::orange);
g.setFont(juce::Font(36.0f, juce::Font::bold));
g.drawFittedText("CLEMMY3", getLocalBounds().removeFromTop(50),
                 juce::Justification::centred, 1);
```

### Drawing Separator Lines

```cpp
void CLEMMY3AudioProcessorEditor::paint(juce::Graphics& g)
{
    // ... existing code ...

    // Horizontal line under title
    g.setColour(juce::Colours::grey);
    g.drawLine(20, 65, getWidth() - 20, 65, 2.0f);  // x1, y1, x2, y2, thickness

    // Vertical line between oscillators and mixer
    g.drawLine(560, 95, 560, 440, 1.5f);
}
```

### Common JUCE Colors

```cpp
juce::Colours::white
juce::Colours::black
juce::Colours::grey
juce::Colours::darkgrey
juce::Colours::lightgrey
juce::Colours::red
juce::Colours::green
juce::Colours::blue
juce::Colours::cyan
juce::Colours::magenta
juce::Colours::yellow
juce::Colours::orange
```

---

## Common Layout Modifications

### Example 1: Make Mixer Knobs Vertical Instead of Horizontal

**File**: Source/PluginEditor.cpp, resized() method (around line 702)

```cpp
// Current: Horizontal layout (removeFromLeft)
auto knobRow = mixerArea.removeFromTop(mixerKnobSize);
osc1GainSlider.setBounds(knobRow.removeFromLeft(mixerColWidth));
osc2GainSlider.setBounds(knobRow.removeFromLeft(mixerColWidth));
osc3GainSlider.setBounds(knobRow.removeFromLeft(mixerColWidth));

// CHANGE TO: Vertical layout (removeFromTop)
osc1GainLabel.setBounds(mixerArea.removeFromTop(16));
osc1GainSlider.setBounds(mixerArea.removeFromTop(mixerKnobSize));
mixerArea.removeFromTop(5);  // Spacing

osc2GainLabel.setBounds(mixerArea.removeFromTop(16));
osc2GainSlider.setBounds(mixerArea.removeFromTop(mixerKnobSize));
mixerArea.removeFromTop(5);

osc3GainLabel.setBounds(mixerArea.removeFromTop(16));
osc3GainSlider.setBounds(mixerArea.removeFromTop(mixerKnobSize));
mixerArea.removeFromTop(5);
```

### Example 2: Change Filter Buttons to Dropdown

**Step 1**: In PluginEditor.h, replace:
```cpp
// Remove these:
juce::TextButton filterLowPassButton;
juce::TextButton filterBandPassButton;
juce::TextButton filterHighPassButton;

// Add this:
juce::ComboBox filterModeSelector;
juce::Label filterModeLabel;
```

**Step 2**: In PluginEditor.cpp constructor, replace button setup with:
```cpp
addAndMakeVisible(filterModeSelector);
filterModeSelector.addItem("Low Pass", 1);
filterModeSelector.addItem("Band Pass", 2);
filterModeSelector.addItem("High Pass", 3);
filterModeSelector.setSelectedId(1);

addAndMakeVisible(filterModeLabel);
filterModeLabel.setText("Mode", juce::dontSendNotification);
```

**Step 3**: In resized(), replace button layout with:
```cpp
auto filterModeRow = filterArea.removeFromTop(25);
filterModeLabel.setBounds(filterModeRow.removeFromLeft(50));
filterModeRow.removeFromLeft(5);
filterModeSelector.setBounds(filterModeRow.removeFromLeft(120));
```

### Example 3: Add More Space Between Sections

**File**: Source/PluginEditor.cpp, resized() method

```cpp
// Current spacing between oscillators and right column:
topRow.removeFromLeft(15);  // 15px spacing

// Increase spacing:
topRow.removeFromLeft(30);  // 30px spacing

// Current spacing between mixer and filter:
rightColumn.removeFromTop(10);  // 10px spacing

// Increase spacing:
rightColumn.removeFromTop(20);  // 20px spacing
```

### Example 4: Reorder Oscillator Controls

Find the oscillator layout section and rearrange the `removeFromTop()` calls:

```cpp
// Current order: Waveform, Enable, Detune, Octave, PWM

// Example new order: Enable, Waveform, Octave, Detune, PWM
auto osc1Area = oscillatorsArea.removeFromLeft(oscWidth);

// 1. Enable button first
auto waveRow = osc1Area.removeFromTop(25);
// ... waveform setup ...

enableBtn.setBounds(osc1Area.removeFromTop(20));
osc1Area.removeFromTop(10);

// 2. Then waveform
// ... (move waveform setup here)

// 3. Octave
// 4. Detune
// 5. PWM
```

### Example 5: Make ADSR Sliders Wider

**File**: Source/PluginEditor.cpp, resized() method (around line 775)

```cpp
// Current:
int adsrSliderWidth = 80;
int adsrSpacing = 12;

// Wider sliders:
int adsrSliderWidth = 100;  // 20px wider
int adsrSpacing = 15;        // More spacing
```

---

## Debugging Layout Issues

### Common Problems

#### Problem 1: Control Not Visible

**Causes**:
1. Forgot `addAndMakeVisible(myControl)` in constructor
2. Control positioned outside window bounds
3. Control has zero width/height
4. Control hidden behind another control

**Solutions**:
```cpp
// 1. Check constructor has:
addAndMakeVisible(myControl);

// 2. Print bounds to console:
DBG("Control bounds: " + myControl.getBounds().toString());

// 3. Temporarily set opaque background:
myControl.setOpaque(true);
myControl.setColour(juce::Slider::backgroundColourId, juce::Colours::red);
```

#### Problem 2: Controls Overlapping

**Cause**: Not enough spacing between controls

**Solution**:
```cpp
control1.setBounds(area.removeFromTop(30));
area.removeFromTop(10);  // ADD SPACING
control2.setBounds(area.removeFromTop(30));
```

#### Problem 3: Section Cut Off

**Cause**: Parent area too small

**Solution**: Increase section height
```cpp
// Before:
auto topRow = area.removeFromTop(330);  // Too small

// After:
auto topRow = area.removeFromTop(400);  // Bigger
```

### Debugging Techniques

#### 1. Print Rectangle Sizes

```cpp
void CLEMMY3AudioProcessorEditor::resized()
{
    auto area = getLocalBounds();
    DBG("Window size: " + juce::String(area.getWidth()) + " x " + juce::String(area.getHeight()));

    auto topRow = area.removeFromTop(360);
    DBG("Top row: " + topRow.toString());
}
```

#### 2. Draw Debug Rectangles

```cpp
void CLEMMY3AudioProcessorEditor::paint(juce::Graphics& g)
{
    // ... normal painting ...

    // DEBUG: Red outline around control
    g.setColour(juce::Colours::red);
    g.drawRect(myControl.getBounds(), 2);
}
```

#### 3. Use JUCE Component Debugger

1. Run plugin in standalone mode
2. Hold **Cmd+Shift+D** (macOS) or **Ctrl+Shift+D** (Windows)
3. JUCE overlays component bounds on the window

#### 4. Simplify Layout Temporarily

Comment out most controls to isolate the problem:

```cpp
void CLEMMY3AudioProcessorEditor::resized()
{
    auto area = getLocalBounds();

    // Only layout the problematic section
    auto topRow = area.removeFromTop(360);
    // ... layout problem area ...

    // Comment out everything else
    /*
    auto bottomRow = area.removeFromTop(180);
    // ... rest of layout
    */
}
```

---

## JUCE Documentation References

### Official Documentation

- **Main Docs**: https://docs.juce.com/
- **API Reference**: https://docs.juce.com/master/index.html
- **Tutorials**: https://docs.juce.com/master/tutorials.html

### Key Classes

| Class | Link | Description |
|-------|------|-------------|
| Component | [docs.juce.com](https://docs.juce.com/master/classComponent.html) | Base class for all GUI components |
| Slider | [docs.juce.com](https://docs.juce.com/master/classSlider.html) | Knobs and sliders |
| Button | [docs.juce.com](https://docs.juce.com/master/classButton.html) | Base button class |
| ComboBox | [docs.juce.com](https://docs.juce.com/master/classComboBox.html) | Dropdown selectors |
| Graphics | [docs.juce.com](https://docs.juce.com/master/classGraphics.html) | Drawing context (paint method) |
| Rectangle | [docs.juce.com](https://docs.juce.com/master/classRectangle.html) | Layout rectangles |
| Colour | [docs.juce.com](https://docs.juce.com/master/classColour.html) | Color handling |

### Recommended Tutorials

1. **Slider Class**: https://docs.juce.com/master/tutorial_slider_values.html
2. **Component Layout**: https://docs.juce.com/master/tutorial_rectangle_advanced.html
3. **Graphics**: https://docs.juce.com/master/tutorial_graphics_class.html
4. **Audio Plugins**: https://docs.juce.com/master/tutorial_create_projucer_basic_plugin.html

---

## Quick Reference

### To Change Window Size:
1. Edit `setSize(1100, 750)` in constructor
2. Adjust section heights in `resized()` to match
3. Rebuild: `cmake --build build --config Release`

### To Change Colors:
1. Edit `paint()` method
2. Modify `g.setColour()` and `g.fillAll()` calls
3. Rebuild and test

### To Move Controls:
1. Edit `resized()` method
2. Change `removeFromTop/Left/Right/Bottom` calls
3. Use DBG() to debug if needed
4. Rebuild and test

### Build and Test Workflow:
```bash
# 1. Build
cmake --build build --config Release

# 2. Launch standalone
open build/CLEMMY3_artefacts/Release/Standalone/CLEMMY3.app

# 3. Or validate AU plugin
auval -v aumu Clm3 Clmy
```

---

## Conclusion

You now have the knowledge to customize the CLEMMY3 GUI! Key concepts:

- **Rectangle slicing** for layout (`removeFromTop/Left/Right/Bottom`)
- **paint()** for visual styling (colors, borders, labels)
- **resized()** for positioning controls
- **Constructor** for initializing controls

Start with small changes and rebuild frequently. Use debugging techniques if layout doesn't look right. Good luck customizing!

---

**Last Updated**: 2025-12-29
**CLEMMY3 Version**: Phase 5 (Moog Ladder Filter)
**JUCE Version**: 8.0.12
