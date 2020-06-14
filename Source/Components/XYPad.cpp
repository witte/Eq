#include "XYPad.h"
#include <juce_audio_processors/juce_audio_processors.h>
#include <BinaryData.h>

namespace witte
{

static String getPrmName (int bandNumber, const String& paramName)
{
    String str (bandNumber);
    str << paramName;
    return  str;
}

XYPad::XYPad (EqAudioProcessor& eqProcessor, AudioProcessorValueTreeState& _tree) :
    processor {eqProcessor},
    tree {_tree}
{
    setPaintingIsUnclipped(true);

    auto typefaceBold = Typeface::createSystemTypefaceFor (BinaryData::OpenSansCondensedBold_ttf, BinaryData::OpenSansCondensedBold_ttfSize);
    openSansBold = Font (typefaceBold);


    frequencies.resize (600);
    for (size_t i = 0; i < frequencies.size(); ++i)
    {
        frequencies [i] = 20.0 * std::pow (2.0, i / (frequencies.size() * 0.1));
    }


    frequenciesPoints.resize (frequencies.size());

    for (auto& magnitudesBand : magnitudes)
        magnitudesBand.resize (frequencies.size(), 1.0);

    magnitudesOut.resize (frequencies.size(), 1.0);

    {
        ScopedLock lockedForWriting (freqPathCreationLock);
        frequencyCurvePath.clear();
        frequencyCurvePath.preallocateSpace (8 + int (frequencies.size()) * 3);
    }
}

XYPad::~XYPad()
{
}

void XYPad::paint (Graphics& g)
{
    {
        ScopedLock lockedForReading (freqPathCreationLock);

        g.setColour (Colour {0xff4ea5ac});
        g.fillPath (frequencyCurvePath);
        g.fillPath (bandsPositionsPath);
    }

    g.drawImageAt (foregroundImage, 0, 0);

    if (hoveringBand >= 0)
    {
        g.setColour (Colours::rebeccapurple.withAlpha (0.5f));
        g.fillEllipse (plotAreas[hoveringBand]);
    }
}

void XYPad::resized()
{
    drawFrequencyCurve();
    drawForegroundImage();
}

void XYPad::mouseDown (const MouseEvent& event)
{
    for (int i = 0; i < 5; ++i)
    {
        if (plotAreas[i].contains (event.getPosition().toFloat().translated (-2.0f, -2.0f)))
        {
            movingBand = i;
            break;
        }
    }
}

void XYPad::mouseMove (const MouseEvent& event)
{
    for (int i = 0; i < 5; ++i)
    {
        if (plotAreas[i].contains (event.getPosition().toFloat().translated (-2.0f, -2.0f)))
        {
            hoveringBand = i;
            repaint();
            return;
        }
    }

    hoveringBand = -1;
    repaint();
}

void XYPad::mouseDrag (const MouseEvent& event)
{
    if (movingBand < 0) return;

    const auto bounds = getLocalBounds().toFloat();
    const auto x = bounds.getX();
    const auto width  = bounds.getWidth();
    const auto height = bounds.getHeight();

    float freqForPos = getFrequencyForPosition ((event.position.getX() - 2.0f) / (x + width));
    freqForPos = std::clamp (freqForPos, 20.0f, 20000.0f);

    String str = getPrmName (movingBand + 1, "Freq");
    float newValue = tree.getParameterRange (str).convertTo0to1 (freqForPos);
    tree.getParameter (str)->setValueNotifyingHost (newValue);


    float gainForPos = jmap (event.position.getY(), height, 0.0f, -26.0f, 26.0f);
    gainForPos = std::clamp (gainForPos, -24.0f, 24.0f);
    str = getPrmName (movingBand + 1, "Gain");
    newValue = tree.getParameterRange (str).convertTo0to1 (gainForPos);
    tree.getParameter (str)->setValueNotifyingHost (newValue);

    drawFrequencyCurve();
    repaint();
}

void XYPad::mouseUp (const MouseEvent&)
{
    movingBand = -1;
    hoveringBand = -1;
}

void XYPad::drawFrequencyCurve()
{
    const auto bounds = getLocalBounds().toFloat();
    const auto width  = bounds.getWidth();
    const auto height = bounds.getHeight();

    std::fill (magnitudesOut.begin(), magnitudesOut.end(), 1.0);

    ScopedLock lockedForWriting (freqPathCreationLock);
    bandsPositionsPath.clear();

    for (int i = 0; i < 5; ++i)
    {
        auto& band = processor.getBands() [i];

        if (band.prmOn->load() <= 0.5f) continue;

        auto& magnitudesBand = magnitudes [i];
        band.processor.state->getMagnitudeForFrequencyArray (frequencies.data(),
                                                             magnitudesBand.data(),
                                                             frequencies.size(), processor.getSampleRate());

        float bandX = getPositionForFrequency (band.prmFreq->load()) * width;
        float bandY = jmap (band.prmGain->load(), -26.0f, 26.0f, height, 0.0f);

        plotAreas [i] = {bandX - 12.0f, bandY - 12.0f, 24.0f, 24.0f};
        bandsPositionsPath.addEllipse (bandX - 2.5f, bandY - 2.5f, 5.0f, 5.0f);

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

void XYPad::drawForegroundImage()
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
