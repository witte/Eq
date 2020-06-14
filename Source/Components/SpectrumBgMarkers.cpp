#include "SpectrumBgMarkers.h"

namespace witte
{

void SpectrumBgMarkers::setSize(int newWidth, int newHeight)
{
    if ((newWidth == width) && (newHeight == height)) return;

    width = newWidth;
    height = newHeight;

    Graphics g {image};
    g.fillAll (baseColor);

    g.setColour (baseColor.brighter (0.024f));
    g.drawHorizontalLine (int (height * 0.5f), 0, width);

    for (auto& bandGain : bandGains)
    {
        int pos = roundToInt (jmap (bandGain, -26.0f, 26.0f, float (height), 0.0f));
        g.drawHorizontalLine (pos, 0, width);
    }

    for (auto& freq : freqsB)
    {
        int pos = roundToInt (getPositionForFrequency (freq) * width);
        g.drawVerticalLine (pos, 0, height);
    }

    g.setColour (baseColor.brighter (0.072f));
    for (auto& freq : freqsA)
    {
        int pos = roundToInt (getPositionForFrequency (freq) * width);
        g.drawVerticalLine (pos, 0, height);
    }
}

Image& SpectrumBgMarkers::getImage()
{
    return image;
}

}
