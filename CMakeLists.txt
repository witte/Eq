cmake_minimum_required(VERSION 3.15)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
project(EqProject VERSION 1.0.0)

add_subdirectory(JUCE)

juce_add_plugin(Eq
    # VERSION ...                     # Set this if the plugin version is different to the project version
    # ICON_BIG ...                    # ICON_* arguments specify a path to an image file to use as an icon for the Standalone
    # ICON_SMALL ...
    COMPANY_NAME "witte"              # Specify the name of the plugin's author
    IS_SYNTH FALSE                    # Is this a synth or an effect?
    NEEDS_MIDI_INPUT FALSE            # Does the plugin need midi input?
    NEEDS_MIDI_OUTPUT FALSE           # Does the plugin need midi output?
    IS_MIDI_EFFECT FALSE              # Is this plugin a MIDI effect?
    EDITOR_WANTS_KEYBOARD_FOCUS FALSE # Does the editor need keyboard focus?
    COPY_PLUGIN_AFTER_BUILD TRUE      # Should the plugin be installed to a default location after building?
    PLUGIN_MANUFACTURER_CODE Witt     # A four-character manufacturer id with at least one upper-case character
    PLUGIN_CODE Eq00                  # A unique four-character plugin id with at least one upper-case character
    FORMATS AU VST3 Standalone        # The formats to build. Other valid formats are: AAX Unity VST AU AUv3
    PRODUCT_NAME "Eq")                # The name of the final executable, which can differ from the target name

# TO DO: get rid of this and include the modules the proper way!
juce_generate_juce_header(Eq)

target_sources(Eq PRIVATE
    Source/Components/BandEditor.cpp
    Source/Components/Label.cpp
    Source/Components/SpectrumAnalyzer.cpp
    Source/Components/TextEditor.cpp
    Source/LookAndFeel/LookAndFeel.cpp
    Source/EqEditor.cpp
    Source/EqProcessor.cpp)

target_compile_definitions(Eq PUBLIC
#    JUCE_COMPANY_NAME="witte"
    JUCE_DISPLAY_SPLASH_SCREEN=0
    JUCE_WEB_BROWSER=0  # If you remove this, add `NEEDS_WEB_BROWSER TRUE` to the `juce_add_plugin` call
    JUCE_USE_CURL=0     # If you remove this, add `NEEDS_CURL TRUE` to the `juce_add_plugin` call
    JUCE_VST3_CAN_REPLACE_VST2=0)

juce_add_binary_data(EqData SOURCES
    Source/LookAndFeel/fontaudio.ttf
    Source/LookAndFeel/OpenSansCondensed-Bold.ttf
    Source/LookAndFeel/OpenSansCondensed-Light.ttf
    Source/LookAndFeel/texture.png)

set_target_properties(Eq EqData PROPERTIES POSITION_INDEPENDENT_CODE ON)

target_link_libraries(Eq PRIVATE
    EqData
    juce::juce_dsp
    juce::juce_audio_utils)

