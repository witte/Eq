#pragma once
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_dsp/juce_dsp.h>


class Processor;

struct Band final : juce::AudioProcessorValueTreeState::Listener
{
    Band (Processor& eqProcessor, int index);
    ~Band() override;

    bool active {true};

    const juce::Identifier idOn;
    const juce::Identifier idType;
    const juce::Identifier idFreq;
    const juce::Identifier idGain;
    const juce::Identifier idQ;

    std::atomic<float>* prmOn   {nullptr};
    std::atomic<float>* prmType {nullptr};
    std::atomic<float>* prmFreq {nullptr};
    std::atomic<float>* prmGain {nullptr};
    std::atomic<float>* prmQ    {nullptr};

    void parameterChanged (const juce::String& parameter, float newValue) override;

    void updateFilter() const;

    [[nodiscard]] int getIndex() const { return index; }

    juce::dsp::ProcessorDuplicator<juce::dsp::IIR::Filter<float>, juce::dsp::IIR::Coefficients<float>> dspProcessor;


    private:
        Processor& eqProcessor;
        int index {0};
};
