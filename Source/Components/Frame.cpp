#include "Frame.h"
#include <BinaryData.h>
#include "../Helpers/UnitConversions.h"
#include "Utils.h"


namespace witte
{


Frame::Frame (const int maxWidth, const int maxHeight)
  : spectrumBackground {juce::Image::PixelFormat::ARGB, maxWidth, maxHeight, true},
    spectrumForeground {juce::Image::PixelFormat::ARGB, maxWidth, maxHeight, true},
    openSansBold {juce::FontOptions{juce::Typeface::createSystemTypefaceFor (BinaryData::OpenSansCondensedBold_ttf, BinaryData::OpenSansCondensedBold_ttfSize)}}
{
    setOpaque (true);
}

void Frame::paint (juce::Graphics& g)
{
    g.drawImageAt (spectrumBackground, 0, 0);
}

void Frame::paintOverChildren (juce::Graphics& g)
{
    g.drawImageAt (spectrumForeground, 0, 0);
}

void Frame::resized()
{
    drawSpectrumBackground();
    drawSpectrumForeground();
}

void Frame::drawSpectrumBackground()
{
    const auto bounds = getLocalBounds().toFloat();
    const auto width  = bounds.getWidth();
    const auto height = bounds.getHeight();

    juce::Graphics g {spectrumBackground};
    g.setColour (baseColor);
    g.fillRect (bounds);

    g.setColour (baseColor.brighter (0.036f));
    utils::drawHorizontalLine (g, height * 0.5f, 0.0f, width);

    for (auto& bandGain : bandGains)
    {
        const auto pos = units::gainToProportion (bandGain) * height;
        utils::drawHorizontalLine (g, pos, 0, width);
    }

    for (auto& freq : freqsB)
    {
        const auto pos = units::freqToProportion (freq) * width;
        utils::drawVerticalLine (g, pos, 0, height);
    }

    g.setColour (baseColor.brighter (0.082f));
    for (auto& freq : freqsA)
    {
        const auto pos = units::freqToProportion (freq) * width;
        utils::drawVerticalLine (g, pos, 0, height);
    }
}

void Frame::drawSpectrumForeground()
{
    const auto bounds = getLocalBounds().toFloat();
    const auto width  = bounds.getWidth();
    const auto height = bounds.getHeight();

    spectrumForeground.clear (spectrumForeground.getBounds());

    juce::Graphics g {spectrumForeground};
    g.setColour (baseColor.brighter (0.4f));
    g.setFont (openSansBold);

    for (auto& bandGain : bandGains)
    {
        const auto pos = juce::roundToInt (units::gainToProportion (bandGain) * height);

        g.drawText (juce::String {bandGain}, static_cast<int>(width) - 44, pos - 14, 42, 28,
            juce::Justification::centredRight);
    }

    for (auto& gain : gains)
    {
        const int pos = juce::roundToInt (units::spectrumGainToProportion (gain) * height);

        g.drawText (juce::String { std::abs (gain)}, 2, pos - 14, 42, 28,
            juce::Justification::centredLeft);
    }

    g.setFont (openSansBold);
    g.setFont (15.0f);
    g.setColour (baseColor.brighter (2.8f));
    for (auto& freq : freqsA)
    {
        const int pos = juce::roundToInt (units::freqToProportion (freq) * width);
        juce::String str;

        if (freq >= 1000)
        {
            str << (freq / 1000);
            str << "k";
        }
        else
        {
            str << freq;
        }

        g.drawText (str, pos - 21, static_cast<int>(height * 0.5f) - 14, 42, 28,
            juce::Justification::centred);
    }
}


}
