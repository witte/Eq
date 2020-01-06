#include "SpectrumAnalyzer.h"

namespace witte
{

static String getPrmName (int bandNumber, const String& paramName)
{
    String str (bandNumber);
    str << paramName;
    return  str;
}

SpectrumAnalyzer::SpectrumAnalyzer (EqAudioProcessor& eqProcessor, AudioProcessorValueTreeState& _tree) :
    processor {eqProcessor},
    tree {_tree}
{
    setOpaque (true);

    auto typefaceBold = Typeface::createSystemTypefaceFor (BinaryData::OpenSansCondensedBold_ttf, BinaryData::OpenSansCondensedBold_ttfSize);
    openSansBold = Font (typefaceBold);


    frequencies.resize (600);
    for (size_t i = 0; i < frequencies.size(); ++i)
    {
        frequencies [i] = 20.0 * std::pow (2.0, i / (frequencies.size() * 0.1));
    }
    magnitudesBand1.resize (frequencies.size(), 1.0);
    magnitudesBand2.resize (frequencies.size(), 1.0);
    magnitudesBand3.resize (frequencies.size(), 1.0);
    magnitudesBand4.resize (frequencies.size(), 1.0);
    magnitudesBand5.resize (frequencies.size(), 1.0);
    magnitudesOut.resize (frequencies.size(), 1.0);

    avgInput.clear();
    avgOutput.clear();

    {
        ScopedLock lockedForWriting (pathCreationLock);
        inP.preallocateSpace  (processor.fftSize * 2);
        outP.preallocateSpace (processor.fftSize * 2);
    }
    {
        ScopedLock lockedForWriting (freqPathCreationLock);
        frequencyCurvePath.clear();
        frequencyCurvePath.preallocateSpace (8 + int (frequencies.size()) * 3);
    }

    startTimerHz (30);
}

SpectrumAnalyzer::~SpectrumAnalyzer()
{
}

void SpectrumAnalyzer::paint (Graphics& g)
{
    const auto bounds = getLocalBounds().toFloat().reduced (2.0f);
    const auto x = bounds.getX();
    const auto y = bounds.getY();
    const auto width  = bounds.getWidth();
    const auto height = bounds.getHeight();
    const auto bottom = bounds.getBottom();

    auto baseColor = Colour {uint8 (1), uint8 (28), uint8 (39)};
    g.fillAll (baseColor);

    g.setColour (baseColor.brighter (0.028f));

    auto gains = { -12.0f, -24.0f, -36.0f, -48.0f, -60.0f, -72.0f, -84.0f };
    auto bandGains = { 24.0f, 12.0f, -12.0f, -24.0f };

    auto freqsA = { 100.0f, 1000.0f, 10000.0f };
    auto freqsB = {  20.0f,   30.0f,   40.0f,   50.0f,   60.0f,   70.0f,   80.0f,   90.0f,
                    200.0f,  300.0f,  400.0f,  500.0f,  600.0f,  700.0f,  800.0f,  900.0f,
                   2000.0f, 3000.0f, 4000.0f, 5000.0f, 6000.0f, 7000.0f, 8000.0f, 9000.0f,
                  20000.0f };

    g.drawHorizontalLine (int (height * 0.5f), x, x + width);

    for (auto& bandGain : bandGains)
    {
        int pos = roundToInt (jmap (bandGain, -26.0f, 26.0f, bottom, y));
        g.drawHorizontalLine (pos, x, x + width);
    }

    for (auto& freq : freqsB)
    {
        int pos = x + (roundToInt (getPositionForFrequency (freq) * width));
        g.drawVerticalLine (pos, y, bottom);
    }


    g.setColour (baseColor.brighter (0.08f));
    for (auto& freq : freqsA)
    {
        int pos = x + (roundToInt (getPositionForFrequency (freq) * width));
        g.drawVerticalLine (pos, y, bottom);
    }

    const auto* fftDataInput  = avgInput.getReadPointer (0);
    const auto* fftDataOutput = avgOutput.getReadPointer (0);
    const auto  factor  = width / 10.0f;

    {
        ScopedLock lockedForReading (pathCreationLock);

        inP.clear();
        outP.clear();
        inP.startNewSubPath (x, bottom);
        outP.startNewSubPath (x, jmap (Decibels::gainToDecibels (fftDataInput [0], mindB), mindB, maxdB, bottom, y));

        for (int i = 0; i < avgInput.getNumSamples(); ++i)
        {
            float freqX = x + (factor * indexToX (i));
            float freqLevel = jmap (Decibels::gainToDecibels (fftDataInput [i], mindB), mindB, maxdB, bottom, y);

            inP.lineTo  (freqX, roundToInt (freqLevel));
            outP.lineTo (freqX, roundToInt (freqLevel));
        }

        for (int i = avgInput.getNumSamples() - 1; i >= 0; --i)
        {
            float freqX = x + (factor * indexToX (i));
            float freqLevel = jmap (Decibels::gainToDecibels (fftDataOutput [i], mindB), mindB, maxdB, bottom, y);

            outP.lineTo (freqX, roundToInt (freqLevel));
        }
        outP.closeSubPath();
        inP.lineTo (width, bottom);
        inP.lineTo (x, bottom);

        g.setColour (Colours::yellowgreen.withAlpha (0.42f));
        g.fillPath (outP);

        g.setColour (baseColor.brighter (0.18f).withAlpha (uint8 (182)));
        g.fillPath (inP);
    }

    {
        ScopedLock lockedForReading (freqPathCreationLock);
        
        g.setColour (Colour {0xff4ea5ac});
        g.strokePath (frequencyCurvePath, frequencyCurve);
        g.fillPath (bandsPositionsPath);
    }


    g.setColour (baseColor.brighter (1.2f));
    for (auto& bandGain : bandGains)
    {
        int pos = roundToInt (jmap (bandGain, -26.0f, 26.0f, bottom, y));
        g.drawText (String {bandGain}, x + width - 44, pos - 14, 42, 28, Justification::centredRight);
    }

    for (auto& gain : gains)
    {
        int pos = roundToInt (jmap (gain + 6.0f, mindB, maxdB, bottom, y));
        g.drawText (String { std::abs (gain)}, x + 2, pos - 14, 42, 28, Justification::centredLeft);
    }

    g.setFont (openSansBold);
    g.setFont (15.0f);
    g.setColour (baseColor.brighter (2.8f));
    for (auto& freq : freqsA)
    {
        int pos = roundToInt (getPositionForFrequency (freq) * (x + width));
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
        g.drawText (str, pos - 21, y + (height * 0.5f) - 14, 42, 28, Justification::centred);
    }

    if (hoveringBand >= 0)
    {
        g.setColour (Colours::rebeccapurple.withAlpha (0.5f));
        g.fillEllipse (plotAreas[hoveringBand]);
    }
}

void SpectrumAnalyzer::resized()
{
    drawFrequencyCurve();
}

void SpectrumAnalyzer::mouseDown (const MouseEvent& event)
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

void SpectrumAnalyzer::mouseMove (const MouseEvent& event)
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

void SpectrumAnalyzer::mouseDrag (const MouseEvent& event)
{
    if (movingBand < 0) return;

    const auto bounds = getLocalBounds().toFloat();
    const auto x = bounds.getX();
    const auto y = bounds.getY();
    const auto width  = bounds.getWidth();
    const auto bottom = bounds.getBottom();

    float freqForPos = getFrequencyForPosition ((event.position.getX() - 2.0f) / (x + width));
    freqForPos = std::clamp (freqForPos, 20.0f, 20000.0f);

    String str = getPrmName (movingBand + 1, "Freq");
    float newValue = tree.getParameterRange (str).convertTo0to1 (freqForPos);
    tree.getParameter (str)->setValueNotifyingHost (newValue);


    float gainForPos = jmap (event.position.getY(), bottom, y, -26.0f, 26.0f) - *processor.prmOutputGain;
    gainForPos = std::clamp (gainForPos, -24.0f, 24.0f);
    str = getPrmName (movingBand + 1, "Gain");
    newValue = tree.getParameterRange (str).convertTo0to1 (gainForPos);
    tree.getParameter (str)->setValueNotifyingHost (newValue);
}

void SpectrumAnalyzer::mouseUp (const MouseEvent&)
{
    movingBand = -1;
    hoveringBand = -1;
}

void SpectrumAnalyzer::drawNextFrame()
{
    while (processor.abstractFifoInput.getNumReady() >= fftInput.getSize())
    {
        fftBufferInput.clear();

        int start1, block1, start2, block2;
        processor.abstractFifoInput.prepareToRead (fftInput.getSize(), start1, block1, start2, block2);

        if (block1 > 0) fftBufferInput.copyFrom (0,      0, processor.audioFifoInput.getReadPointer (0, start1), block1);
        if (block2 > 0) fftBufferInput.copyFrom (0, block1, processor.audioFifoInput.getReadPointer (0, start2), block2);

        processor.abstractFifoInput.finishedRead ((block1 + block2) / 2);

        hannWindow.multiplyWithWindowingTable (fftBufferInput.getWritePointer (0), size_t (fftInput.getSize()));
        fftInput.performFrequencyOnlyForwardTransform (fftBufferInput.getWritePointer (0));

        ScopedLock lockedForWriting (pathCreationLock);
        avgInput.addFrom  (0,           0, avgInput.getReadPointer (avgInputPtr), avgInput.getNumSamples(), -1.0f);
        avgInput.copyFrom (avgInputPtr, 0, fftBufferInput.getReadPointer (0),     avgInput.getNumSamples(), 1.0f / (avgInput.getNumSamples() * (avgInput.getNumChannels() - 1)));
        avgInput.addFrom  (0,           0, avgInput.getReadPointer (avgInputPtr), avgInput.getNumSamples());

        if (++avgInputPtr == avgInput.getNumChannels()) avgInputPtr = 1;
    }

    while (processor.abstractFifoOutput.getNumReady() >= fftOutput.getSize())
    {
        fftBufferOutput.clear();

        int start1, block1, start2, block2;
        processor.abstractFifoOutput.prepareToRead (fftOutput.getSize(), start1, block1, start2, block2);

        if (block1 > 0) fftBufferOutput.copyFrom (0,      0, processor.audioFifoOutput.getReadPointer (0, start1), block1);
        if (block2 > 0) fftBufferOutput.copyFrom (0, block1, processor.audioFifoOutput.getReadPointer (0, start2), block2);

        processor.abstractFifoOutput.finishedRead ((block1 + block2) / 2);

        hannWindow.multiplyWithWindowingTable (fftBufferOutput.getWritePointer (0), size_t (fftOutput.getSize()));
        fftOutput.performFrequencyOnlyForwardTransform (fftBufferOutput.getWritePointer (0));

        ScopedLock lockedForWriting (pathCreationLock);
        avgOutput.addFrom  (0,            0, avgOutput.getReadPointer (avgOutputPtr), avgOutput.getNumSamples(), -1.0f);
        avgOutput.copyFrom (avgOutputPtr, 0, fftBufferOutput.getReadPointer (0), avgOutput.getNumSamples(), 1.0f / (avgOutput.getNumSamples() * (avgOutput.getNumChannels() - 1)));
        avgOutput.addFrom  (0,            0, avgOutput.getReadPointer (avgOutputPtr), avgOutput.getNumSamples());

        if (++avgOutputPtr == avgOutput.getNumChannels()) avgOutputPtr = 1;
    }
}

void SpectrumAnalyzer::drawFrequencyCurve()
{
    const auto bounds = getLocalBounds().toFloat().reduced (2.0f);
    const auto x = bounds.getX();
    const auto y = bounds.getY();
    const auto width  = bounds.getWidth();
    const auto bottom = bounds.getBottom();

    std::fill (magnitudesOut.begin(), magnitudesOut.end(), 1.0);

    ScopedLock lockedForWriting (freqPathCreationLock);
    bandsPositionsPath.clear();

    float outputGain = *processor.prmOutputGain;

    for (int i = 1; i <= 5; ++i)
    {
        EqAudioProcessor::Band& band = (i == 1)? processor.band1 :
                                       (i == 2)? processor.band2 :
                                       (i == 3)? processor.band3 :
                                       (i == 4)? processor.band4 : processor.band5;

        if (!band.active) continue;

        auto& magnitudes = (i == 1)? magnitudesBand1 :
                           (i == 2)? magnitudesBand2 :
                           (i == 3)? magnitudesBand3 :
                           (i == 4)? magnitudesBand4 : magnitudesBand5;

        band.processor.state->getMagnitudeForFrequencyArray (frequencies.data(),
                                                             magnitudes.data(),
                                                             frequencies.size(), processor.getSampleRate());

        float bandX = x + (getPositionForFrequency (*band.prmFreq) * width);
        float bandY = jmap (*band.prmGain + outputGain, -26.0f, 26.0f, bottom, y);

        plotAreas[i-1] = {bandX - 12.0f, bandY - 12.0f, 24.0f, 24.0f};
        bandsPositionsPath.addEllipse (bandX - 2.5f, bandY - 2.5f, 5.0f, 5.0f);

        FloatVectorOperations::multiply (magnitudesOut.data(), magnitudes.data(), int (magnitudesOut.size()));
    }

    frequencyCurvePath.clear();
    frequencyCurvePath.startNewSubPath (x,
            jmap (float (Decibels::gainToDecibels (magnitudesOut [0]) + outputGain), -26.0f, 26.0f, bottom, y));

    for (size_t i = 1; i < frequencies.size(); ++i)
    {
        float xx = x + (getPositionForFrequency (frequencies[i]) * width);
        float gain = Decibels::gainToDecibels (magnitudesOut [i]) + outputGain;
        float yy = jmap (gain, -26.0f, 26.0f, bottom, y);

        frequencyCurvePath.lineTo (xx, yy);
    }
}

void SpectrumAnalyzer::timerCallback()
{
    if (processor.nextFFTBlockReady.load())
    {
        drawNextFrame();
        processor.nextFFTBlockReady.store (false);
        repaint();
    }

    if (processor.frequenciesCurveChanged)
    {
        drawFrequencyCurve();
        processor.frequenciesCurveChanged.store (false);
        repaint();
    }
}

}
