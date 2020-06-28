#include "Frame.h"
#include <BinaryData.h>
#include "../Helpers/UnitConversions.h"

namespace witte
{

Frame::Frame (int maxWidth, int maxHeight)
  : spectrumBackground {Image::PixelFormat::ARGB, maxWidth, maxHeight, true},
    spectrumForeground {Image::PixelFormat::ARGB, maxWidth, maxHeight, true},
    openSansBold {Typeface::createSystemTypefaceFor (BinaryData::OpenSansCondensedBold_ttf, BinaryData::OpenSansCondensedBold_ttfSize)}
{
    setOpaque (true);
}

void Frame::paint (Graphics& g)
{
    g.drawImageAt (spectrumBackground, 0, 0);
}

void Frame::paintOverChildren (Graphics& g)
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
    const auto bounds = getLocalBounds();
    const auto width  = bounds.getWidth();
    const auto height = bounds.getHeight();

    Graphics g {spectrumBackground};
    g.setColour (baseColor);
    g.fillRect (bounds);

    g.setColour (baseColor.brighter (0.036f));
    g.drawHorizontalLine (int (height * 0.5f), 0, width);

    for (auto& bandGain : bandGains)
    {
        int pos = units::gainToProportion (bandGain) * height;
        g.drawHorizontalLine (pos, 0, width);
    }

    for (auto& freq : freqsB)
    {
        int pos = roundToInt (units::freqToProportion (freq) * width);
        g.drawVerticalLine (pos, 0, height);
    }

    g.setColour (baseColor.brighter (0.082f));
    for (auto& freq : freqsA)
    {
        int pos = roundToInt (units::freqToProportion (freq) * width);
        g.drawVerticalLine (pos, 0, height);
    }
}

void Frame::drawSpectrumForeground()
{
    const auto bounds = getLocalBounds();
    const auto width  = bounds.getWidth();
    const auto height = bounds.getHeight();

    spectrumForeground.clear (spectrumForeground.getBounds());

    Graphics g {spectrumForeground};
    g.setColour (baseColor.brighter (0.4f));
    g.setFont (openSansBold);

    for (auto& bandGain : bandGains)
    {
        int pos = roundToInt (units::gainToProportion (bandGain) * height);
        g.drawText (String {bandGain}, width - 44, pos - 14, 42, 28, Justification::centredRight);
    }

    for (auto& gain : gains)
    {
        int pos = roundToInt (units::spectrumGainToProportion (gain) * height);
        g.drawText (String { std::abs (gain)}, 2, pos - 14, 42, 28, Justification::centredLeft);
    }

    g.setFont (openSansBold);
    g.setFont (15.0f);
    g.setColour (baseColor.brighter (2.8f));
    for (auto& freq : freqsA)
    {
        int pos = roundToInt (units::freqToProportion (freq) * width);
        String str;
        if (freq >= 1000)
        {
            str << (freq / 1000);
            str << "k";
        }
        else
        {
            str << freq;
        }

        g.drawText (str, pos - 21, (height * 0.5f) - 14, 42, 28, Justification::centred);
    }
}

}
