#pragma once
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_dsp/juce_dsp.h>
#include <juce_gui_basics/juce_gui_basics.h>

#include "../EqProcessor.h"

using namespace juce;

namespace witte
{

class FrequencyCurve : public Component
{
    public:
        FrequencyCurve (EqAudioProcessor&, std::initializer_list<RangedAudioParameter*> parameters);
        ~FrequencyCurve() override;

        void paint (Graphics&) override;
        void resized() override;


    private:
        EqAudioProcessor& processor;

        std::function<void(float)> prmChangedCallback = [&] (float) { drawFrequencyCurve(); repaint(); };
        std::vector<std::unique_ptr<ParameterAttachment>> attachments;

        Colour baseColor {0xff011523};

        Path frequencyCurvePath;
        CriticalSection freqPathCreationLock;

        std::vector<double> frequencies;
        std::vector<Point<float>> frequenciesPoints;
        std::array<std::vector<double>, 5> magnitudes;
        std::vector<double> magnitudesOut;
        void drawFrequencyCurve();


        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (FrequencyCurve)
};

}
