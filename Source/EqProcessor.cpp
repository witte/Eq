#include "EqProcessor.h"
#include "EqEditor.h"

namespace IDs
{
    const String editor {"editor"};
    const String sizeX  {"size-x"};
    const String sizeY  {"size-y"};
}

EqAudioProcessor::EqAudioProcessor() : parameters (*this, &undoManager, "Eq", std::move (prmLayout)),
    band1 {*this, 1}, band2 {*this, 2}, band3 {*this, 3}, band4 {*this, 4}, band5 {*this, 5}
{
    for (unsigned long i = 1; i <= 5; ++i)
    {
        Band& band = (i == 1)? band1 :
                     (i == 2)? band2 :
                     (i == 3)? band3 :
                     (i == 4)? band4 : band5;

        String on   {i}; on   << "On";
        String type {i}; type << "Type";
        String freq {i}; freq << "Freq";
        String gain {i}; gain << "Gain";
        String q    {i}; q    << "Q";

        band.prmOn   = parameters.getRawParameterValue (on);
        band.prmType = parameters.getRawParameterValue (type);
        band.prmFreq = parameters.getRawParameterValue (freq);
        band.prmGain = parameters.getRawParameterValue (gain);
        band.prmQ    = parameters.getRawParameterValue (q);

        band.active = *band.prmOn > 0.5f;

        parameters.addParameterListener (on,   &band);
        parameters.addParameterListener (type, &band);
        parameters.addParameterListener (freq, &band);
        parameters.addParameterListener (gain, &band);
        parameters.addParameterListener (q,    &band);
    }

    prmOutputGain = parameters.getRawParameterValue ("OutGain");
    parameters.addParameterListener ("OutGain", this);
}

void EqAudioProcessor::prepareToPlay (double _sampleRate, int samplesPerBlock)
{
    sampleRate = _sampleRate;
    
    dsp::ProcessSpec spec;
    spec.sampleRate = _sampleRate;
    spec.maximumBlockSize = uint32 (samplesPerBlock);
    spec.numChannels = uint32 (getTotalNumOutputChannels ());

    band1.updateFilter();
    band2.updateFilter();
    band3.updateFilter();
    band4.updateFilter();
    band5.updateFilter();

    band1.processor.prepare (spec);
    band2.processor.prepare (spec);
    band3.processor.prepare (spec);
    band4.processor.prepare (spec);
    band5.processor.prepare (spec);

    abstractFifoInput.setTotalSize  (int (_sampleRate));
    abstractFifoOutput.setTotalSize (int (_sampleRate));

    audioFifoInput.setSize  (1, int (_sampleRate));
    audioFifoOutput.setSize (1, int (_sampleRate));

    abstractFifoInput.reset();
    abstractFifoOutput.reset();

    audioFifoInput.clear();
    audioFifoOutput.clear();
}

void EqAudioProcessor::processBlock (AudioBuffer<float>& buffer, MidiBuffer& /*midiMessages*/)
{
    dsp::AudioBlock<float> ioBuffer (buffer);
    dsp::ProcessContextReplacing<float> context  (ioBuffer);

    pushNextSampleToFifo (buffer, 0, 2, abstractFifoInput, audioFifoInput);

    if (band1.active) band1.processor.process (context);
    if (band2.active) band2.processor.process (context);
    if (band3.active) band3.processor.process (context);
    if (band4.active) band4.processor.process (context);
    if (band5.active) band5.processor.process (context);

    buffer.applyGain (Decibels::decibelsToGain (prmOutputGain->load()));

    pushNextSampleToFifo (buffer, 0, 2, abstractFifoOutput, audioFifoOutput);
}

void EqAudioProcessor::pushNextSampleToFifo (const AudioBuffer<float>& buffer, int startChannel, int numChannels,
                                             AbstractFifo& absFifo, AudioBuffer<float>& fifo)
{
    if (absFifo.getFreeSpace() < buffer.getNumSamples()) return;

    int start1, block1, start2, block2;
    absFifo.prepareToWrite (buffer.getNumSamples(), start1, block1, start2, block2);
    fifo.copyFrom (0, start1, buffer.getReadPointer (startChannel), block1);
    if (block2 > 0)
        fifo.copyFrom (0, start2, buffer.getReadPointer (startChannel, block1), block2);

    for (int channel = startChannel + 1; channel < startChannel + numChannels; ++channel)
    {
        if (block1 > 0) fifo.addFrom (0, start1, buffer.getReadPointer (channel), block1);
        if (block2 > 0) fifo.addFrom (0, start2, buffer.getReadPointer (channel, block1), block2);
    }
    absFifo.finishedWrite (block1 + block2);
    nextFFTBlockReady.store (true);
};

void EqAudioProcessor::getStateInformation (MemoryBlock& destData)
{
    auto editor = parameters.state.getOrCreateChildWithName (IDs::editor, nullptr);
    editor.setProperty (IDs::sizeX, editorSize.x, nullptr);
    editor.setProperty (IDs::sizeY, editorSize.y, nullptr);

    MemoryOutputStream stream (destData, false);
    parameters.state.writeToStream (stream);
}

void EqAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    ValueTree tree = ValueTree::readFromData (data, size_t (sizeInBytes));
    if (tree.isValid())
    {
        parameters.state = tree;

        auto editor = parameters.state.getChildWithName (IDs::editor);
        if (editor.isValid())
        {
            editorSize.setX (editor.getProperty (IDs::sizeX, 68));
            editorSize.setY (editor.getProperty (IDs::sizeY, 12));

            if (auto* activeEditor = getActiveEditor())
                activeEditor->setSize (editorSize.x, editorSize.y);
        }
    }
}

AudioProcessorEditor* EqAudioProcessor::createEditor() { return new witte::EqAudioProcessorEditor (*this, parameters); }

void EqAudioProcessor::parameterChanged (const String&, float newValue)
{
    *prmOutputGain = newValue;
    frequenciesCurveChanged.store (true);
}

AudioProcessor* JUCE_CALLTYPE createPluginFilter()     { return new EqAudioProcessor(); }

void EqAudioProcessor::Band::parameterChanged (const String& parameter, float newValue)
{
    String str = parameter.substring (1, parameter.length());

    if (str == "On")
    {
        *prmOn = newValue;
        active = newValue > 0.5f;
    }
    else
    {
             if (str == "Type") *prmType = newValue;
        else if (str == "Freq") *prmFreq = newValue;
        else if (str == "Gain") *prmGain = newValue;
        else if (str == "Q")    *prmQ    = newValue;

        updateFilter();
    }

    eqProcessor.frequenciesCurveChanged.store (true);
}

void EqAudioProcessor::Band::updateFilter()
{
    switch (int (*prmType))
    {
        case 0: *processor.state = *dsp::IIR::Coefficients<float>::makeHighPass (eqProcessor.sampleRate,
                                   *prmFreq, *prmQ);
            break;
        case 1: *processor.state = *dsp::IIR::Coefficients<float>::makeLowShelf (eqProcessor.sampleRate,
                                   *prmFreq, *prmQ, Decibels::decibelsToGain (prmGain->load()));
            break;
        case 2: *processor.state = *dsp::IIR::Coefficients<float>::makePeakFilter (eqProcessor.sampleRate,
                                   *prmFreq, *prmQ, Decibels::decibelsToGain (prmGain->load()));
            break;
        case 3: *processor.state = *dsp::IIR::Coefficients<float>::makeHighShelf (eqProcessor.sampleRate,
                                   *prmFreq, *prmQ, Decibels::decibelsToGain (prmGain->load()));
            break;
        case 4: *processor.state = *dsp::IIR::Coefficients<float>::makeLowPass (eqProcessor.sampleRate,
                                   *prmFreq, *prmQ);
            break;
    }
}
