#pragma once
#include "JuceHeader.h"

namespace witte
{

class BandEditor : public juce::Component
{
    public:
        BandEditor (AudioProcessorValueTreeState&, int);

        void resized() override;

        void paint (Graphics& g) override;


    private:
        AudioProcessorValueTreeState& tree;

        ToggleButton on {String::fromUTF8 (u8"\uf16b")};
        ComboBox     type;
        PopupMenu    contextMenu;
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
