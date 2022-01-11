#pragma once
#include <cmath>
#include <juce_audio_processors/juce_audio_processors.h>
#include "UnitConversions.h"


namespace witte
{

inline std::unique_ptr<juce::AudioParameterBool> makePrmBool
(
    const juce::String& parameterID,
    const juce::String& name,
    const bool defaultValue,
    const juce::String& label = juce::String()
)
{
    return std::make_unique<juce::AudioParameterBool> (parameterID, name, defaultValue, label);
}

inline std::unique_ptr<juce::AudioParameterChoice> makePrmChoice
(
    const juce::String& parameterID,
    const juce::String& name,
    const juce::StringArray& choices,
    int defaultItemIndex,
    const juce::String& label = juce::String()
)
{
    return std::make_unique<juce::AudioParameterChoice> (parameterID, name, choices, defaultItemIndex, label);
}

inline std::unique_ptr<juce::RangedAudioParameter> makePrmFreq
(
    const juce::String& parameterID,
    const juce::String& name,
    const float   defaultValue = 500.0f,
    const juce::String& label = {}
)
{
    juce::NormalisableRange<float> range { 20.0f, 20000.0f,
                                           [] (float, float, float proportion) { return units::proportionToFreq (proportion); },
                                           [] (float, float, float value)      { return units::freqToProportion (value); } };
    
    return std::make_unique<juce::AudioParameterFloat>
    (
        parameterID,
        name,
        range,
        defaultValue,
        label,
        juce::AudioProcessorParameter::genericParameter,
        [] (float value, int)
        {
                 if (value <   100.0f) return juce::String {value,           1} +  " Hz";
            else if (value <  1000.0f) return juce::String {value,           0} +  " Hz";
            else if (value < 10000.0f) return juce::String {value / 1000.0f, 2} + " kHz";
            else if (value < 20000.0f) return juce::String {value / 1000.0f, 1} + " kHz";
            else                       return juce::String {                    "20 kHz"};
        }
    );
}

inline std::unique_ptr<juce::RangedAudioParameter> makePrmDb
(
    const juce::String& parameterID,
    const juce::String& name,
    const float defaultValue = 0.0f,
    const juce::String& label = {},
    const juce::AudioProcessorParameter::Category category = juce::AudioProcessorParameter::genericParameter
)
{
    return std::make_unique<juce::AudioParameterFloat>
    (
          parameterID,
          name,
          juce::NormalisableRange<float> { -24.0f, 24.0f, 0.01f },
          defaultValue,
          label,
          category,
          [] (float value, int)  { return juce::String (value, 2) + " dB"; },
          [] (juce::String text) { return text.getFloatValue(); }
    );
}

inline std::unique_ptr<juce::RangedAudioParameter> makePrmFloat
(
    const juce::String& parameterID,
    const juce::String& name,
    const float         minValue,
    const float         maxValue,
    const float         skew,
    const float         defaultValue,
    const juce::String& label = {},
    juce::AudioProcessorParameter::Category category = juce::AudioProcessorParameter::genericParameter,
    std::function<juce::String (float value, int)>  stringFromValue = [] (float value, int)   { return juce::String (value, 2); },
    std::function<float (const juce::String& text)> valueFromString = [] (const juce::String& text) { return text.getFloatValue(); }
)
{
    return std::make_unique<juce::AudioParameterFloat>
    (
        parameterID,
        name,
        juce::NormalisableRange<float> { minValue, maxValue, 0.01f, skew },
        defaultValue,
        label,
        category,
        stringFromValue,
        valueFromString
    );
}

}
