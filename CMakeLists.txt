cmake_minimum_required(VERSION 3.15)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_OSX_ARCHITECTURES "arm64;x86_64")
set(CMAKE_OSX_DEPLOYMENT_TARGET 10.15)

set(PLUGIN_NAME Eq)
set(PLUGIN_DATA ${PLUGIN_NAME}Data)
# add_compile_options(-fsanitize=address)
# add_link_options(-fsanitize=address)
# add_link_options(-fsanitize=thread)

project(${PLUGIN_NAME} VERSION 1.0.0)

add_subdirectory(JUCE)

juce_add_plugin(${PLUGIN_NAME}
    COMPANY_NAME "witte"
    IS_SYNTH FALSE
    NEEDS_MIDI_INPUT FALSE
    NEEDS_MIDI_OUTPUT FALSE
    IS_MIDI_EFFECT FALSE
    EDITOR_WANTS_KEYBOARD_FOCUS FALSE
    COPY_PLUGIN_AFTER_BUILD FALSE
    PLUGIN_MANUFACTURER_CODE Witt
    PLUGIN_CODE Eq00
    FORMATS AU VST3
    PRODUCT_NAME "Eq")

target_sources(${PLUGIN_NAME} PRIVATE
    Source/Components/Utils.cpp
    Source/Components/BandEditor.cpp
    Source/Components/Frame.cpp
    Source/Components/FrequencyCurve.cpp
    Source/Components/Label.cpp
    Source/Components/SpectrumAnalyzer.cpp
    Source/Components/TextEditor.cpp
    Source/Components/XYPad.cpp
    Source/LookAndFeel/LookAndFeel.cpp
    Source/EqEditor.cpp
    Source/EqProcessor.cpp)

target_compile_definitions(${PLUGIN_NAME} PUBLIC
    JUCE_ENABLE_REPAINT_DEBUGGING=0
    JUCE_WEB_BROWSER=0
    JUCE_USE_CURL=0
    JUCE_VST3_CAN_REPLACE_VST2=0
    JUCE_SILENCE_XCODE_15_LINKER_WARNING=1)

juce_add_binary_data(${PLUGIN_DATA} SOURCES
    Source/LookAndFeel/fontaudio.ttf
    Source/LookAndFeel/OpenSansCondensed-Bold.ttf
    Source/LookAndFeel/OpenSansCondensed-Light.ttf
    Source/LookAndFeel/texture.png)

set_target_properties(${PLUGIN_NAME} ${PLUGIN_DATA} PROPERTIES POSITION_INDEPENDENT_CODE ON)

if (UNIX)
    target_compile_options(${PLUGIN_NAME} PUBLIC -Wall -Wextra -pedantic)
else()
    add_definitions("/wd4244") # disable "possible loss of data" warning/error
    target_compile_options(${PLUGIN_NAME} PUBLIC /W4)
endif()

target_link_libraries(${PLUGIN_NAME} PRIVATE
    ${PLUGIN_DATA}
    juce::juce_audio_processors
    juce::juce_audio_utils
    juce::juce_audio_devices
    juce::juce_gui_basics
    juce::juce_dsp)
