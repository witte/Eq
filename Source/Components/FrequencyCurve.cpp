#include "FrequencyCurve.h"
#include <juce_audio_processors/juce_audio_processors.h>
#include "../Helpers/UnitConversions.h"

namespace witte
{

FrequencyCurve::FrequencyCurve (EqAudioProcessor& eqProcessor, std::initializer_list<RangedAudioParameter*> parameters) :
    processor {eqProcessor}
{
    setPaintingIsUnclipped (true);

    for (auto& parameter : parameters)
    {
        attachments.push_back (std::make_unique<ParameterAttachment>(*parameter, prmChangedCallback));
    }

    frequencies.resize (600);
    for (size_t i = 0; i < frequencies.size(); ++i)
    {
        frequencies [i] = 20.0 * std::pow (2.0, i / (frequencies.size() * 0.1));
    }

    frequenciesPoints.resize (frequencies.size());

    for (auto& magnitudesBand : magnitudes)
    {
        magnitudesBand.resize (frequencies.size(), 1.0);
    }

    magnitudesOut.resize (frequencies.size(), 1.0);

    ScopedLock lockedForWriting (freqPathCreationLock);
    frequencyCurvePath.clear();
    frequencyCurvePath.preallocateSpace (8 + int (frequencies.size()) * 3);
}

FrequencyCurve::~FrequencyCurve()
{
}

void FrequencyCurve::paint (Graphics& g)
{
    ScopedLock lockedForReading (freqPathCreationLock);

    g.setColour (Colour {0xff4ea5ac});
    g.fillPath (frequencyCurvePath);
}

void FrequencyCurve::resized()
{
    drawFrequencyCurve();
}

void FrequencyCurve::drawFrequencyCurve()
{
    const auto bounds = getLocalBounds().toFloat();
    const auto width  = bounds.getWidth();
    const auto height = bounds.getHeight();

    std::fill (magnitudesOut.begin(), magnitudesOut.end(), 1.0);

    ScopedLock lockedForWriting (freqPathCreationLock);

    for (int i = 0; i < 5; ++i)
    {
        auto& band = processor.getBands() [i];

        if (band.prmOn->load() <= 0.5f) continue;

        auto& magnitudesBand = magnitudes [i];
        band.processor.state->getMagnitudeForFrequencyArray (frequencies.data(),
                                                             magnitudesBand.data(),
                                                             frequencies.size(), processor.getSampleRate());

        FloatVectorOperations::multiply (magnitudesOut.data(), magnitudesBand.data(), int (magnitudesOut.size()));
    }

    float outputGain = *processor.prmOutputGain;
    frequencyCurvePath.clear();
    frequencyCurvePath.startNewSubPath (0.0f, units::gainToProportion (Decibels::decibelsToGain (magnitudesOut [0]) + outputGain) * float (height));

    for (size_t i = 1; i < frequencies.size(); ++i)
    {
        float xx = units::freqToProportion (frequencies [i]) * width;
        float gain = Decibels::gainToDecibels (magnitudesOut [i]) + outputGain;
        float yy = units::gainToProportion (gain) * height;

        frequencyCurvePath.lineTo (xx, yy - 0.5f);
        frequenciesPoints[i] = {xx, yy + 0.5f};
    }

    for (size_t i = frequencies.size() - 1; i > 0; --i)
    {
        frequencyCurvePath.lineTo (frequenciesPoints [i]);
    }

    frequencyCurvePath.closeSubPath();
}

}
