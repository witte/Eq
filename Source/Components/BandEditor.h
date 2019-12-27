#pragma once
#include "JuceHeader.h"
#include "Knob.h"

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


        ToggleButton on   {String::fromUTF8 (u8"\uf16b")};
        ComboBox     type;
        PopupMenu    contextMenu;
        witte::Knob  freq {1};
        witte::Knob  gain {2};
        witte::Knob  q    {2};

        AudioProcessorValueTreeState::ButtonAttachment   attOn;
        AudioProcessorValueTreeState::ComboBoxAttachment attType;
        AudioProcessorValueTreeState::SliderAttachment   attFreq;
        AudioProcessorValueTreeState::SliderAttachment   attGain;
        AudioProcessorValueTreeState::SliderAttachment   attQ;


        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (BandEditor)
};

}
