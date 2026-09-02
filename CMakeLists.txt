cmake_minimum_required(VERSION 4.3)
project(Eq VERSION 1.0.0)


set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_OSX_ARCHITECTURES "arm64;x86_64")
set(CMAKE_OSX_DEPLOYMENT_TARGET 10.15)
# add_compile_options(-fsanitize=address)
# add_link_options(-fsanitize=address)
# add_link_options(-fsanitize=thread)


add_subdirectory(JUCE)

juce_add_plugin(Eq
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

target_sources(Eq PRIVATE
    Source/Components/Utils.cpp
    Source/Components/BandEditor.cpp
    Source/Components/Frame.cpp
    Source/Components/FrequencyCurve.cpp
    Source/Components/Label.cpp
    Source/Components/SpectrumAnalyzer.cpp
    Source/Components/TextEditor.cpp
    Source/Components/XYPad.cpp
    Source/LookAndFeel/LookAndFeel.cpp
    Source/Editor.cpp
    Source/Band.cpp
    Source/Processor.cpp)

target_compile_definitions(Eq PUBLIC
    JUCE_ENABLE_REPAINT_DEBUGGING=0
    JUCE_WEB_BROWSER=0
    JUCE_USE_CURL=0
    JUCE_VST3_CAN_REPLACE_VST2=0
    JUCE_SILENCE_XCODE_15_LINKER_WARNING=1)

juce_add_binary_data(EqData SOURCES
    Source/LookAndFeel/fontaudio.ttf
    Source/LookAndFeel/OpenSansCondensed-Bold.ttf
    Source/LookAndFeel/OpenSansCondensed-Light.ttf
    Source/LookAndFeel/texture.png)

set_target_properties(Eq EqData PROPERTIES POSITION_INDEPENDENT_CODE ON)

if (UNIX)
    target_compile_options(Eq PUBLIC -Wall -Wextra -pedantic)
else()
    add_definitions("/wd4244") # disable "possible loss of data" warning/error
    target_compile_options(Eq PUBLIC /W4)
endif()

target_link_libraries(Eq PRIVATE
    EqData
    juce::juce_audio_processors
    juce::juce_audio_utils
    juce::juce_audio_devices
    juce::juce_gui_basics
    juce::juce_dsp)


include(cmake/pluginval.cmake)
add_pluginval(Eq)
if (APPLE)
    include(cmake/auval.cmake)
    add_auval(Eq)
endif ()
