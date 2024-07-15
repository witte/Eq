#pragma once
#include "juce_audio_processors/juce_audio_processors.h"
#include "juce_gui_basics/juce_gui_basics.h"


namespace witte
{


class BandEditor final : public juce::Component
{
    public:
        BandEditor (juce::AudioProcessorValueTreeState&, int);

        void resized() override;

        void paint (juce::Graphics& g) override;


    private:
        juce::AudioProcessorValueTreeState& tree;

        juce::ToggleButton on {"0"};
        juce::ComboBox type;
        juce::Slider freq;
        juce::Slider gain;
        juce::Slider q;

        juce::AudioProcessorValueTreeState::ButtonAttachment   attOn;
        juce::AudioProcessorValueTreeState::ComboBoxAttachment attType;
        juce::AudioProcessorValueTreeState::SliderAttachment   attFreq;
        juce::AudioProcessorValueTreeState::SliderAttachment   attGain;
        juce::AudioProcessorValueTreeState::SliderAttachment   attQ;


        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (BandEditor)
};


}
