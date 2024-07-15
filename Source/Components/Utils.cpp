#include "Utils.h"


namespace witte::utils
{


void drawVerticalLine(const juce::Graphics& context, const float x, const float top, const float bottom)
{
    if (top < bottom)
        context.fillRect (juce::Rectangle { x, top, 1.0f, bottom - top });
}

void drawHorizontalLine(const juce::Graphics& context, const float y, const float left, const float right)
{
    if (left < right)
        context.fillRect (juce::Rectangle { left, y, right - left, 1.0f });
}


}
