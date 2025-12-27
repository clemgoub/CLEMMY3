# GUI Asset Specifications for CLEMMY3

Comprehensive guide for creating graphical assets for the CLEMMY3 synthesizer plugin.

## Table of Contents
1. [Technical Requirements](#technical-requirements)
2. [App Icon & Branding](#app-icon--branding)
3. [Knobs & Rotary Controls](#knobs--rotary-controls)
4. [Sliders](#sliders)
5. [Buttons & Switches](#buttons--switches)
6. [Background & Panels](#background--panels)
7. [Waveform Displays](#waveform-displays)
8. [LED Indicators](#led-indicators)
9. [Typography & Labels](#typography--labels)
10. [Export Settings](#export-settings)
11. [File Organization](#file-organization)

---

## Technical Requirements

### General Specifications
- **Color Space**: sRGB (critical for consistent colors across platforms)
- **Bit Depth**: 24-bit RGB or 32-bit RGBA (with alpha channel for transparency)
- **File Format**: PNG (lossless, supports transparency)
- **Resolution**: Create at 2x size for Retina/HiDPI displays, then scale down for 1x

### Screen Density Multipliers
Modern plugins should support multiple screen densities:
- **1x**: Standard resolution (96 DPI)
- **2x**: Retina/HiDPI (192 DPI) - **PRIMARY TARGET**
- Optional: 1.5x for some Windows displays

**Best Practice**: Design at 2x size, then let JUCE scale down automatically.

### Color Considerations
- Use dark backgrounds (easier on eyes during long sessions)
- Ensure 4.5:1 contrast ratio for text (WCAG AA accessibility)
- Avoid pure white (#FFFFFF) - use off-white (#E8E8E8) to reduce eye strain
- Test colors in both bright studio and dark bedroom environments

---

## App Icon & Branding

### Plugin Icon (macOS/Windows)
Required for app launchers, plugin browsers, and system displays.

**Sizes Needed:**
- **macOS (.icns file)**:
  - 16x16, 32x32, 64x64, 128x128, 256x256, 512x512, 1024x1024
  - Create at 1024x1024, use tool to generate smaller sizes
- **Windows (.ico file)**:
  - 16x16, 24x24, 32x32, 48x48, 64x64, 128x128, 256x256

**Design Guidelines:**
- Simple, recognizable shape (works at 16x16)
- Bold, high-contrast design
- Avoid fine details that disappear at small sizes
- Consider using "C3" monogram or waveform icon
- Test at smallest size (16x16) first

**File Naming:**
```
icon_1024.png    (master file)
icon_512.png
icon_256.png
... etc
```

**Conversion Tools:**
- macOS: Use `iconutil` or online converters to create .icns
- Windows: Use ImageMagick or online tools to create .ico
- JUCE handles icon setting programmatically

### Logo for Plugin Header
Main branding displayed in plugin UI.

**Specifications:**
- **Size**: 400x80px @2x (200x40px @1x)
- **Format**: PNG with transparency
- **Style**: Horizontal wordmark + icon (optional)
- **Colors**: Metallic gradient or solid color with subtle shine
- **Shadow**: Optional subtle drop shadow for depth

**File Naming:**
```
logo_2x.png      (400x80px)
logo_1x.png      (200x40px)
```

---

## Knobs & Rotary Controls

The most important UI elements in a synthesizer. Users interact with these constantly.

### Specifications

**Knob Size:**
- **Large knobs** (main params): 120x120px @2x (60x60px @1x)
- **Medium knobs** (secondary): 100x100px @2x (50x50px @1x)
- **Small knobs** (fine-tune): 80x80px @2x (40x40px @1x)

**Filmstrip Technique:**
JUCE uses "filmstrip" images - vertical stack of knob frames.

- **Number of Frames**: 64, 128, or 151 frames (151 recommended for smooth motion)
- **Frame Arrangement**: Vertical stack, top = minimum, bottom = maximum
- **Total Image Height**: knobHeight × numFrames
- **Example**: 120px knob × 151 frames = 18,120px tall image

**File Specifications:**
```
knob_main_2x.png        (120 × 18,120px for 151 frames)
knob_secondary_2x.png   (100 × 15,100px for 151 frames)
```

### Design Guidelines

**Visual Style Options:**

1. **Skeuomorphic** (realistic):
   - Metallic material (aluminum, brass, chrome)
   - Subtle gradients and highlights
   - Shadow to show depth
   - Indicator line/dot at 12 o'clock position

2. **Flat/Modern**:
   - Solid colors with subtle gradient
   - Clear indicator line
   - Minimal shadows
   - High contrast

**Indicator Types:**
- **Line**: Extends from center to edge (most common)
- **Dot**: Small circle at edge
- **Pointer**: Triangular shape pointing outward
- **Illuminated arc**: Ring around knob edge

**Creating Filmstrip in Design Software:**

**Adobe Photoshop/Illustrator:**
1. Create smart object of knob base (120x120px)
2. Use Actions to rotate and duplicate 151 times
3. Rotation range: -135° to +135° (270° total sweep)
4. Stack vertically with no gaps
5. Export as single PNG

**Figma/Sketch:**
1. Create component of base knob
2. Use plugin like "Angle" or manual rotation
3. Arrange instances vertically
4. Export as PNG

**Alternative - Pre-rendered:**
1. Model knob in Blender/Cinema4D
2. Render 151 frames as turntable rotation
3. Composite in vertical filmstrip

**Rotation Mapping:**
```
Frame 0:   -135° (7:30 position - minimum)
Frame 75:    0°  (12:00 position - center)
Frame 150: +135° (4:30 position - maximum)
```

---

## Sliders

### Vertical Sliders (Used for ADSR currently)

**Slider Track:**
- **Size**: 30x200px @2x (15x100px @1x)
- **Design**: Subtle groove/channel, darker than background
- **End caps**: Rounded or flat

**Slider Thumb (handle):**
- **Size**: 40x24px @2x (20x12px @1x)
- **Design**: Grippy appearance (horizontal lines or dots)
- **States**: Normal, Hover, Active (optional)

**File Naming:**
```
slider_vertical_track_2x.png    (30x200px)
slider_vertical_thumb_2x.png    (40x24px)
slider_vertical_thumb_hover_2x.png (optional)
```

### Horizontal Sliders (Optional)

**Slider Track:**
- **Size**: 200x30px @2x (100x15px @1x)

**Slider Thumb:**
- **Size**: 24x40px @2x (12x20px @1x)

### Design Guidelines

- Track should have subtle 3D depth (inner shadow)
- Thumb should have highlight to show grabbability
- Consider LED/meter in track showing value fill
- Provide visual feedback for value (text box is good, but visual is better)

---

## Buttons & Switches

### Toggle Buttons (On/Off states)

**Specifications:**
- **Size**: 80x40px @2x (40x20px @1x)
- **States**: Off, On
- **Optional**: Hover, Disabled

**Filmstrip Approach:**
Stack states vertically:
```
State 0: Off
State 1: On
State 2: Hover off (optional)
State 3: Hover on (optional)
```

**File Naming:**
```
button_toggle_2x.png    (80x160px for 2 states)
button_toggle_2x.png    (80x320px for 4 states)
```

### Momentary Buttons (Pressed/Released)

**Specifications:**
- **Size**: 60x60px @2x (30x30px @1x)
- **States**: Normal, Pressed

### Waveform Selector Buttons

For oscillator waveform selection (Sine, Saw, Square).

**Specifications:**
- **Size**: 50x50px @2x each (25x25px @1x)
- **States**: Unselected, Selected
- **Design**: Icon representing waveform shape

**Visual Approach:**
- Simple line graphic of waveform
- Highlight/glow when selected
- Subtle border or background fill

**File Naming:**
```
waveform_sine_unselected_2x.png
waveform_sine_selected_2x.png
waveform_saw_unselected_2x.png
waveform_saw_selected_2x.png
waveform_square_unselected_2x.png
waveform_square_selected_2x.png
```

### Design Guidelines

- Use color/brightness to indicate state
- On state: Brighter, colored LED, or glowing effect
- Off state: Darker, greyscale
- Ensure tactile appearance (looks clickable)

---

## Background & Panels

### Main Background

**Specifications:**
- **Size**: 1600x760px @2x (800x380px @1x) - matches current window size
- **Format**: PNG or JPG (JPG for smaller file size if no transparency needed)
- **Design**: Subtle texture, not distracting

**Design Options:**

1. **Brushed Metal**: Classic hardware synth look
   - Subtle horizontal brush strokes
   - Slight gradient from top to bottom
   - Professional, timeless

2. **Wood Panel**: Vintage warmth
   - High-quality wood texture
   - Realistic grain
   - Walnut, mahogany, or oak tones

3. **Dark Minimal**: Modern, clean
   - Dark grey (#2A2A2A to #3A3A3A)
   - Very subtle noise texture
   - Flat with subtle gradient

4. **Carbon Fiber**: High-tech
   - Carbon weave pattern
   - Subtle shine
   - Modern, premium feel

**Best Practice**: Create tileable texture, then apply as background in JUCE.

### Panel Sections

Divide UI into functional areas (Oscillator, Envelope, LFO, etc.).

**Specifications:**
- **Borders**: 1-2px lines, subtle color (#404040)
- **Section Headers**: Small caps text or embossed labels
- **Depth**: Subtle inner shadow or border to create recessed panels

**File Naming:**
```
background_2x.png
panel_section_2x.png (if using separate panel graphics)
```

---

## Waveform Displays

### Oscilloscope/Waveform Viewer

Real-time visual feedback of audio output (optional but awesome).

**Specifications:**
- **Size**: 400x150px @2x (200x75px @1x)
- **Background**: Dark (#1A1A1A), recessed appearance
- **Waveform Color**: Bright (#00FF88 or #00A8FF)
- **Grid**: Optional subtle grid lines (#2A2A2A)

**Design Elements:**
- Frame/bezel around display
- Subtle screen glow effect
- CRT-style if going vintage
- LCD-style if going modern

**File Naming:**
```
oscilloscope_frame_2x.png    (frame/bezel only)
(waveform drawn programmatically in JUCE)
```

---

## LED Indicators

### Activity LEDs

Show voice activity, LFO phase, or MIDI input.

**Specifications:**
- **Size**: 20x20px @2x (10x10px @1x)
- **States**: Off, On (and optionally different colors)

**Design:**
- Circular or rounded square
- Off state: Dark grey, subtle highlight
- On state: Bright color with glow effect
- Realistic LED bezel

**Colors by Function:**
- **Red**: Clipping, warning
- **Green**: Active, good signal
- **Blue**: MIDI input, modulation
- **Amber**: CPU load, processing

**File Naming:**
```
led_off_2x.png
led_red_2x.png
led_green_2x.png
led_blue_2x.png
```

---

## Typography & Labels

### Font Recommendations

**System Fonts (Safe):**
- **Helvetica Neue** (macOS)
- **Segoe UI** (Windows)
- **Roboto** (Cross-platform)

**Embedded Fonts (More Control):**
- **DIN** - Industrial, technical look
- **Eurostile** - Futuristic, modern
- **Orbitron** - Sci-fi, digital
- **Share Tech Mono** - Monospace, technical

**License Note**: Ensure fonts are licensed for embedding in commercial software.

### Text Rendering

**Parameter Labels:**
- **Size**: 11-13pt (scaled for 2x)
- **Color**: #B8B8B8 (light grey)
- **Weight**: Regular or Medium
- **Style**: All caps for section headers, sentence case for parameters

**Parameter Values:**
- **Size**: 12-14pt
- **Color**: #FFFFFF or #E8E8E8
- **Weight**: Bold or Semibold
- **Style**: Numbers should be clear and readable

**JUCE Handles Text Rendering**, but you can provide:
- Font files (.ttf/.otf) to embed
- Color specifications
- Size guidelines

---

## Export Settings

### Adobe Photoshop

```
File > Export > Export As...
Format: PNG
Size: 100% (if designed at 2x) or 200% (if designed at 1x)
Metadata: None
Color Space: sRGB
Transparency: Checked (if needed)
```

### Figma

```
Select layer/frame
Export settings:
  Format: PNG
  Scale: 2x (or 1x depending on design size)
  Color profile: sRGB
```

### Sketch

```
Select layer
Make Exportable
Format: PNG
Size: 2x
Color profile: sRGB
Export
```

### Blender (for 3D knobs)

```
Render Settings:
  Resolution: Match target size × frames
  Format: PNG
  Color Management: sRGB
  Transparent: Yes (if needed)
  Bit Depth: 8-bit RGB or RGBA
```

---

## File Organization

### Recommended Folder Structure

```
Assets/
├── Icons/
│   ├── icon_1024.png
│   ├── icon_512.png
│   └── ...
├── Logo/
│   ├── logo_2x.png
│   └── logo_1x.png
├── Knobs/
│   ├── knob_main_2x.png
│   ├── knob_secondary_2x.png
│   └── knob_small_2x.png
├── Sliders/
│   ├── slider_vertical_track_2x.png
│   ├── slider_vertical_thumb_2x.png
│   └── slider_horizontal_track_2x.png
├── Buttons/
│   ├── button_toggle_2x.png
│   ├── waveform_sine_selected_2x.png
│   └── ...
├── Backgrounds/
│   ├── background_2x.png
│   └── panel_section_2x.png
├── LEDs/
│   ├── led_off_2x.png
│   ├── led_red_2x.png
│   └── ...
└── Fonts/
    ├── CustomFont-Regular.ttf
    └── CustomFont-Bold.ttf
```

### Naming Convention

```
{element}_{variant}_{density}.png

Examples:
knob_main_2x.png
button_toggle_hover_2x.png
background_2x.png
```

---

## Integration with JUCE

### Loading Images in JUCE

```cpp
// Load 2x image with automatic scaling
auto knobImage = juce::ImageCache::getFromMemory(
    BinaryData::knob_main_2x_png,
    BinaryData::knob_main_2x_pngSize);

// JUCE automatically scales based on display density
```

### Binary Resources

JUCE can embed images directly into the plugin binary:

1. Add images to project via Projucer
2. Images added to `BinaryData` namespace
3. Access via `BinaryData::filename_png`

### LookAndFeel Customization

Custom knobs, sliders, buttons applied via LookAndFeel class:

```cpp
class CLEMMY3LookAndFeel : public juce::LookAndFeel_V4
{
    void drawRotarySlider(...) override
    {
        // Custom knob drawing using filmstrip
    }

    void drawLinearSlider(...) override
    {
        // Custom slider drawing
    }
};
```

---

## Design Workflow Recommendations

### 1. Sketch & Concept (1-2 hours)
- Paper sketches of layout
- Define color palette (3-5 colors)
- Choose aesthetic direction (modern/vintage/minimal)

### 2. Create Core Assets (4-6 hours)
- Start with one knob (get it perfect)
- Create main background
- Design button states
- Keep it consistent

### 3. Test in Mockup (1 hour)
- Composite assets into full UI mockup
- Check at actual size (800x380px)
- Verify contrast and readability

### 4. Refine & Iterate (2-3 hours)
- Adjust colors, shadows, highlights
- Ensure visual hierarchy (important controls stand out)
- Test on different displays (bright/dark environments)

### 5. Export All Assets (1 hour)
- Follow naming convention
- Organize in folders
- Create both 1x and 2x versions

### 6. Implementation (Phase 8)
- Integrate into JUCE
- Fine-tune sizes and positions
- Test on real displays

---

## Quality Checklist

Before finalizing assets:

- [ ] All images at 2x resolution
- [ ] sRGB color space
- [ ] Transparency where needed
- [ ] Consistent style across all elements
- [ ] Tested at actual display size
- [ ] Knob filmstrips have 151 frames
- [ ] File sizes reasonable (< 500KB per image)
- [ ] No jagged edges or compression artifacts
- [ ] Readable text/labels at 1x size
- [ ] Works on both light and dark displays

---

## Inspiration & References

### Plugin UIs to Study
- **Serum** (Xfer Records) - Modern, clean
- **Omnisphere** (Spectrasonics) - Cinematic, premium
- **Diva** (u-he) - Skeuomorphic, vintage
- **Vital** (Matt Tytel) - Futuristic, open-source
- **Pigments** (Arturia) - Colorful, modern

### Asset Resources
- **KnobGallery.com** - Examples of knob designs
- **Plugin Boutique** - Browse plugin screenshots
- **Dribbble** - UI design inspiration (search "synthesizer UI")

### Tools
- **Photoshop/Illustrator** - Industry standard
- **Figma** - Modern, collaborative, free tier
- **Affinity Designer** - One-time purchase alternative
- **Blender** - Free 3D for realistic knobs
- **GIMP** - Free alternative to Photoshop

---

## Summary - Priority Asset List

For CLEMMY3, create these assets first:

### Phase 1 (Essential):
1. Main background (1600x760px @2x)
2. Main knob filmstrip (120x18,120px, 151 frames)
3. Plugin icon (1024x1024px)

### Phase 2 (Important):
4. Secondary knob for smaller controls
5. Button assets (waveform selector, toggle)
6. Logo/branding

### Phase 3 (Polish):
7. LED indicators
8. Custom slider graphics
9. Section panel graphics
10. Oscilloscope frame

---

## Questions?

When implementing in Phase 8, we'll:
1. Create custom LookAndFeel class
2. Load and apply all assets
3. Fine-tune positions and sizes
4. Add animations and interactions
5. Test on different screen densities

Good luck with your designs! Take your time to create assets that match your vision for CLEMMY3.
