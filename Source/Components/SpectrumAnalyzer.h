#pragma once
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_dsp/juce_dsp.h>
#include <juce_gui_basics/juce_gui_basics.h>

#include "../EqProcessor.h"
#include "SpectrumBgMarkers.h"

using namespace juce;

namespace witte
{

class SpectrumAnalyzer : public Component, private Timer
{
    public:
        SpectrumAnalyzer (EqAudioProcessor&);

        void paint (Graphics&) override;
        void resized() override;


    private:
        EqAudioProcessor& processor;

        witte::SpectrumBgMarkers markers;

        Colour baseColor {uint8 (1), uint8 (28), uint8 (39)};

        dsp::FFT fftInput  {12};
        dsp::FFT fftOutput {12};

        dsp::WindowingFunction<float> hannWindow {size_t (fftInput.getSize()), dsp::WindowingFunction<float>::hann};

        AudioBuffer<float> fftBufferInput  { 1, fftInput.getSize()  * 2 };
        AudioBuffer<float> fftBufferOutput { 1, fftOutput.getSize() * 2 };

        AudioBuffer<float> avgInput  { 5, fftInput.getSize() / 2 };
        AudioBuffer<float> avgOutput { 5, fftOutput.getSize() / 2 };
        int avgInputPtr  = 1;
        int avgOutputPtr = 1;

        Path inP;
        Path outP;
        CriticalSection pathCreationLock;

        struct fftPoint
        {
            int firstBinIndex = 0;
            int lastBinIndex = 120;

            int x;
        };
        int fftPointsSize = 0;
        std::vector<fftPoint> fftPoints;

        float getFftPointLevel (const float* buffer, const fftPoint& point);

        static constexpr float maxdB =  6.0f;
        static constexpr float mindB = -84.0f;

        void drawNextFrame();

        void timerCallback() override;
    

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SpectrumAnalyzer)
};

}
