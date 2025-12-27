cmake_minimum_required(VERSION 3.15)

project(CLEMMY3 VERSION 0.1.0)

# Add JUCE to the project
add_subdirectory($ENV{HOME}/JUCE ${CMAKE_CURRENT_BINARY_DIR}/JUCE)

# Create the plugin target
juce_add_plugin(CLEMMY3
    COMPANY_NAME "ClemmyAudio"
    IS_SYNTH TRUE
    NEEDS_MIDI_INPUT TRUE
    NEEDS_MIDI_OUTPUT FALSE
    IS_MIDI_EFFECT FALSE
    EDITOR_WANTS_KEYBOARD_FOCUS FALSE
    COPY_PLUGIN_AFTER_BUILD TRUE
    PLUGIN_MANUFACTURER_CODE Clmy
    PLUGIN_CODE Clm3
    FORMATS AU VST3 Standalone
    PRODUCT_NAME "CLEMMY3")

# Add source files
target_sources(CLEMMY3
    PRIVATE
        Source/PluginProcessor.cpp
        Source/PluginEditor.cpp
        Source/DSP/Oscillator.cpp
        Source/DSP/Envelope.cpp)

# Add compile definitions
target_compile_definitions(CLEMMY3
    PUBLIC
        JUCE_WEB_BROWSER=0
        JUCE_USE_CURL=0
        JUCE_VST3_CAN_REPLACE_VST2=0
        JUCE_STANDALONE_FILTER_WINDOW_USE_KIOSK_MODE=0
        JUCE_USE_CUSTOM_PLUGIN_STANDALONE_APP=0)

# Link JUCE libraries
target_link_libraries(CLEMMY3
    PRIVATE
        juce::juce_audio_utils
        juce::juce_audio_processors
    PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags
        juce::juce_recommended_warning_flags)

# Set C++ standard
target_compile_features(CLEMMY3 PRIVATE cxx_std_17)
