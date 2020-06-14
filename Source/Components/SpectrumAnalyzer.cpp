#include "SpectrumAnalyzer.h"
#include <juce_audio_processors/juce_audio_processors.h>

namespace witte
{

SpectrumAnalyzer::SpectrumAnalyzer (EqAudioProcessor& eqProcessor) : processor {eqProcessor}
{
    setOpaque (true);

    avgInput.clear();
    avgOutput.clear();

    fftPoints.resize (processor.fftSize);

    {
        ScopedLock lockedForWriting (pathCreationLock);
        inP.preallocateSpace  (processor.fftSize * 2);
        outP.preallocateSpace (processor.fftSize * 2);
    }

    startTimerHz (30);
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
    g.drawImageAt (markers.getImage(), 0, 0);

    const auto bounds = getLocalBounds().toFloat();
    const auto width  = bounds.getWidth();
    const auto height = bounds.getHeight();

    const auto* fftDataInput  = avgInput.getReadPointer (0);
    const auto* fftDataOutput = avgOutput.getReadPointer (0);

    ScopedLock lockedForReading (pathCreationLock);
    inP.clear();
    outP.clear();

    {
        fftPoint& point = fftPoints[0];
        int y = jmap (getFftPointLevel (fftDataInput, point), mindB, maxdB, height, 0.0f) + 0.5f;

        inP.startNewSubPath  (point.x, y);
        outP.startNewSubPath (point.x, y);
    }

    for (int i = 0; i < fftPointsSize; ++i)
    {
        fftPoint& point = fftPoints[i];
        int y = jmap (getFftPointLevel (fftDataInput, point), mindB, maxdB, height, 0.0f) + 0.5f;

        inP.lineTo  (point.x, y);
        outP.lineTo (point.x, y);
    }

    for (int i = fftPointsSize - 1; i >= 0; --i)
    {
        fftPoint& point = fftPoints[i];
        int y = jmap (getFftPointLevel (fftDataOutput, point), mindB, maxdB, height, 0.0f) + 0.5f;

        outP.lineTo (point.x, y);
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

void SpectrumAnalyzer::resized()
{
    const auto bounds = getLocalBounds();
    auto widthFactor = bounds.getWidth() / 10.0f;
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

    markers.setSize(bounds.getWidth(), bounds.getHeight());
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

void SpectrumAnalyzer::timerCallback()
{
    if (!processor.nextFFTBlockReady.load()) return;

    drawNextFrame();
    processor.nextFFTBlockReady.store (false);
    repaint();
}

}
