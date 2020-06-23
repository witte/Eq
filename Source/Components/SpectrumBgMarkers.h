#pragma once
#include <array>
#include <juce_gui_basics/juce_gui_basics.h>

using namespace juce;

namespace witte
{

class SpectrumBgMarkers
{
    public:
        SpectrumBgMarkers () {}

        void setSize(int newWidth, int newHeight);
        Image& getImage();


    private:
        int width  = 768 * 2; // twice the initial Eq's size
        int height = 482 * 2;

        Colour baseColor {0xff011523};

        constexpr static std::array<float, 7> gains = { -12.0f, -24.0f, -36.0f, -48.0f, -60.0f, -72.0f, -84.0f };
        constexpr static std::array<float, 4> bandGains = { 24.0f, 12.0f, -12.0f, -24.0f };

        constexpr static std::array<float, 3> freqsA = { 100.0f, 1000.0f, 10000.0f };
        constexpr static std::array<float, 25> freqsB = {  20.0f,   30.0f,   40.0f,   50.0f,   60.0f,   70.0f,   80.0f,   90.0f,
                                                          200.0f,  300.0f,  400.0f,  500.0f,  600.0f,  700.0f,  800.0f,  900.0f,
                                                         2000.0f, 3000.0f, 4000.0f, 5000.0f, 6000.0f, 7000.0f, 8000.0f, 9000.0f,
                                                        20000.0f };

        static float getPositionForFrequency (float freq)
        {
            return (std::log (freq / 20.0f) / std::log (2.0f)) / 10.0f;
        }

        Image image {Image::ARGB, width, height, true};

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SpectrumBgMarkers)
};

}
