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

        std::function<void(float)> prmChangedCallback = [&] (float) { repaint(); };
        std::vector<std::unique_ptr<ParameterAttachment>> attachments;

        Colour baseColor {0xff011523};

        constexpr static std::array<float, 7> gains = { -12.0f, -24.0f, -36.0f, -48.0f, -60.0f, -72.0f, -84.0f };
        constexpr static std::array<float, 4> bandGains = { 24.0f, 12.0f, -12.0f, -24.0f };

        constexpr static std::array<float, 3> freqsA = { 100.0f, 1000.0f, 10000.0f };
        constexpr static std::array<float, 25> freqsB = {  20.0f,   30.0f,   40.0f,   50.0f,   60.0f,   70.0f,   80.0f,   90.0f,
                                                          200.0f,  300.0f,  400.0f,  500.0f,  600.0f,  700.0f,  800.0f,  900.0f,
                                                         2000.0f, 3000.0f, 4000.0f, 5000.0f, 6000.0f, 7000.0f, 8000.0f, 9000.0f,
                                                        20000.0f };

        Path frequencyCurvePath;
        CriticalSection freqPathCreationLock;

        std::vector<double> frequencies;
        std::vector<Point<float>> frequenciesPoints;
        std::array<std::vector<double>, 5> magnitudes;
        std::vector<double> magnitudesOut;


        static float getPositionForFrequency (float freq)
        {
            return (std::log (freq / 20.0f) / std::log (2.0f)) / 10.0f;
        }
        static float getFrequencyForPosition (float pos)
        {
            return 20.0f * std::pow (2.0f, pos * 10.0f);
        }

        static constexpr float maxdB =  6.0f;
        static constexpr float mindB = -84.0f;

        static constexpr float maxBanddB =  24.0f;
        static constexpr float minBanddB = -24.0f;

        static float getPositionForGain (float gain, float top, float bottom)
        {
            return jmap (Decibels::gainToDecibels (gain, mindB), mindB, maxdB, bottom, top);
        }

        void mouseDown (const MouseEvent& event) override;
        void mouseMove (const MouseEvent& event) override;
        void mouseDrag (const MouseEvent& event) override;
        void mouseUp   (const MouseEvent&) override;

        Font openSansBold;

        void drawFrequencyCurve();
        Image foregroundImage {Image::ARGB, 768 * 2, 482 * 2, true};

        void drawForegroundImage();


        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (FrequencyCurve)
};

}
