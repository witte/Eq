#include "FrequencyCurve.h"
#include <juce_audio_processors/juce_audio_processors.h>
#include <BinaryData.h>
#include "../Helpers/ParameterHelpers.h"

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

    auto typefaceBold = Typeface::createSystemTypefaceFor (BinaryData::OpenSansCondensedBold_ttf, BinaryData::OpenSansCondensedBold_ttfSize);
    openSansBold = Font (typefaceBold);


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
    {
        ScopedLock lockedForReading (freqPathCreationLock);

        g.setColour (Colour {0xff4ea5ac});
        g.fillPath (frequencyCurvePath);
    }

    g.drawImageAt (foregroundImage, 0, 0);
}

void FrequencyCurve::resized()
{
    drawFrequencyCurve();
    drawForegroundImage();
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
    frequencyCurvePath.startNewSubPath (0.0f,
            jmap (float (Decibels::gainToDecibels (magnitudesOut [0]) + outputGain), -26.0f, 26.0f, height, 0.0f));

    for (size_t i = 1; i < frequencies.size(); ++i)
    {
        float xx = getPositionForFrequency (frequencies [i]) * width;
        float gain = Decibels::gainToDecibels (magnitudesOut [i]) + outputGain;
        float yy = jmap (gain, -26.0f, 26.0f, height, 0.0f);

        frequencyCurvePath.lineTo (xx, yy - 0.5f);
        frequenciesPoints[i] = {xx, yy + 0.5f};
    }

    for (size_t i = frequencies.size() - 1; i > 0; --i)
    {
        frequencyCurvePath.lineTo (frequenciesPoints [i]);
    }

    frequencyCurvePath.closeSubPath();
}

void FrequencyCurve::drawForegroundImage()
{
    const auto bounds = getLocalBounds().toFloat();
    const auto width  = bounds.getWidth();
    const auto height = bounds.getHeight();

    foregroundImage.clear (foregroundImage.getBounds());

    Graphics g {foregroundImage};
    g.setColour (baseColor.brighter (1.2f));

    for (auto& bandGain : bandGains)
    {
        int pos = roundToInt (jmap (bandGain, -26.0f, 26.0f, height, 0.0f));
        g.drawText (String {bandGain}, width - 44, pos - 14, 42, 28, Justification::centredRight);
    }

    for (auto& gain : gains)
    {
        int pos = roundToInt (jmap (gain + 6.0f, mindB, maxdB, height, 0.0f));
        g.drawText (String { std::abs (gain)}, 2, pos - 14, 42, 28, Justification::centredLeft);
    }

    g.setFont (openSansBold);
    g.setFont (15.0f);
    g.setColour (baseColor.brighter (2.8f));
    for (auto& freq : freqsA)
    {
        int pos = roundToInt (getPositionForFrequency (freq) * width);
        String str;
        if (freq >= 1000)
        {
            str << (freq / 1000);
            str << "k";
        }
        else
        {
            str << freq;
        }

        g.drawText (str, pos - 21, (height * 0.5f) - 14, 42, 28, Justification::centred);
    }
}

}
