#pragma once
#include "EqProcessor.h"
#include "Components/BandEditor.h"
#include "Components/Frame.h"
#include "Components/SpectrumAnalyzer.h"
#include "Components/XYPad.h"
#include "Components/FrequencyCurve.h"
#include "LookAndFeel/LookAndFeel.h"


namespace witte
{


class EqAudioProcessorEditor final : public juce::AudioProcessorEditor
{
    public:
        EqAudioProcessorEditor (EqAudioProcessor&, juce::AudioProcessorValueTreeState&);
        ~EqAudioProcessorEditor() override;

        void paint (juce::Graphics&) override;
        void resized() override;


    private:
        EqAudioProcessor& processor;
        juce::AudioProcessorValueTreeState& tree;
        witte::LookAndFeel lookAndFeel;

        constexpr static int defaultWidth = 768;
        constexpr static int defaultHeight = 482;

        juce::Colour baseColor {0xff011523};
        juce::Image background;

        witte::Frame frame;
        witte::SpectrumAnalyzer analyzer;
        witte::FrequencyCurve frequencyCurve;
        witte::XYPad xyPad;

        witte::BandEditor band1;
        witte::BandEditor band2;
        witte::BandEditor band3;
        witte::BandEditor band4;
        witte::BandEditor band5;

        juce::Slider outputGain;
        juce::AudioProcessorValueTreeState::SliderAttachment attOutputGain;


        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (EqAudioProcessorEditor)
};


}
