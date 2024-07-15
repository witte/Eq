#pragma once
#include <juce_gui_basics/juce_gui_basics.h>
#include "../EqProcessor.h"


namespace witte
{


class FrequencyCurve final : public juce::Component,
                             public juce::AudioProcessorValueTreeState::Listener
{
    public:
        explicit FrequencyCurve (EqAudioProcessor&);
        ~FrequencyCurve() override;

        void paint (juce::Graphics&) override;
        void resized() override;


    private:
        EqAudioProcessor& processor;

        void parameterChanged (const juce::String&, float) override;

        std::mutex mutex;

        juce::Colour baseColor {0xff011523};

        juce::Path frequencyCurvePath;

        std::vector<double> frequencies;
        std::vector<juce::Point<float>> frequenciesPoints;
        std::array<std::vector<double>, 5> magnitudes;
        std::vector<double> magnitudesOut;
        void drawFrequencyCurve();


        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (FrequencyCurve)
};


}
