#pragma once
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_dsp/juce_dsp.h>
#include <juce_gui_basics/juce_gui_basics.h>

#include "../EqProcessor.h"

using namespace juce;

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
        Path frequencyCurvePath;
        Path bandsPositionsPath;
        CriticalSection pathCreationLock;
        CriticalSection freqPathCreationLock;

        struct fftPoint
        {
            int firstBinIndex = 0;
            int lastBinIndex = 120;

            int x;
        };
        int fftPointsSize = 0;
        std::vector<fftPoint> fftPoints;

        float getFftPointLevel (const float* buffer, const fftPoint& point);


        std::array<float, 7> gains = { -12.0f, -24.0f, -36.0f, -48.0f, -60.0f, -72.0f, -84.0f };
        std::array<float, 4> bandGains = { 24.0f, 12.0f, -12.0f, -24.0f };

        std::array<float, 3> freqsA = { 100.0f, 1000.0f, 10000.0f };
        std::array<float, 25> freqsB = {  20.0f,   30.0f,   40.0f,   50.0f,   60.0f,   70.0f,   80.0f,   90.0f,
                                         200.0f,  300.0f,  400.0f,  500.0f,  600.0f,  700.0f,  800.0f,  900.0f,
                                        2000.0f, 3000.0f, 4000.0f, 5000.0f, 6000.0f, 7000.0f, 8000.0f, 9000.0f,
                                       20000.0f };

        int movingBand {-1};
        int hoveringBand {-1};
        std::array<Rectangle<float>, 5> plotAreas {{}};

        std::vector<double> frequencies;
        std::vector<Point<float>> frequenciesPoints;
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

        void mouseDown (const MouseEvent& event) override;
        void mouseMove (const MouseEvent& event) override;
        void mouseDrag (const MouseEvent& event) override;
        void mouseUp   (const MouseEvent&) override;

        Font openSansBold;

        bool isEmpty = true;

        void drawNextFrame();
        void drawFrequencyCurve();
        Image backgroundImage {Image::ARGB, 768 * 2, 482 * 2, true};
        Image foregroundImage {Image::ARGB, 768 * 2, 482 * 2, true};

        void drawSpectrumCurves (Graphics& g, float width, float height);
        void drawBackgroundImage();
        void drawForegroundImage();

        void timerCallback() override;
    

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SpectrumAnalyzer)
};

}
