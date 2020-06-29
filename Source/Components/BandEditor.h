#pragma once
#include "juce_audio_processors/juce_audio_processors.h"
#include "juce_gui_basics/juce_gui_basics.h"

using namespace juce;

namespace witte
{

class BandEditor : public Component
{
    public:
        BandEditor (AudioProcessorValueTreeState&, int);

        void resized() override;

        void paint (Graphics& g) override;


    private:
        AudioProcessorValueTreeState& tree;

        ToggleButton on {"0"};
        ComboBox type;
        Slider freq;
        Slider gain;
        Slider q   ;

        AudioProcessorValueTreeState::ButtonAttachment   attOn;
        AudioProcessorValueTreeState::ComboBoxAttachment attType;
        AudioProcessorValueTreeState::SliderAttachment   attFreq;
        AudioProcessorValueTreeState::SliderAttachment   attGain;
        AudioProcessorValueTreeState::SliderAttachment   attQ;


        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (BandEditor)
};

}
