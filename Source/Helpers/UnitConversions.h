#pragma once
#include <cmath>

namespace witte
{

namespace units
{

static float proportionToFreq (float proportion)
{
    return 20.0f * std::pow (2.0f, proportion * 10.0f);
}

static float freqToProportion (float freq)
{
    return (std::log (freq / 20.0f) / std::log (2.0f)) / 10.0f;
}

template <typename Type>
constexpr Type jmax (Type a, Type b) { return a < b ? b : a; }


template <typename Type>
static Type jmap (Type sourceValue, Type sourceRangeMin, Type sourceRangeMax, Type targetRangeMin, Type targetRangeMax)
{
    return targetRangeMin + ((targetRangeMax - targetRangeMin) * (sourceValue - sourceRangeMin)) / (sourceRangeMax - sourceRangeMin);
}

template <typename Type>
static Type gainToDecibels (Type gain, Type minusInfinityDb = Type (-80.0f))
{
    return gain > Type() ? jmax (minusInfinityDb, static_cast<Type> (std::log10 (gain)) * Type (20.0))
                         : minusInfinityDb;
}

static float gainToProportion (float gain)
{
    return jmap (float (gain), -24.0f, 24.0f, 1.0f, 0.0f);
}

static constexpr float maxdB =  6.0f;
static constexpr float mindB = -84.0f;

static float spectrumGainToProportion (float gain)
{
    return jmap (gain, mindB, maxdB, 1.0f, 0.0f);
}

}
}
