#include "Editor.h"
#include "Processor.h"


namespace IDs
{
    const juce::String editor {"editor"};
    const juce::String sizeX  {"size-x"};
    const juce::String sizeY  {"size-y"};
}

Processor::Processor() :
    parameters (*this, &undoManager, "Eq", std::move (prmLayout)),
    bands { Band {*this, 1}, Band {*this, 2}, Band {*this, 3}, Band {*this, 4}, Band {*this, 5} }
{
    for (unsigned long i = 1; i <= 5; ++i)
    {
        Band& band = bands[i - 1];

        juce::String on   {i}; on   << "On";
        juce::String type {i}; type << "Type";
        juce::String freq {i}; freq << "Freq";
        juce::String gain {i}; gain << "Gain";
        juce::String q    {i}; q    << "Q";

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

    prmOutputGain = parameters.getRawParameterValue (idOutputGain);
    parameters.addParameterListener (idOutputGain, this);
}

void Processor::prepareToPlay (const double _sampleRate, const int samplesPerBlock)
{
    sampleRate = _sampleRate;
    
    const juce::dsp::ProcessSpec spec { _sampleRate, static_cast<juce::uint32>(samplesPerBlock),
        static_cast<juce::uint32>(getTotalNumOutputChannels()) };

    for (auto& band : bands)
    {
        band.updateFilter();
        band.processor.prepare (spec);
    }
}

void Processor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& /*midiMessages*/)
{
    juce::dsp::AudioBlock<float> ioBuffer{buffer};
    const juce::dsp::ProcessContextReplacing context{ioBuffer};

    if (copyToFifo) pushNextSampleToFifo (buffer, 0, 2, abstractFifoInput, audioFifoInput);

    for (auto& band : bands)
    {
        if (band.active) band.processor.process (context);
    }

    buffer.applyGain (juce::Decibels::decibelsToGain (prmOutputGain->load()));

    if (copyToFifo) pushNextSampleToFifo (buffer, 0, 2, abstractFifoOutput, audioFifoOutput);
}

void Processor::pushNextSampleToFifo (const juce::AudioBuffer<float>& buffer, const int startChannel,
                                             const int numChannels, juce::AbstractFifo& absFifo,
                                             juce::AudioBuffer<float>& fifo)
{
    if (absFifo.getFreeSpace() < buffer.getNumSamples()) return;

    int start1;
    int block1;
    int start2;
    int block2;
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
}

void Processor::getStateInformation (juce::MemoryBlock& destData)
{
    auto editor = parameters.state.getOrCreateChildWithName (IDs::editor, nullptr);
    editor.setProperty (IDs::sizeX, editorSize.x, nullptr);
    editor.setProperty (IDs::sizeY, editorSize.y, nullptr);

    juce::MemoryOutputStream stream (destData, false);
    parameters.state.writeToStream (stream);
}

void Processor::setStateInformation (const void* data, int sizeInBytes)
{
    juce::ValueTree tree = juce::ValueTree::readFromData (data, size_t (sizeInBytes));
    if (tree.isValid())
    {
        parameters.state = tree;

        auto editor = parameters.state.getChildWithName (IDs::editor);
        if (editor.isValid())
        {
            editorSize.setX (editor.getProperty (IDs::sizeX, 68));
            editorSize.setY (editor.getProperty (IDs::sizeY, 12));

            if (auto* ed = getActiveEditor())
                ed->setSize (editorSize.x, editorSize.y);
        }
    }
}

juce::AudioProcessorEditor* Processor::createEditor() { return new witte::Editor (*this, parameters); }

void Processor::parameterChanged (const juce::String&, float newValue)
{
    *prmOutputGain = newValue;
}

void Processor::setCopyToFifo (bool _copyToFifo)
{
    if (_copyToFifo)
    {
        abstractFifoInput.setTotalSize  (int (sampleRate));
        abstractFifoOutput.setTotalSize (int (sampleRate));

        audioFifoInput.setSize  (1, int (sampleRate));
        audioFifoOutput.setSize (1, int (sampleRate));

        abstractFifoInput.reset();
        abstractFifoOutput.reset();

        audioFifoInput.clear();
        audioFifoOutput.clear();
    }

    copyToFifo.store (_copyToFifo);
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter() { return new Processor(); }
