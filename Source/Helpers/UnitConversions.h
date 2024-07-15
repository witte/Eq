#pragma once
#include <cmath>


namespace witte::units
{


inline float proportionToFreq (const float proportion)
{
    return 20.0f * std::pow (2.0f, proportion * 10.0f);
}

inline float freqToProportion (const float freq)
{
    return (std::log (freq / 20.0f) / std::log (2.0f)) / 10.0f;
}

inline float jmax (const float a, const float b) { return a < b ? b : a; }

inline float jmap (const float sourceValue, const float sourceRangeMin, const float sourceRangeMax,
                   const float targetRangeMin, const float targetRangeMax)
{
    return targetRangeMin + ((targetRangeMax - targetRangeMin) * (sourceValue - sourceRangeMin)) /
        (sourceRangeMax - sourceRangeMin);
}

constexpr float maxdB =   6.0f;
constexpr float mindB = -84.0f;

inline float gainToDecibels (float gain)
{
    return gain > float() ? jmax (mindB, static_cast<float> (std::log10 (gain)) * 20.0f) : mindB;
}

inline float gainToProportion (float gain)
{
    return jmap (gain, -24.0f, 24.0f, 1.0f, 0.0f);
}

inline float spectrumGainToProportion (float gain)
{
    return jmap (gain, mindB, maxdB, 1.0f, 0.0f);
}


}
