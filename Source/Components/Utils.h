#pragma once
#include <juce_graphics/juce_graphics.h>


namespace witte::utils
{


void drawVerticalLine (const juce::Graphics& context, float x, float top, float bottom);

void drawHorizontalLine (const juce::Graphics& context, float y, float left, float right);


}
