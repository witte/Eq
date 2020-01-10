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

    for (auto& magnitudesBand : magnitudes)
        magnitudesBand.resize (frequencies.size(), 1.0);

    magnitudesOut.resize (frequencies.size(), 1.0);

    avgInput.clear();
    avgOutput.clear();

    fftPoints.resize (processor.fftSize);

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

float SpectrumAnalyzer::getFftPointLevel (const float* buffer, const fftPoint& point)
{
    float level = 0.0f;

    for (int i = point.firstBinIndex; i <= point.lastBinIndex; ++i)
    {
        if (buffer[i] > level) level = buffer[i];
    }

    return Decibels::gainToDecibels (level, mindB);
}

void SpectrumAnalyzer::paint (Graphics& g)
{
    const auto bounds = getLocalBounds().toFloat();
    const auto width  = bounds.getWidth();
    const auto height = bounds.getHeight();

    auto baseColor = Colour {uint8 (1), uint8 (28), uint8 (39)};
    g.fillAll (baseColor);

    g.setColour (baseColor.brighter (0.028f));
    g.drawHorizontalLine (int (height * 0.5f), 0, width);

    for (auto& bandGain : bandGains)
    {
        int pos = roundToInt (jmap (bandGain, -26.0f, 26.0f, height, 0.0f));
        g.drawHorizontalLine (pos, 0, width);
    }

    for (auto& freq : freqsB)
    {
        int pos = roundToInt (getPositionForFrequency (freq) * width);
        g.drawVerticalLine (pos, 0, height);
    }


    g.setColour (baseColor.brighter (0.08f));
    for (auto& freq : freqsA)
    {
        int pos = roundToInt (getPositionForFrequency (freq) * width);
        g.drawVerticalLine (pos, 0, height);
    }

    const auto* fftDataInput  = avgInput.getReadPointer (0);
    const auto* fftDataOutput = avgOutput.getReadPointer (0);

    {
        ScopedLock lockedForReading (pathCreationLock);

        inP.clear();
        outP.clear();

        {
            fftPoint& point = fftPoints[0];
            int yy = jmap (getFftPointLevel (fftDataInput, point), mindB, maxdB, height, 0.0f) + 0.5f;

            inP.startNewSubPath  (point.x, yy);
            outP.startNewSubPath (point.x, yy);
        }

        for (int i = 0; i < fftPointsSize; ++i)
        {
            fftPoint& point = fftPoints[i];
            int yy = jmap (getFftPointLevel (fftDataInput, point), mindB, maxdB, height, 0.0f) + 0.5f;

            inP.lineTo  (point.x, yy);
            outP.lineTo (point.x, yy);
        }

        for (int i = fftPointsSize - 1; i >= 0; --i)
        {
            fftPoint& point = fftPoints[i];
            int yy = jmap (getFftPointLevel (fftDataOutput, point), mindB, maxdB, height, 0.0f) + 0.5f;

            outP.lineTo (point.x, yy);
        }

        outP.closeSubPath();

        inP.lineTo (width, height);
        inP.lineTo (0.0f, height);
        inP.closeSubPath();

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

    if (hoveringBand >= 0)
    {
        g.setColour (Colours::rebeccapurple.withAlpha (0.5f));
        g.fillEllipse (plotAreas[hoveringBand]);
    }
}

void SpectrumAnalyzer::resized()
{
    auto widthFactor = getLocalBounds().getWidth() / 10.0f;
    auto sampleRate = float (processor.getSampleRate());
    auto fftSize = fftInput.getSize();

    fftPointsSize = 0;
    int lastX = 0;
    fftPoints[0].firstBinIndex = 0;

    int i = 0;
    while (i < processor.fftSize)
    {
        fftPoint& point = fftPoints [fftPointsSize];
        point.firstBinIndex = i;
        point.x = lastX;
        
        int x = lastX;
        while ((x <= lastX) && (i < processor.fftSize))
        {
            ++i;

            auto pos = std::log ( ((sampleRate * i) / fftSize) / 20.f) / std::log (2.0f);
            x = (pos > 0.0f)? (widthFactor * pos) + 0.5f : 0;
        }
        
        point.lastBinIndex = i - 1;

        ++fftPointsSize;
        lastX = x;
    }
    
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
        avgOutput.copyFrom (avgOutputPtr, 0, fftBufferOutput.getReadPointer (0),      avgOutput.getNumSamples(), 1.0f / (avgOutput.getNumSamples() * (avgOutput.getNumChannels() - 1)));
        avgOutput.addFrom  (0,            0, avgOutput.getReadPointer (avgOutputPtr), avgOutput.getNumSamples());

        if (++avgOutputPtr == avgOutput.getNumChannels()) avgOutputPtr = 1;
    }
}

void SpectrumAnalyzer::drawFrequencyCurve()
{
    const auto bounds = getLocalBounds().toFloat().reduced (2.0f);
    const auto x = bounds.getX();
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

        float bandX = x + (getPositionForFrequency (band.prmFreq->load()) * width);
        float bandY = jmap (band.prmGain->load(), -26.0f, 26.0f, height, 0.0f);

        plotAreas [i] = {bandX - 12.0f, bandY - 12.0f, 24.0f, 24.0f};
        bandsPositionsPath.addEllipse (bandX - 2.5f, bandY - 2.5f, 5.0f, 5.0f);

        FloatVectorOperations::multiply (magnitudesOut.data(), magnitudesBand.data(), int (magnitudesOut.size()));
    }

    float outputGain = *processor.prmOutputGain;
    frequencyCurvePath.clear();
    frequencyCurvePath.startNewSubPath (x,
            jmap (float (Decibels::gainToDecibels (magnitudesOut [0]) + outputGain), -26.0f, 26.0f, height, 0.0f));

    for (size_t i = 1; i < frequencies.size(); ++i)
    {
        float xx = x + (getPositionForFrequency (frequencies [i]) * width);
        float gain = Decibels::gainToDecibels (magnitudesOut [i]) + outputGain;
        float yy = jmap (gain, -26.0f, 26.0f, height, 0.0f);

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

    if (processor.frequenciesCurveChanged.load())
    {
        drawFrequencyCurve();
        processor.frequenciesCurveChanged.store (false);
        repaint();
    }
}

}
