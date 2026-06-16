#include "Editor.h"
#include "Processor.h"


namespace IDs
{
    const juce::String editor {"editor"};
    const juce::String sizeX  {"size-x"};
    const juce::String sizeY  {"size-y"};
}

Processor::Processor() :
    vts (*this, nullptr, "Eq", std::move (prmLayout)),
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

        band.prmOn   = vts.getRawParameterValue (on);
        band.prmType = vts.getRawParameterValue (type);
        band.prmFreq = vts.getRawParameterValue (freq);
        band.prmGain = vts.getRawParameterValue (gain);
        band.prmQ    = vts.getRawParameterValue (q);

        band.active = *band.prmOn > 0.5f;

        vts.addParameterListener (on,   &band);
        vts.addParameterListener (type, &band);
        vts.addParameterListener (freq, &band);
        vts.addParameterListener (gain, &band);
        vts.addParameterListener (q,    &band);
    }

    prmOutputGain = vts.getRawParameterValue (idOutputGain);
    vts.addParameterListener (idOutputGain, this);
}

Processor::~Processor() = default;

void Processor::prepareToPlay (const double sampleRate, const int samplesPerBlock)
{
    const juce::dsp::ProcessSpec spec { sampleRate, static_cast<juce::uint32>(samplesPerBlock),
        static_cast<juce::uint32>(getTotalNumOutputChannels()) };

    for (auto& band : bands)
    {
        band.updateFilter();
        band.dspProcessor.prepare (spec);
    }
}

void Processor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& /*midiMessages*/)
{
    juce::dsp::AudioBlock<float> ioBuffer{buffer};
    const juce::dsp::ProcessContextReplacing context{ioBuffer};

    const auto shouldCopyToFifo = copyToFifo.load();

    if (shouldCopyToFifo)
        pushNextSampleToFifo (buffer, 0, 2, abstractFifoInput, audioFifoInput);

    for (auto& band : bands)
    {
        if (!band.active)
            continue;

        band.dspProcessor.process (context);
    }

    buffer.applyGain (juce::Decibels::decibelsToGain (prmOutputGain->load()));

    if (shouldCopyToFifo)
    {
        pushNextSampleToFifo (buffer, 0, 2, abstractFifoOutput, audioFifoOutput);
        isNextFFTBlockReady.store(true);
    }
}

void Processor::pushNextSampleToFifo (const juce::AudioBuffer<float>& buffer, const int startChannel,
                                      const int numChannels, juce::AbstractFifo& absFifo,
                                      juce::AudioBuffer<float>& fifo)
{
    if (absFifo.getFreeSpace() < buffer.getNumSamples())
        return;

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
        if (block1 > 0)
            fifo.addFrom (0, start1, buffer.getReadPointer (channel), block1);

        if (block2 > 0)
            fifo.addFrom (0, start2, buffer.getReadPointer (channel, block1), block2);
    }

    absFifo.finishedWrite (block1 + block2);
}

void Processor::getStateInformation (juce::MemoryBlock& destData)
{
    auto editor = vts.state.getOrCreateChildWithName (IDs::editor, nullptr);
    editor.setProperty (IDs::sizeX, editorSize.x, nullptr);
    editor.setProperty (IDs::sizeY, editorSize.y, nullptr);

    juce::MemoryOutputStream stream (destData, false);
    vts.state.writeToStream (stream);
}

void Processor::setStateInformation (const void* data, const int sizeInBytes)
{
    const juce::ValueTree tree = juce::ValueTree::readFromData (data, static_cast<size_t>(sizeInBytes));
    if (!tree.isValid())
        return;

    vts.state = tree;

    const auto editor = vts.state.getChildWithName (IDs::editor);
    if (!editor.isValid())
        return;

    editorSize.setX (editor.getProperty (IDs::sizeX, 68));
    editorSize.setY (editor.getProperty (IDs::sizeY, 12));

    if (auto* ed = getActiveEditor())
        ed->setSize (editorSize.x, editorSize.y);
}

juce::AudioProcessorEditor* Processor::createEditor() { return new witte::Editor (*this, vts); }

void Processor::parameterChanged (const juce::String&, const float newValue)
{
    *prmOutputGain = newValue;
}

void Processor::setCopyToFifo (const bool _copyToFifo)
{
    if (_copyToFifo)
    {
        const auto sr = static_cast<int>(getSampleRate());

        abstractFifoInput.setTotalSize  (sr);
        abstractFifoOutput.setTotalSize (sr);

        audioFifoInput.setSize  (1, sr);
        audioFifoOutput.setSize (1, sr);

        abstractFifoInput.reset();
        abstractFifoOutput.reset();

        audioFifoInput.clear();
        audioFifoOutput.clear();
    }

    copyToFifo.store (_copyToFifo);
}


juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter() { return new Processor(); }
