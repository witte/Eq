#include "FrequencyCurve.h"
#include "../Helpers/UnitConversions.h"


namespace witte
{

FrequencyCurve::FrequencyCurve (EqAudioProcessor& eqProcessor) :
    processor {eqProcessor}
{
    setPaintingIsUnclipped (true);

    std::unique_lock<std::mutex> lock(mutex);

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

    frequencyCurvePath.clear();
    frequencyCurvePath.preallocateSpace (8 + int (frequencies.size()) * 3);

    for (auto& band : eqProcessor.getBands())
    {
        processor.getVTS().addParameterListener(band.idOn,   this);
        processor.getVTS().addParameterListener(band.idType, this);
        processor.getVTS().addParameterListener(band.idFreq, this);
        processor.getVTS().addParameterListener(band.idGain, this);
        processor.getVTS().addParameterListener(band.idQ,    this);
    }

    processor.getVTS().addParameterListener(eqProcessor.idOutputGain, this);
}

FrequencyCurve::~FrequencyCurve()
{
    for (auto& band : processor.getBands())
    {
        processor.getVTS().removeParameterListener(band.idOn,   this);
        processor.getVTS().removeParameterListener(band.idType, this);
        processor.getVTS().removeParameterListener(band.idFreq, this);
        processor.getVTS().removeParameterListener(band.idGain, this);
        processor.getVTS().removeParameterListener(band.idQ,    this);
    }

    processor.getVTS().removeParameterListener(processor.idOutputGain, this);
}

void FrequencyCurve::paint (juce::Graphics &g)
{
    std::unique_lock<std::mutex> lock(mutex);

    g.setColour (juce::Colour {0xff4ea5ac});
    g.fillPath (frequencyCurvePath);
}

void FrequencyCurve::resized()
{
    drawFrequencyCurve();
}

void FrequencyCurve::parameterChanged(const juce::String&, float)
{
    drawFrequencyCurve();
    repaint();
}

void FrequencyCurve::drawFrequencyCurve()
{
    std::unique_lock<std::mutex> lock(mutex);

    const auto bounds = getLocalBounds().toFloat();
    const auto width  = bounds.getWidth();
    const auto height = bounds.getHeight();

    std::fill (magnitudesOut.begin(), magnitudesOut.end(), 1.0);

    for (int i = 0; i < 5; ++i)
    {
        auto& band = processor.getBands() [i];

        if (band.prmOn->load() <= 0.5f) continue;

        auto& magnitudesBand = magnitudes [i];
        band.processor.state->getMagnitudeForFrequencyArray (frequencies.data(),
                                                             magnitudesBand.data(),
                                                             frequencies.size(), processor.getSampleRate());

        juce::FloatVectorOperations::multiply (magnitudesOut.data(), magnitudesBand.data(), int (magnitudesOut.size()));
    }

    float outputGain = *processor.prmOutputGain;
    frequencyCurvePath.clear();
    frequencyCurvePath.startNewSubPath (0.0f, units::gainToProportion (juce::Decibels::decibelsToGain (magnitudesOut [0]) + outputGain) * float (height));

    for (size_t i = 1; i < frequencies.size(); ++i)
    {
        float xx = units::freqToProportion (frequencies [i]) * width;
        float gain = juce::Decibels::gainToDecibels (magnitudesOut [i]) + outputGain;
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
