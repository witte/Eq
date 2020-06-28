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

static float jmax (float a, float b) { return a < b ? b : a; }

static float jmap (float sourceValue, float sourceRangeMin, float sourceRangeMax, float targetRangeMin, float targetRangeMax)
{
    return targetRangeMin + ((targetRangeMax - targetRangeMin) * (sourceValue - sourceRangeMin)) / (sourceRangeMax - sourceRangeMin);
}

static constexpr float maxdB =   6.0f;
static constexpr float mindB = -84.0f;

static float gainToDecibels (float gain)
{
    return gain > float() ? jmax (mindB, static_cast<float> (std::log10 (gain)) * 20.0f) : mindB;
}

static float gainToProportion (float gain)
{
    return jmap (gain, -24.0f, 24.0f, 1.0f, 0.0f);
}

static float spectrumGainToProportion (float gain)
{
    return jmap (gain, mindB, maxdB, 1.0f, 0.0f);
}

}
}
