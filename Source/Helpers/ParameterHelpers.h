#pragma once
#include <cmath>
#include <juce_audio_processors/juce_audio_processors.h>
#include "UnitConversions.h"

using namespace juce;

namespace witte
{

static std::unique_ptr<AudioParameterBool> makePrmBool
(
    const String& parameterID,
    const String& name,
    const bool defaultValue,
    const String& label = String()
)
{
    return std::make_unique<AudioParameterBool> (parameterID, name, defaultValue, label);
}

static std::unique_ptr<AudioParameterChoice> makePrmChoice
(
    const String& parameterID,
    const String& name,
    const StringArray& choices,
    int defaultItemIndex,
    const String& label = String()
)
{
    return std::make_unique<AudioParameterChoice> (parameterID, name, choices, defaultItemIndex, label);
}

static std::unique_ptr<RangedAudioParameter> makePrmFreq
(
    const String& parameterID,
    const String& name,
    const float   defaultValue = 500.0f,
    const String& label = {}
)
{
    NormalisableRange<float> range { 20.0f, 20000.0f,
                                     [] (float, float, float proportion) { return units::proportionToFreq (proportion); },
                                     [] (float, float, float value)      { return units::freqToProportion (value); } };
    
    return std::make_unique<AudioParameterFloat>
    (
        parameterID,
        name,
        range,
        defaultValue,
        label,
        AudioProcessorParameter::genericParameter,
        [] (float value, int)
        {
                 if (value <   100.0f) return String {value,           1} +  " Hz";
            else if (value <  1000.0f) return String {value,           0} +  " Hz";
            else if (value < 10000.0f) return String {value / 1000.0f, 2} + " kHz";
            else if (value < 20000.0f) return String {value / 1000.0f, 1} + " kHz";
            else                       return String {                    "20 kHz"};
        }
    );
}

static std::unique_ptr<RangedAudioParameter> makePrmDb
(
    const String& parameterID,
    const String& name,
    const float   defaultValue = 0.0f,
    const String& label = {},
    const AudioProcessorParameter::Category category = AudioProcessorParameter::genericParameter
)
{
    return std::make_unique<AudioParameterFloat>
    (
          parameterID,
          name,
          NormalisableRange<float> { -24.0f, 24.0f, 0.01f },
          defaultValue,
          label,
          category,
          [] (float value, int) { return String (value, 2) + " dB"; },
          [] (String text)      { return text.getFloatValue(); }
    );
}

static std::unique_ptr<RangedAudioParameter> makePrmFloat
(
    const String& parameterID,
    const String& name,
    const float   minValue,
    const float   maxValue,
    const float   skew,
    const float   defaultValue,
    const String& label = {},
    AudioProcessorParameter::Category category = AudioProcessorParameter::genericParameter,
    std::function<String (float value, int)>  stringFromValue = [] (float value, int)   { return String (value, 2); },
    std::function<float (const String& text)> valueFromString = [] (const String& text) { return text.getFloatValue(); }
)
{
    return std::make_unique<AudioParameterFloat>
    (
        parameterID,
        name,
        NormalisableRange<float> { minValue, maxValue, 0.01f, skew },
        defaultValue,
        label,
        category,
        stringFromValue,
        valueFromString
    );
}

}
