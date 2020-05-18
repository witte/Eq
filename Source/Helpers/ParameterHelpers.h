#pragma once
#include <juce_audio_processors/juce_audio_processors.h>

#include "FrequenciesTable.h"

using namespace juce;

namespace witte
{

static inline bool qFuzzyCompare (float p1, float p2)
{
    return (std::abs (p1 - p2) * 100000.f <= std::min (std::abs (p1), std::abs (p2)));
}

static std::unique_ptr<AudioParameterBool> makePrmBool (const String& parameterID,
                                                        const String& name,
                                                        const bool defaultValue,
                                                        const String& label = String())
{
    return std::make_unique<AudioParameterBool> (parameterID, TRANS (name), defaultValue, TRANS (label));
}

static std::unique_ptr<AudioParameterChoice> makePrmChoice (const String& parameterID,
                                                            const String& name,
                                                            const StringArray& choices,
                                                            int defaultItemIndex,
                                                            const String& label = String())
{
    return std::make_unique<AudioParameterChoice> (parameterID, TRANS (name), choices, defaultItemIndex, label);
}

static std::unique_ptr<RangedAudioParameter> makePrmFreq (const String& parameterID,
                                                          const String& name,
                                                          const float   defaultValue = 500.0f,
                                                          const String& label = {})
{
    NormalisableRange<float> range { 20.0f, 20000.0f,
        [] (float, float, float proportion) // proportion to frequency
        {
            return freqTable.at (static_cast<unsigned long> (proportion * (freqTableSize - 1) + 0.5f));
        },
        
        [] (float, float, float value) // frequency to proportion
        {
            long index = 0;
            for (unsigned long i = 0; i < freqTableSize; ++i)
            {
                if (value <= freqTable.at (i))
                {
                    index = i;
                    break;
                }
            }

            return index / (freqTableSize - 1);
        },

        [] (float, float, float value)
        {
            if (value <=    20.0f) return    20.0f;
            if (value >= 20000.0f) return 20000.0f;

            float interval;

                 if (value <    50.0f) interval =   0.5f;
            else if (value <   100.0f) interval =   1.0f;
            else if (value <   200.0f) interval =   2.0f;
            else if (value <   500.0f) interval =   5.0f;
            else if (value <  1000.0f) interval =  10.0f;
            else if (value <  2000.0f) interval =  20.0f;
            else if (value <  5000.0f) interval =  50.0f;
            else if (value < 10000.0f) interval = 100.0f;
            else                       interval = 200.0f;

            return interval * std::floor ((value / interval) + 0.5f);
        }
    };
    
    return std::make_unique<AudioParameterFloat>
    (
        parameterID,
        TRANS (name),
        range,
        defaultValue,
        TRANS (label),
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

static std::unique_ptr<RangedAudioParameter> makePrmDb (const String& parameterID,
                                                        const String& name,
                                                        const float   defaultValue = 0.0f,
                                                        const String& label = {},
                                                        const AudioProcessorParameter::Category category = AudioProcessorParameter::genericParameter)
{
    return std::make_unique<AudioParameterFloat>
    (
          parameterID,
          TRANS (name),
          NormalisableRange<float> { -24.0f, 24.0f, 0.01f },
          defaultValue,
          TRANS (label),
          category,
          [] (float value, int) { return String (value, 2) + " dB"; },
          [] (String text)      { return text.getFloatValue(); }
    );
}

static std::unique_ptr<RangedAudioParameter> makePrmFloat (const String& parameterID,
                                                           const String& name,
                                                           const float   minValue,
                                                           const float   maxValue,
                                                           const float   skew,
                                                           const float   defaultValue,
                                                           const String& label = {},
                                                           AudioProcessorParameter::Category category = AudioProcessorParameter::genericParameter,
                                                           std::function<String (float value, int maximumStringLength)> stringFromValue = nullptr,
                                                           std::function<float (const String& text)> valueFromString = nullptr)
{
    if (stringFromValue == nullptr)
    {
        stringFromValue = [] (float value, int) { return String (value, 2); };
    }
    if (valueFromString == nullptr)
    {
        valueFromString = [] (const String& text) { return text.getFloatValue(); };
    }

    return std::make_unique<AudioParameterFloat>
    (
        parameterID,
        TRANS (name),
        NormalisableRange<float> { minValue, maxValue, 0.01f, skew },
        defaultValue,
        TRANS (label),
        category,
        stringFromValue,
        valueFromString
    );
}

}
