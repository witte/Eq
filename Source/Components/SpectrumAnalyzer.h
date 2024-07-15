#pragma once
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_dsp/juce_dsp.h>
#include <juce_gui_basics/juce_gui_basics.h>
#include "../EqProcessor.h"


namespace witte
{


class SpectrumAnalyzer final : public juce::Component, juce::Timer
{
    public:
        explicit SpectrumAnalyzer (EqAudioProcessor&);

        void paint (juce::Graphics&) override;
        void resized() override;


    private:
        EqAudioProcessor& processor;

        juce::dsp::FFT fftInput  {12};
        juce::dsp::FFT fftOutput {12};

        juce::dsp::WindowingFunction<float> hannWindow {static_cast<size_t>(fftInput.getSize()),
            juce::dsp::WindowingFunction<float>::hann};

        juce::AudioBuffer<float> fftBufferInput  { 1, fftInput.getSize()  * 2 };
        juce::AudioBuffer<float> fftBufferOutput { 1, fftOutput.getSize() * 2 };

        juce::AudioBuffer<float> avgInput  { 5, fftInput.getSize() / 2 };
        juce::AudioBuffer<float> avgOutput { 5, fftOutput.getSize() / 2 };
        int avgInputPtr  = 1;
        int avgOutputPtr = 1;

        juce::Colour baseColor {0xff011c27};

        juce::Path inP;
        juce::Path outP;
        juce::CriticalSection pathCreationLock;

        struct fftPoint
        {
            int firstBinIndex = 0;
            int lastBinIndex = 120;

            int x = 0;
        };
        int fftPointsSize = 0;
        std::vector<fftPoint> fftPoints;

        static float getFftPointLevel (const float* buffer, const fftPoint& point);

        static constexpr float maxdB =  6.0f;
        static constexpr float mindB = -84.0f;

        void drawNextFrame();

        void timerCallback() override;
    

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SpectrumAnalyzer)
};


}
