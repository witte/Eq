#pragma once
#include "JuceHeader.h"
#include "../EqProcessor.h"

namespace witte
{

class SpectrumAnalyzer : public Component, private Timer
{
    public:
        SpectrumAnalyzer (EqAudioProcessor&, AudioProcessorValueTreeState&);
        ~SpectrumAnalyzer() override;

        void paint (Graphics&) override;
        void resized() override;


    private:
        EqAudioProcessor& processor;
        AudioProcessorValueTreeState& tree;

        dsp::FFT fftInput  {12};
        dsp::FFT fftOutput {12};

        dsp::WindowingFunction<float> hannWindow {size_t (fftInput.getSize()),  dsp::WindowingFunction<float>::hann};

        AudioBuffer<float> fftBufferInput  { 1, fftInput.getSize()  * 2 };
        AudioBuffer<float> fftBufferOutput { 1, fftOutput.getSize() * 2 };

        AudioBuffer<float> avgInput  { 5, fftInput.getSize() / 2 };
        AudioBuffer<float> avgOutput { 5, fftOutput.getSize() / 2 };
        int avgInputPtr  = 1;
        int avgOutputPtr = 1;

        Path inP;
        Path outP;
        Path frequencyCurvePath;
        Path bandsPositionsPath;
        CriticalSection pathCreationLock;
        CriticalSection freqPathCreationLock;

        int movingBand {-1};
        int hoveringBand {-1};
        std::array<Rectangle<float>, 5> plotAreas {{}};

        std::vector<double> frequencies;
        std::array<std::vector<double>, 5> magnitudes;
        std::vector<double> magnitudesOut;


        static constexpr float maxdB =  6.0f;
        static constexpr float mindB = -84.0f;

        static constexpr float maxBanddB =  24.0f;
        static constexpr float minBanddB = -24.0f;

        static float getPositionForFrequency (float freq)
        {
            return (std::log (freq / 20.0f) / std::log (2.0f)) / 10.0f;
        }
        static float getFrequencyForPosition (float pos)
        {
            return 20.0f * std::pow (2.0f, pos * 10.0f);
        }

        static float getPositionForGain (float gain, float top, float bottom)
        {
            return jmap (Decibels::gainToDecibels (gain, mindB), mindB, maxdB, bottom, top);
        }

        inline float indexToX (float index) const
        {
            const auto freq = (float (processor.getSampleRate()) * index) / fftInput.getSize();
            auto x = std::log (freq / 20.f) / std::log (2.0f);

            return (x > 0.0f) ? x : 0.0f;
        }

        void mouseDown (const MouseEvent& event) override;
        void mouseMove (const MouseEvent& event) override;
        void mouseDrag (const MouseEvent& event) override;
        void mouseUp   (const MouseEvent&) override;

        Font openSansBold;

        float sampleRate {48000.0f};

        bool isEmpty = true;

        void drawNextFrame();
        void drawFrequencyCurve();

        void timerCallback() override;
    
        PathStrokeType frequencyCurve {1.5f, PathStrokeType::JointStyle::beveled};


        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SpectrumAnalyzer)
};

}
