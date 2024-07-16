#include "SpectrumAnalyzer.h"
#include <juce_audio_processors/juce_audio_processors.h>


namespace witte
{


SpectrumAnalyzer::SpectrumAnalyzer (EqAudioProcessor& eqProcessor) : processor {eqProcessor}
{
    setPaintingIsUnclipped (true);

    avgInput.clear();
    avgOutput.clear();

    fftPoints.resize (EqAudioProcessor::fftSize);

    {
        juce::ScopedLock lockedForWriting (pathCreationLock);
        inP.preallocateSpace  (EqAudioProcessor::fftSize * 2);
        outP.preallocateSpace (EqAudioProcessor::fftSize * 2);
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

    return juce::Decibels::gainToDecibels (level, mindB);
}

void SpectrumAnalyzer::paint (juce::Graphics& g)
{
    if (resizeDebounceInFrames > 0)
    {
        resizeDebounceInFrames--;
        return;
    }

    const auto bounds = getLocalBounds().toFloat();
    const auto width  = bounds.getWidth();
    const auto height = bounds.getHeight();

    const auto* fftDataInput  = avgInput.getReadPointer (0);
    const auto* fftDataOutput = avgOutput.getReadPointer (0);

    juce::ScopedLock lockedForReading (pathCreationLock);
    inP.clear();
    outP.clear();

    {
        const fftPoint& point = fftPoints[0];
        const float y = juce::jmap (getFftPointLevel (fftDataInput, point),
            mindB, maxdB, height, 0.0f) + 0.5f;

        inP.startNewSubPath  (static_cast<float> (point.x), y);
        outP.startNewSubPath (static_cast<float> (point.x), y);
    }

    for (int i = 0; i < fftPointsSize; ++i)
    {
        fftPoint& point = fftPoints[i];
        const float y = juce::jmap (getFftPointLevel (fftDataInput, point),
            mindB, maxdB, height, 0.0f) + 0.5f;

        inP.lineTo  (static_cast<float> (point.x), y);
        outP.lineTo (static_cast<float> (point.x), y);
    }

    for (int i = fftPointsSize - 1; i >= 0; --i)
    {
        fftPoint& point = fftPoints[i];
        const float y = juce::jmap (getFftPointLevel (fftDataOutput, point),
            mindB, maxdB, height, 0.0f) + 0.5f;

        outP.lineTo (static_cast<float> (point.x), y);
    }

    outP.closeSubPath();

    inP.lineTo (width, height);
    inP.lineTo (0.0f, height);
    inP.closeSubPath();

    g.setColour (juce::Colour {0x6b9acd32});
    g.fillPath (outP);

    g.setColour (baseColor.brighter (0.18f).withAlpha (static_cast<juce::uint8> (182)));
    g.fillPath (inP);
}

void SpectrumAnalyzer::resized()
{
    // When the component is being resized the firstBinIndex'es and pointLastBinIndex'es
    // get updated: better to wait until resizing is done so we don't recalculate
    // them needlessly
    static constexpr int framesToWaitBeforePaintingAfterResizing = 5;
    resizeDebounceInFrames = framesToWaitBeforePaintingAfterResizing;

    const auto width = getLocalBounds().toFloat().getWidth();
    const auto widthFactor = width / 10.0f;
    const auto sampleRate = static_cast<float> (processor.getSampleRate());
    const auto fftSize = static_cast<float> (fftInput.getSize());

    fftPointsSize = 0;
    int lastX = 0;
    fftPoints[0].firstBinIndex = 0;

    int i = 0;
    while (i < EqAudioProcessor::fftSize)
    {
        auto&[pointFirstBinIndex, pointLastBinIndex, pointX] = fftPoints [fftPointsSize];

        pointFirstBinIndex = i;
        pointX = lastX;
        
        int x = lastX;
        while (x <= lastX && i < EqAudioProcessor::fftSize)
        {
            ++i;

            const auto pos = std::log ( (sampleRate * static_cast<float> (i)) / fftSize / 20.0f) / std::log (2.0f);
            x = juce::roundToInt ( (pos > 0.0f)? (widthFactor * pos) + 0.5f : 0.0f);
        }
        
        pointLastBinIndex = i - 1;

        ++fftPointsSize;
        lastX = x;
    }
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

        hannWindow.multiplyWithWindowingTable (fftBufferInput.getWritePointer (0), static_cast<size_t>(fftInput.getSize()));
        fftInput.performFrequencyOnlyForwardTransform (fftBufferInput.getWritePointer (0));

        juce::ScopedLock lockedForWriting (pathCreationLock);
        avgInput.addFrom  (0,           0, avgInput.getReadPointer (avgInputPtr), avgInput.getNumSamples(), -1.0f);
        avgInput.copyFrom (avgInputPtr, 0, fftBufferInput.getReadPointer (0), avgInput.getNumSamples(), 1.0f / (static_cast<float> (avgInput.getNumSamples()) * (static_cast<float> (avgInput.getNumChannels()) - 1)));
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

        hannWindow.multiplyWithWindowingTable (fftBufferOutput.getWritePointer (0), static_cast<size_t>(fftOutput.getSize()));
        fftOutput.performFrequencyOnlyForwardTransform (fftBufferOutput.getWritePointer (0));

        juce::ScopedLock lockedForWriting (pathCreationLock);
        avgOutput.addFrom  (0,            0, avgOutput.getReadPointer (avgOutputPtr), avgOutput.getNumSamples(), -1.0f);
        avgOutput.copyFrom (avgOutputPtr, 0, fftBufferOutput.getReadPointer (0),      avgOutput.getNumSamples(), 1.0f / (static_cast<float> (avgOutput.getNumSamples()) * (static_cast<float> (avgOutput.getNumChannels()) - 1)));
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
