#pragma once
#include "../Processor.h"
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_dsp/juce_dsp.h>
#include <juce_gui_basics/juce_gui_basics.h>


namespace witte
{


class SpectrumAnalyzer final : public juce::Component, juce::Timer
{
    public:
        explicit SpectrumAnalyzer (Processor&);

        void paint (juce::Graphics& g) override;
        void resized() override;


    private:
        struct fftPoint
        {
            int firstBinIndex = 0;
            int lastBinIndex = 120;

            int x = 0;
        };

        static int getFftOrderForSampleRate (double sampleRate) noexcept;
        void initFft (double sampleRate);

        static float getFftPointLevel (const float* buffer, const fftPoint& point);

        static constexpr float maxdB =  6.0f;
        static constexpr float mindB = -84.0f;

        void recalculateFftPoints();
        void drawNextFrame();

        void timerCallback() override;

        Processor& processor;

        int resizeDebounceInFrames = 0;
        double lastSampleRate = 48000;

        static constexpr double targetTimeSeconds { 4096.0 / 96000.0 };
        int fftOrder = 12;
        juce::dsp::FFT fftInput { fftOrder };
        juce::dsp::FFT fftOutput { fftOrder };

        std::unique_ptr<juce::dsp::WindowingFunction<float>> hannWindow;

        juce::AudioBuffer<float> fftBufferInput;
        juce::AudioBuffer<float> fftBufferOutput;

        juce::AudioBuffer<float> avgInput  { 5, 2048 };
        juce::AudioBuffer<float> avgOutput { 5, 2048 };
        int avgInputPtr  = 1;
        int avgOutputPtr = 1;

        const juce::Colour baseColor { 0xff011c27 };

        juce::Path inP;
        juce::Path outP;
        juce::CriticalSection pathCreationLock;

        int fftPointsSize = 0;
        std::vector<fftPoint> fftPoints;
    

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SpectrumAnalyzer)
};


}
