# CLEMMY3 UI Customization Guide

**Learn how to modify and customize the CLEMMY3 plugin GUI**

This guide teaches you how to experiment with the GUI layout, control sizes, colors, and visual styling in the CLEMMY3 synthesizer plugin. The GUI is built using JUCE 8.0.12, a professional C++ framework for audio plugin development.

---

## Table of Contents

1. [GUI Architecture Overview](#gui-architecture-overview)
2. [The Layout System: How Components are Positioned](#the-layout-system-how-components-are-positioned)
3. [Modifying Control Sizes and Positions](#modifying-control-sizes-and-positions)
4. [Changing Control Types (Knobs, Sliders, Buttons)](#changing-control-types-knobs-sliders-buttons)
5. [Adding New Controls](#adding-new-controls)
6. [Customizing Colors, Fonts, and Visual Styling](#customizing-colors-fonts-and-visual-styling)
7. [Common Layout Modifications Examples](#common-layout-modifications-examples)
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

    setSize(900, 700);  // Set window size
}
```

```cpp
void CLEMMY3AudioProcessorEditor::paint(juce::Graphics& g)
{
    // Draw backgrounds, section labels, titles, decorations
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

### Current GUI Layout (900×700px)

```
┌─────────────────────────────────────────────────────────────┐
│                         CLEMMY3                              │ ← Title (80px height)
│          Phase 4: Triple Oscillator Architecture             │
├──────────┬──────────┬──────────┬───────────────────────────┤
│  OSC 1   │  OSC 2   │  OSC 3   │         MIXER             │ ← Top Section (330px)
│ (200px)  │ (200px)  │ (200px)  │        (140px)            │
│          │          │          │  Osc.1 Gain               │
│ Enable   │ Enable   │ Enable   │  Osc.2 Gain               │
│ Waveform │ Waveform │ Waveform │  Osc.3 Gain               │
│ Detune   │ Detune   │ Detune   │  Noise Enable/Type/Gain   │
│ Octave   │ Octave   │ Octave   │  Master Volume            │
│ PW       │ PW       │ PW       │                           │
├──────────┴──────────┴──────────┴───────────────────────────┤
│           ADSR ENVELOPE        │  LFO (Coming Soon)        │ ← Second Row (120px)
│  Attack Decay Sustain Release  │                           │
├────────────────────────────────────────────────────────────┤
│                   MIDI Keyboard                            │ ← Keyboard
└────────────────────────────────────────────────────────────┘
```

---

## The Layout System: How Components are Positioned

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
| `removeFromTop(height)` | Remove and return a horizontal strip from top | `area.removeFromTop(50)` |
| `removeFromBottom(height)` | Remove and return a horizontal strip from bottom | `area.removeFromBottom(30)` |
| `removeFromLeft(width)` | Remove and return a vertical strip from left | `area.removeFromLeft(200)` |
| `removeFromRight(width)` | Remove and return a vertical strip from right | `area.removeFromRight(150)` |
| `reduce(horizontal, vertical)` | Shrink rectangle by adding margins | `area.reduce(20, 10)` |
| `setBounds(x, y, width, height)` | Set control position directly | `button.setBounds(10, 20, 100, 30)` |
| `setBounds(Rectangle<int>)` | Set control position from rectangle | `button.setBounds(area.removeFromTop(30))` |

### Example: Positioning a Column of Controls

```cpp
void resized()
{
    auto area = getLocalBounds();

    // Title area (80px height)
    area.removeFromTop(80);

    // Create a column on the left (200px wide)
    auto column = area.removeFromLeft(200);

    // Position controls vertically in the column
    myLabel.setBounds(column.removeFromTop(20));
    myComboBox.setBounds(column.removeFromTop(25));
    column.removeFromTop(5);  // 5px spacing
    mySlider.setBounds(column.removeFromTop(100));
}
```

### The Current CLEMMY3 Layout Strategy

```cpp
void CLEMMY3AudioProcessorEditor::resized()
{
    auto area = getLocalBounds();

    // 1. Title area
    area.removeFromTop(80);

    // 2. Voice mode selector
    auto voiceModeArea = area.removeFromTop(30);
    // ... position voice mode selector in this area

    // 3. Oscillator + Mixer section (horizontal layout)
    auto topSection = area.removeFromTop(330);
    topSection.reduce(20, 0);  // 20px left/right margins

    // Create 4 columns: osc1, osc2, osc3, mixer
    auto osc1Area = topSection.removeFromLeft(200);
    topSection.removeFromLeft(15);  // spacing
    auto osc2Area = topSection.removeFromLeft(200);
    topSection.removeFromLeft(15);  // spacing
    auto osc3Area = topSection.removeFromLeft(200);
    topSection.removeFromLeft(15);  // spacing
    auto mixerArea = topSection;  // remaining width

    // 4. Position controls within each column
    // ... (see layoutOscillatorColumn lambda)

    // 5. ADSR section
    auto adsrArea = area.removeFromTop(120);
    // ... position ADSR sliders

    // 6. MIDI keyboard (uses remaining space)
    midiKeyboard.setBounds(area.reduced(10, 5));
}
```

---

## Modifying Control Sizes and Positions

All control positioning happens in **Source/PluginEditor.cpp** in the `resized()` method.

### Changing Window Size

**File**: Source/PluginEditor.cpp, constructor

```cpp
CLEMMY3AudioProcessorEditor::CLEMMY3AudioProcessorEditor(CLEMMY3AudioProcessor& p)
    : AudioProcessorEditor(&p), audioProcessor(p), midiKeyboard(...)
{
    // ... initialization code ...

    setSize(900, 700);  // ← CHANGE THESE VALUES
    //      ^^^  ^^^
    //      width height
}
```

**Example**: Make window taller for more controls
```cpp
setSize(900, 800);  // 100px taller
```

### Changing Column Widths

**File**: Source/PluginEditor.cpp, resized() method (around line 450)

```cpp
int oscColumnWidth = 200;      // ← Oscillator column width
int mixerColumnWidth = 140;    // ← Mixer column width (not currently used)
int columnSpacing = 15;        // ← Space between columns
```

**Example**: Make oscillator columns narrower, mixer wider
```cpp
int oscColumnWidth = 180;      // Narrower oscillators
int mixerColumnWidth = 180;    // Wider mixer
int columnSpacing = 10;        // Less spacing
```

### Changing Section Heights

**File**: Source/PluginEditor.cpp, resized() method

```cpp
auto topSection = area.removeFromTop(330);  // ← Oscillator+Mixer height
// ...
auto adsrArea = area.removeFromTop(120);    // ← ADSR section height
```

**Example**: More space for oscillators, less for ADSR
```cpp
auto topSection = area.removeFromTop(400);  // 70px taller
auto adsrArea = area.removeFromTop(80);     // 40px shorter
```

### Changing Control Sizes Within Columns

**File**: Source/PluginEditor.cpp, resized() method (around line 455)

```cpp
int sliderHeight = 110;    // ← Knob height (including text box)
int controlSpacing = 5;    // ← Space between controls
```

**Example**: Larger knobs
```cpp
int sliderHeight = 130;    // 20px taller knobs
int controlSpacing = 8;    // More breathing room
```

### Changing Individual Control Sizes

Find the control in the `resized()` method and adjust its bounds:

```cpp
// Original: Waveform selector
waveSelector.setBounds(col.removeFromTop(25));  // 25px tall

// Modified: Taller waveform selector
waveSelector.setBounds(col.removeFromTop(30));  // 30px tall
```

### Adjusting Knob Text Box Size

**File**: Source/PluginEditor.cpp, constructor

```cpp
osc1DetuneSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 18);
//                                                                  ^^  ^^
//                                                                  width height
```

**Example**: Wider text box
```cpp
osc1DetuneSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 80, 20);
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
osc1DetuneSlider.setRange(-100.0, 100.0, 1.0);  // min, max, step
osc1DetuneSlider.setValue(0.0);
osc1DetuneSlider.setTextValueSuffix(" ct");  // Display "0 ct"
```

**Available Rotary Styles**:
- `RotaryHorizontalVerticalDrag` (current): Drag horizontally or vertically
- `RotaryHorizontalDrag`: Drag horizontally only
- `RotaryVerticalDrag`: Drag vertically only

### Vertical Sliders

**Current Implementation** (e.g., attackSlider):

```cpp
// In constructor:
addAndMakeVisible(attackSlider);
attackSlider.setSliderStyle(juce::Slider::LinearVertical);
attackSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 70, 18);
attackSlider.setRange(0.001, 2.0, 0.001);
attackSlider.setValue(0.01);
attackSlider.setTextValueSuffix(" s");
```

**Available Linear Styles**:
- `LinearVertical`: Vertical slider (current for ADSR)
- `LinearHorizontal`: Horizontal slider
- `LinearBar`: Filled bar (like a level meter)

### Converting Knob to Vertical Slider

**Before** (Knob):
```cpp
mySlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
mySlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 18);
```

**After** (Vertical Slider):
```cpp
mySlider.setSliderStyle(juce::Slider::LinearVertical);
mySlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 80, 18);
// Text box typically wider for linear sliders
```

### Converting Slider to Knob

**Before** (Vertical Slider):
```cpp
mySlider.setSliderStyle(juce::Slider::LinearVertical);
```

**After** (Knob):
```cpp
mySlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
```

### Toggle Buttons

**Current Implementation** (e.g., osc1EnableButton):

```cpp
// In constructor:
addAndMakeVisible(osc1EnableButton);
osc1EnableButton.setButtonText("Oscillator 1");
osc1EnableButton.setToggleState(true, juce::dontSendNotification);
```

**Options**:
- Change button text: `setButtonText("My Text")`
- Default state: `setToggleState(false, ...)` for OFF by default

### Text Buttons (Octave +/- Buttons)

**Current Implementation** (e.g., osc1OctaveUpButton):

```cpp
// In constructor:
addAndMakeVisible(osc1OctaveUpButton);
osc1OctaveUpButton.setButtonText("+");
osc1OctaveUpButton.onClick = [this]() {
    auto* param = audioProcessor.parameters.getRawParameterValue("osc1Octave");
    int currentOctave = (int)param->load();
    if (currentOctave < 3)
        audioProcessor.parameters.getParameter("osc1Octave")->setValueNotifyingHost((currentOctave + 1 + 3) / 6.0f);
};
```

**Options**:
- Change button text: `setButtonText("Up")` or `setButtonText("▲")`
- Change button style: Add custom LookAndFeel (advanced)

### ComboBox (Dropdown Selectors)

**Current Implementation** (e.g., osc1WaveformSelector):

```cpp
// In constructor:
addAndMakeVisible(osc1WaveformSelector);
osc1WaveformSelector.addItem("Sine", 1);
osc1WaveformSelector.addItem("Sawtooth", 2);
osc1WaveformSelector.addItem("Square", 3);
osc1WaveformSelector.addItem("Triangle", 4);
osc1WaveformSelector.setSelectedId(1);  // Default: Sine
```

**Options**:
- Add more items: `addItem("My Waveform", 5)`
- Change default: `setSelectedId(3)` for Square default

---

## Adding New Controls

To add a new control to the GUI, you need to modify **THREE places** in the code.

### Example: Adding a Filter Cutoff Knob

#### Step 1: Declare the Control in PluginEditor.h

**File**: Source/PluginEditor.h

```cpp
class CLEMMY3AudioProcessorEditor : public juce::AudioProcessorEditor
{
public:
    // ... existing code ...

private:
    // ... existing controls ...

    // ========== NEW FILTER CONTROLS ==========
    juce::Slider filterCutoffSlider;
    juce::Label filterCutoffLabel;

    // ========== PARAMETER ATTACHMENTS ==========
    // ... existing attachments ...

    // NEW: Filter attachment
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> filterCutoffAttachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(CLEMMY3AudioProcessorEditor)
};
```

#### Step 2: Initialize the Control in PluginEditor.cpp Constructor

**File**: Source/PluginEditor.cpp, constructor (around line 100)

```cpp
CLEMMY3AudioProcessorEditor::CLEMMY3AudioProcessorEditor(CLEMMY3AudioProcessor& p)
    : AudioProcessorEditor(&p), audioProcessor(p), midiKeyboard(...)
{
    // ... existing initialization code ...

    // ========== NEW: FILTER CUTOFF ==========
    addAndMakeVisible(filterCutoffSlider);
    filterCutoffSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    filterCutoffSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 70, 18);
    filterCutoffSlider.setRange(20.0, 20000.0, 1.0);  // 20Hz to 20kHz
    filterCutoffSlider.setValue(1000.0);              // Default: 1kHz
    filterCutoffSlider.setTextValueSuffix(" Hz");
    filterCutoffSlider.setSkewFactorFromMidPoint(1000.0);  // Logarithmic scale

    addAndMakeVisible(filterCutoffLabel);
    filterCutoffLabel.setText("Cutoff", juce::dontSendNotification);
    filterCutoffLabel.setJustificationType(juce::Justification::centred);

    // Create parameter attachment (assumes parameter exists in PluginProcessor)
    filterCutoffAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.parameters, "filterCutoff", filterCutoffSlider);

    // ... rest of constructor ...

    setSize(900, 700);
}
```

#### Step 3: Position the Control in resized() Method

**File**: Source/PluginEditor.cpp, resized() method (around line 550)

```cpp
void CLEMMY3AudioProcessorEditor::resized()
{
    // ... existing layout code ...

    // Position filter cutoff knob (example: add to mixer column)
    auto filterArea = mixerArea.removeFromTop(70);
    filterCutoffLabel.setBounds(filterArea.removeFromTop(18));
    filterCutoffSlider.setBounds(filterArea);
    mixerArea.removeFromTop(5);  // spacing

    // ... rest of layout ...
}
```

#### Step 4: Add Section Label in paint() Method (Optional)

**File**: Source/PluginEditor.cpp, paint() method (around line 400)

```cpp
void CLEMMY3AudioProcessorEditor::paint(juce::Graphics& g)
{
    // ... existing drawing code ...

    g.setFont(12.0f);
    g.setColour(juce::Colours::white);

    // ... existing section labels ...

    g.drawText("FILTER", 665, 300, 215, 20, juce::Justification::centred);

    // ... rest of paint ...
}
```

### Important Notes About Adding Controls

1. **Parameter must exist in PluginProcessor.cpp**: Before adding a GUI control, the parameter must be defined in `CLEMMY3AudioProcessor::createParameterLayout()`
2. **Attachment links GUI to backend**: The `SliderAttachment` automatically syncs the GUI control with the audio processor parameter
3. **Order matters**: Initialize controls in constructor BEFORE calling `setSize()`
4. **Use consistent naming**: Follow the existing naming pattern (e.g., `filterCutoffSlider`, `filterCutoffLabel`, `filterCutoffAttachment`)

---

## Customizing Colors, Fonts, and Visual Styling

All visual drawing happens in the **paint()** method.

### Current paint() Implementation

**File**: Source/PluginEditor.cpp

```cpp
void CLEMMY3AudioProcessorEditor::paint(juce::Graphics& g)
{
    // 1. Background
    g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));

    // 2. Title
    g.setColour(juce::Colours::white);
    g.setFont(28.0f);
    g.drawFittedText("CLEMMY3", getLocalBounds().removeFromTop(50), juce::Justification::centred, 1);

    // 3. Subtitle
    g.setFont(14.0f);
    g.setColour(juce::Colours::lightgrey);
    g.drawFittedText("Phase 4: Triple Oscillator Architecture with Full GUI", getLocalBounds().removeFromTop(70), juce::Justification::centred, 1);

    // 4. Section labels
    g.setFont(12.0f);
    g.setColour(juce::Colours::white);

    g.drawText("OSCILLATOR 1", 20, 100, 200, 20, juce::Justification::centred);
    g.drawText("OSCILLATOR 2", 235, 100, 200, 20, juce::Justification::centred);
    g.drawText("OSCILLATOR 3", 450, 100, 200, 20, juce::Justification::centred);
    g.drawText("MIXER", 665, 100, 215, 20, juce::Justification::centred);
    g.drawText("ADSR ENVELOPE", 20, 460, 400, 20, juce::Justification::centred);

    g.setColour(juce::Colours::grey);
    g.drawText("LFO MODULATION (Coming Soon)", 440, 460, 440, 20, juce::Justification::centred);
}
```

### Changing Background Color

```cpp
// Current: Uses default LookAndFeel color
g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));

// Option 1: Solid color
g.fillAll(juce::Colours::darkgrey);

// Option 2: Custom color (RGB)
g.fillAll(juce::Colour(30, 30, 40));  // Dark blue-grey

// Option 3: Gradient background
g.setGradientFill(juce::ColourGradient(
    juce::Colour(40, 40, 50), 0, 0,           // Start color and position
    juce::Colour(20, 20, 30), 0, getHeight(), // End color and position
    false));  // isRadial
g.fillRect(getLocalBounds());
```

### Changing Title Font and Color

```cpp
// Current:
g.setColour(juce::Colours::white);
g.setFont(28.0f);
g.drawFittedText("CLEMMY3", getLocalBounds().removeFromTop(50), juce::Justification::centred, 1);

// Option 1: Larger, bold title
g.setColour(juce::Colours::cyan);
g.setFont(juce::Font(36.0f, juce::Font::bold));
g.drawFittedText("CLEMMY3", getLocalBounds().removeFromTop(50), juce::Justification::centred, 1);

// Option 2: Custom font (requires font file)
juce::Font customFont = juce::Typeface::createSystemTypefaceFor(BinaryData::MyFont_ttf, BinaryData::MyFont_ttfSize);
customFont = customFont.withHeight(32.0f);
g.setFont(customFont);
g.drawFittedText("CLEMMY3", getLocalBounds().removeFromTop(50), juce::Justification::centred, 1);
```

### Changing Section Label Colors

```cpp
// Current:
g.setColour(juce::Colours::white);
g.drawText("OSCILLATOR 1", 20, 100, 200, 20, juce::Justification::centred);

// Option 1: Orange labels
g.setColour(juce::Colours::orange);
g.drawText("OSCILLATOR 1", 20, 100, 200, 20, juce::Justification::centred);

// Option 2: Different color per section
g.setColour(juce::Colours::red);
g.drawText("OSCILLATOR 1", 20, 100, 200, 20, juce::Justification::centred);

g.setColour(juce::Colours::green);
g.drawText("OSCILLATOR 2", 235, 100, 200, 20, juce::Justification::centred);

g.setColour(juce::Colours::blue);
g.drawText("OSCILLATOR 3", 450, 100, 200, 20, juce::Justification::centred);
```

### Drawing Separator Lines

```cpp
void CLEMMY3AudioProcessorEditor::paint(juce::Graphics& g)
{
    // ... existing code ...

    // Draw horizontal line under title
    g.setColour(juce::Colours::grey);
    g.drawLine(20, 80, getWidth() - 20, 80, 2.0f);  // x1, y1, x2, y2, thickness

    // Draw vertical line between oscillators and mixer
    g.drawLine(655, 100, 655, 430, 1.0f);
}
```

### Drawing Section Backgrounds

```cpp
void CLEMMY3AudioProcessorEditor::paint(juce::Graphics& g)
{
    // ... fill background ...

    // Draw background rectangle for oscillator section
    g.setColour(juce::Colour(50, 50, 60));  // Slightly lighter than background
    g.fillRect(20, 120, 630, 310);  // x, y, width, height

    // Draw border around mixer section
    g.setColour(juce::Colours::grey);
    g.drawRect(665, 120, 215, 310, 2);  // x, y, width, height, borderThickness

    // ... rest of paint ...
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
juce::Colours::purple
```

### Custom Colors (RGB/RGBA)

```cpp
juce::Colour customColor(255, 128, 64);         // RGB (0-255)
juce::Colour transparentColor(255, 128, 64, 128);  // RGBA (last value is alpha/opacity)
```

---

## Common Layout Modifications Examples

### Example 1: Add Space Between Oscillator and Mixer Sections

**File**: Source/PluginEditor.cpp, resized() method

```cpp
// After creating osc3Area, add more spacing:
topSection.removeFromLeft(15);  // Current spacing
auto osc3Area = topSection.removeFromLeft(200);
topSection.removeFromLeft(30);  // INCREASED from 15 to 30
auto mixerArea = topSection;
```

### Example 2: Make ADSR Sliders Wider

**File**: Source/PluginEditor.cpp, resized() method

```cpp
// Current:
int adsrSliderWidth = 80;

// Modified:
int adsrSliderWidth = 100;  // 20px wider
```

### Example 3: Add Title Above Mixer Column

**File**: Source/PluginEditor.cpp, resized() method

```cpp
// In mixer layout section:
auto mixerArea = topSection;
mixerArea.removeFromTop(20);

// ADD THIS:
auto mixerTitleArea = mixerArea.removeFromTop(25);
// Then create a label and position it here (must declare in .h first)
```

### Example 4: Reorder ADSR Sliders (e.g., Sustain Before Decay)

**File**: Source/PluginEditor.cpp, resized() method

```cpp
// Current order: Attack, Decay, Sustain, Release
auto attackArea = adsrArea.removeFromLeft(adsrSliderWidth);
attackLabel.setBounds(attackArea.removeFromTop(15));
attackSlider.setBounds(attackArea);
adsrArea.removeFromLeft(adsrSpacing);

// SWAP DECAY AND SUSTAIN:
// 1. Attack
auto attackArea = adsrArea.removeFromLeft(adsrSliderWidth);
attackLabel.setBounds(attackArea.removeFromTop(15));
attackSlider.setBounds(attackArea);
adsrArea.removeFromLeft(adsrSpacing);

// 2. Sustain (moved before Decay)
auto sustainArea = adsrArea.removeFromLeft(adsrSliderWidth);
sustainLabel.setBounds(sustainArea.removeFromTop(15));
sustainSlider.setBounds(sustainArea);
adsrArea.removeFromLeft(adsrSpacing);

// 3. Decay (moved after Sustain)
auto decayArea = adsrArea.removeFromLeft(adsrSliderWidth);
decayLabel.setBounds(decayArea.removeFromTop(15));
decaySlider.setBounds(decayArea);
adsrArea.removeFromLeft(adsrSpacing);

// 4. Release
auto releaseArea = adsrArea.removeFromLeft(adsrSliderWidth);
releaseLabel.setBounds(releaseArea.removeFromTop(15));
releaseSlider.setBounds(releaseArea);
```

### Example 5: Move MIDI Keyboard to Top

**File**: Source/PluginEditor.cpp, resized() method

```cpp
void CLEMMY3AudioProcessorEditor::resized()
{
    auto area = getLocalBounds();
    area.removeFromTop(80);  // Title area

    // MOVED: MIDI Keyboard at top
    auto keyboardArea = area.removeFromTop(80);
    midiKeyboard.setBounds(keyboardArea.reduced(10, 5));

    area.removeFromTop(10);  // spacing

    // Voice mode selector
    auto voiceModeArea = area.removeFromTop(30);
    // ... rest of layout unchanged
}
```

### Example 6: Stack Oscillators Vertically Instead of Horizontally

**File**: Source/PluginEditor.cpp, resized() method

```cpp
// Instead of:
auto osc1Area = topSection.removeFromLeft(200);
auto osc2Area = topSection.removeFromLeft(200);
auto osc3Area = topSection.removeFromLeft(200);

// Use:
auto osc1Area = topSection.removeFromTop(110);
topSection.removeFromTop(10);  // spacing
auto osc2Area = topSection.removeFromTop(110);
topSection.removeFromTop(10);  // spacing
auto osc3Area = topSection.removeFromTop(110);

// Then call layoutOscillatorColumn with horizontal layout adjustments
```

---

## Debugging Layout Issues

### Common Problems and Solutions

#### Problem 1: Control Not Visible

**Possible Causes**:
1. Forgot to call `addAndMakeVisible(myControl)` in constructor
2. Control positioned outside window bounds
3. Control has zero width or height
4. Control hidden behind another control (z-order)

**Solutions**:
```cpp
// 1. Check constructor:
addAndMakeVisible(myControl);  // Must be called!

// 2. Check bounds in resized():
myControl.setBounds(area.removeFromTop(50));  // Make sure area is valid

// 3. Check calculated size:
DBG("Control bounds: " + myControl.getBounds().toString());

// 4. Change z-order (add control later in constructor):
addAndMakeVisible(backgroundPanel);  // Back
addAndMakeVisible(myControl);        // Front
```

#### Problem 2: Controls Overlapping

**Cause**: Not enough spacing removed between controls

**Solution**: Add more spacing
```cpp
myControl1.setBounds(area.removeFromTop(30));
area.removeFromTop(10);  // ADD SPACING HERE
myControl2.setBounds(area.removeFromTop(30));
```

#### Problem 3: Control Cut Off / Partially Visible

**Cause**: Parent area too small

**Solution**: Increase parent area size
```cpp
// Before:
auto topSection = area.removeFromTop(250);  // Too small

// After:
auto topSection = area.removeFromTop(330);  // Bigger
```

#### Problem 4: Unexpected Layout After Resize

**Cause**: `resized()` uses hard-coded positions instead of relative layout

**Solution**: Use rectangle slicing instead of setBounds(x, y, w, h)
```cpp
// BAD (hard-coded):
myControl.setBounds(100, 50, 200, 30);

// GOOD (relative):
auto area = getLocalBounds().removeFromTop(80);
myControl.setBounds(area.removeFromLeft(200));
```

### Debugging Techniques

#### Technique 1: Use DBG() to Print Rectangle Sizes

```cpp
void CLEMMY3AudioProcessorEditor::resized()
{
    auto area = getLocalBounds();
    DBG("Window bounds: " + area.toString());  // Print to console

    auto topSection = area.removeFromTop(330);
    DBG("Top section: " + topSection.toString());

    // ... rest of layout ...
}
```

**Example Output**:
```
Window bounds: 0, 0, 900, 700
Top section: 0, 80, 900, 330
```

#### Technique 2: Draw Debug Rectangles in paint()

```cpp
void CLEMMY3AudioProcessorEditor::paint(juce::Graphics& g)
{
    // ... normal painting ...

    // DEBUG: Draw red outline around problematic control
    g.setColour(juce::Colours::red);
    g.drawRect(myControl.getBounds(), 2);  // 2px red border
}
```

#### Technique 3: Temporarily Set Opaque Background

```cpp
// In constructor, add:
myControl.setOpaque(true);
myControl.setColour(juce::Slider::backgroundColourId, juce::Colours::blue);

// Now you can see the control's exact bounds
```

#### Technique 4: Use JUCE Component Debugger

1. Run plugin in standalone mode
2. Hold **Cmd+Shift+D** (macOS) or **Ctrl+Shift+D** (Windows)
3. JUCE will overlay component bounds in the window

#### Technique 5: Simplify Layout Temporarily

Comment out most controls to isolate the problem:

```cpp
void CLEMMY3AudioProcessorEditor::resized()
{
    auto area = getLocalBounds();

    // Only layout the problematic control
    myProblematicControl.setBounds(area.removeFromTop(100));

    // Comment out everything else temporarily
    /*
    auto topSection = area.removeFromTop(330);
    // ... rest of layout
    */
}
```

---

## JUCE Documentation References

### Official JUCE Documentation

- **Main Documentation**: https://docs.juce.com/
- **API Reference**: https://docs.juce.com/master/index.html
- **Tutorials**: https://docs.juce.com/master/tutorials.html

### Key Classes for GUI Development

| Class | Documentation | Description |
|-------|---------------|-------------|
| `Component` | [Link](https://docs.juce.com/master/classComponent.html) | Base class for all GUI components |
| `Slider` | [Link](https://docs.juce.com/master/classSlider.html) | Knobs and sliders |
| `Button` | [Link](https://docs.juce.com/master/classButton.html) | Base class for buttons |
| `ToggleButton` | [Link](https://docs.juce.com/master/classToggleButton.html) | On/off buttons |
| `TextButton` | [Link](https://docs.juce.com/master/classTextButton.html) | Clickable text buttons |
| `ComboBox` | [Link](https://docs.juce.com/master/classComboBox.html) | Dropdown selectors |
| `Label` | [Link](https://docs.juce.com/master/classLabel.html) | Text labels |
| `Graphics` | [Link](https://docs.juce.com/master/classGraphics.html) | Drawing context (used in paint()) |
| `Rectangle` | [Link](https://docs.juce.com/master/classRectangle.html) | Rectangle class for layout |
| `Colour` | [Link](https://docs.juce.com/master/classColour.html) | Color class |
| `Font` | [Link](https://docs.juce.com/master/classFont.html) | Font class |
| `LookAndFeel` | [Link](https://docs.juce.com/master/classLookAndFeel.html) | Visual styling system |

### Recommended JUCE Tutorials

1. **The Slider Class**: https://docs.juce.com/master/tutorial_slider_values.html
2. **Component Layout**: https://docs.juce.com/master/tutorial_rectangle_advanced.html
3. **Listeners and Callbacks**: https://docs.juce.com/master/tutorial_listeners_and_broadcasters.html
4. **Custom Graphics**: https://docs.juce.com/master/tutorial_graphics_class.html
5. **Audio Plugin Development**: https://docs.juce.com/master/tutorial_create_projucer_basic_plugin.html

### Other Resources

- **JUCE Forum**: https://forum.juce.com/ (Community help and examples)
- **The Audio Programmer (YouTube)**: https://www.youtube.com/c/TheAudioProgrammer
- **Matkat Music (YouTube)**: https://www.youtube.com/c/MatkatMusic

---

## Quick Reference: Complete Workflow

### To Add a New Knob to the GUI:

1. **Edit PluginEditor.h**: Declare `juce::Slider myKnob;` and `std::unique_ptr<SliderAttachment> myKnobAttachment;`
2. **Edit PluginEditor.cpp constructor**: Initialize knob, add to window, create attachment
3. **Edit PluginEditor.cpp resized()**: Position the knob using rectangle slicing
4. **Edit PluginEditor.cpp paint()** (optional): Add label or decoration
5. **Rebuild**: `cmake --build build --config Release`
6. **Test**: Open plugin and verify the knob appears and works

### To Change Window Size:

1. **Edit PluginEditor.cpp**: Change `setSize(900, 700)` to desired dimensions
2. **Edit PluginEditor.cpp resized()**: Adjust section heights to fit new window size
3. **Rebuild and test**

### To Change Colors:

1. **Edit PluginEditor.cpp paint()**: Modify `g.setColour()` and `g.fillAll()` calls
2. **Rebuild and test**

### To Change Layout:

1. **Edit PluginEditor.cpp resized()**: Modify `removeFromTop/Left/Right/Bottom` calls
2. **Rebuild and test**
3. **Use DBG() or component debugger if layout is incorrect**

---

## Conclusion

You now have the knowledge to customize the CLEMMY3 GUI! The key concepts are:

- **Rectangle slicing** for layout (removeFromTop/Left/Right/Bottom)
- **paint()** for visual styling (colors, fonts, decorations)
- **resized()** for positioning controls
- **Component initialization** in the constructor

Start with small changes and rebuild frequently to see results. Use the debugging techniques if something doesn't look right. Good luck experimenting!

---

**Last Updated**: 2025-12-27
**CLEMMY3 Version**: Phase 4
**JUCE Version**: 8.0.12
