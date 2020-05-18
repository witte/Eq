cmake_minimum_required(VERSION 3.15)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
project(EqPlugin VERSION 1.0.0)

add_subdirectory(JUCE)

juce_add_plugin(Eq
    COMPANY_NAME "witte"
    IS_SYNTH FALSE
    NEEDS_MIDI_INPUT FALSE
    NEEDS_MIDI_OUTPUT FALSE
    IS_MIDI_EFFECT FALSE
    EDITOR_WANTS_KEYBOARD_FOCUS FALSE
    COPY_PLUGIN_AFTER_BUILD TRUE
    PLUGIN_MANUFACTURER_CODE Witt
    PLUGIN_CODE Eq00
    FORMATS AU VST3
    PRODUCT_NAME "Eq")

target_sources(Eq PRIVATE
    Source/Components/BandEditor.cpp
    Source/Components/Label.cpp
    Source/Components/SpectrumAnalyzer.cpp
    Source/Components/TextEditor.cpp
    Source/LookAndFeel/LookAndFeel.cpp
    Source/EqEditor.cpp
    Source/EqProcessor.cpp)

target_compile_definitions(Eq PUBLIC
    JUCE_DISPLAY_SPLASH_SCREEN=0
    JUCE_WEB_BROWSER=0
    JUCE_USE_CURL=0
    JUCE_VST3_CAN_REPLACE_VST2=0)

juce_add_binary_data(EqData SOURCES
    Source/LookAndFeel/fontaudio.ttf
    Source/LookAndFeel/OpenSansCondensed-Bold.ttf
    Source/LookAndFeel/OpenSansCondensed-Light.ttf
    Source/LookAndFeel/texture.png)


set_target_properties(Eq EqData PROPERTIES POSITION_INDEPENDENT_CODE ON)

target_link_libraries(Eq PRIVATE
    EqData
    juce::juce_audio_processors
    juce::juce_audio_utils
    juce::juce_audio_devices
    juce::juce_gui_basics
    juce::juce_dsp)

