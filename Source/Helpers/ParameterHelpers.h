#pragma once
#include <cmath>
#include <juce_audio_processors/juce_audio_processors.h>
#include "UnitConversions.h"


constexpr auto versionHint = 1;

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
    const juce::AudioParameterBoolAttributes attributes;
    return std::make_unique<juce::AudioParameterBool> (juce::ParameterID(parameterID, versionHint), name, defaultValue, attributes.withLabel(label));
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
    const juce::AudioParameterChoiceAttributes attributes;
    return std::make_unique<juce::AudioParameterChoice> (juce::ParameterID(parameterID, versionHint), name, choices, defaultItemIndex, attributes.withLabel(label));
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
                                           [] (float, float, const float proportion) { return units::proportionToFreq (proportion); },
                                           [] (float, float, const float value)      { return units::freqToProportion (value); } };

    const juce::AudioParameterFloatAttributes attributes;

    return std::make_unique<juce::AudioParameterFloat>
    (
        juce::ParameterID(parameterID, versionHint),
        name,
        range,
        defaultValue,
        attributes
            .withLabel(label)
            .withStringFromValueFunction([] (const float value, int) -> juce::String
        {
            if (value <   100.0f) return juce::String {value,           1} +  " Hz";
            if (value <  1000.0f) return juce::String {value,           0} +  " Hz";
            if (value < 10000.0f) return juce::String {value / 1000.0f, 2} + " kHz";
            if (value < 20000.0f) return juce::String {value / 1000.0f, 1} + " kHz";

            return juce::String { "20 kHz"};
        })
    );
}

inline std::unique_ptr<juce::RangedAudioParameter> makePrmDb
(
    const juce::String& parameterID,
    const juce::String& name,
    const float defaultValue = 0.0f,
    const juce::String& label = {}
) {
    const juce::AudioParameterFloatAttributes attributes;

    return std::make_unique<juce::AudioParameterFloat>
    (
          juce::ParameterID(parameterID, versionHint),
          name,
          juce::NormalisableRange { -24.0f, 24.0f, 0.01f },
          defaultValue,
          attributes
            .withLabel(label)
            .withStringFromValueFunction([] (const float value, int)  { return juce::String (value, 2) + " dB"; })
            .withValueFromStringFunction([] (const juce::String& text) { return text.getFloatValue(); })
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
    const juce::String& label = {}
)
{
    const juce::AudioParameterFloatAttributes attributes;

    return std::make_unique<juce::AudioParameterFloat>
    (
        juce::ParameterID(parameterID, versionHint),
        name,
        juce::NormalisableRange { minValue, maxValue, 0.01f, skew },
        defaultValue,
        attributes
            .withLabel(label)
            .withStringFromValueFunction([] (const float value, int)   { return juce::String (value, 2); })
            .withValueFromStringFunction([] (const juce::String& text) { return text.getFloatValue(); })
    );
}

}
