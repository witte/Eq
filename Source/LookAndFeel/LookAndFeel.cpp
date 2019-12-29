#include "LookAndFeel.h"
#include "../Components/Label.h"

namespace witte
{

LookAndFeel::LookAndFeel() : juce::LookAndFeel_V4 ()
{
    setColour (ResizableWindow::backgroundColourId, {11, 75, 93});

    setColour (Slider::rotarySliderFillColourId, {29, 39, 49});
    setColour (Slider::rotarySliderOutlineColourId, {217, 179, 16});

    setColour (TextEditor::backgroundColourId, {51, 100, 114});

    setColour (PopupMenu::backgroundColourId, {11, 75, 93});
    setColour (PopupMenu::highlightedBackgroundColourId, Colour (11, 75, 93).darker (0.6f));
    setColour (PopupMenu::highlightedTextColourId, Colours::white.darker (3.2f));
    setColour (PopupMenu::textColourId, Colours::white.darker (0.2f));

    setColour (ToggleButton::textColourId, Colours::white.darker (0.2f));

    auto typeface = Typeface::createSystemTypefaceFor (BinaryData::OpenSansCondensedLight_ttf, BinaryData::OpenSansCondensedLight_ttfSize);
    setDefaultSansSerifTypeface (typeface);


    auto typeface2 = Typeface::createSystemTypefaceFor (BinaryData::fontaudio_ttf, BinaryData::fontaudio_ttfSize);
    fontAudio = Font (typeface2);

    LookAndFeel::setDefaultLookAndFeel (this);
}

void LookAndFeel::drawCornerResizer (Graphics& g, int w, int h, bool isMouseOver, bool isMouseDragging)
{
    g.setColour ((isMouseOver || isMouseDragging) ? Colour {static_cast<uint8> (50), 140, 193, 0.5f} : Colour {static_cast<uint8> (29), 39, 49, 0.42f});

    const float lineThickness = std::min (w, h) * 0.14f;

    for (float i = 0.5f; i < 1.0f; i += 0.3f)
    {
        g.drawLine (w * i,
                    h + 1.0f,
                    w + 1.0f,
                    h * i,
                    lineThickness);
    }
}

void LookAndFeel::drawButtonBackground (Graphics& g, Button&, const Colour&,
                                        bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown)
{
    g.fillAll (Colours::white.withAlpha (uint8 (100 + (shouldDrawButtonAsDown * 10) + (shouldDrawButtonAsHighlighted * 10))));
}

void LookAndFeel::drawToggleButton (Graphics& g, ToggleButton& button,
                       bool shouldDrawButtonAsHighlighted, bool)
{
    if (shouldDrawButtonAsHighlighted || button.hasKeyboardFocus (true))
        g.fillAll (Colours::white.withAlpha (uint8 (12)));

    if (! button.isEnabled())
        g.setOpacity (0.5f);

    auto textColor = button.getToggleState()? button.findColour (ToggleButton::textColourId) :
                                              button.findColour (ToggleButton::textColourId).darker (0.8f);

    g.setColour (textColor);
    g.setFont (fontAudio);
    g.setFont (button.getHeight() * 0.72f);

    g.drawText (button.getButtonText(), button.getLocalBounds(), Justification::centred, false);
}

void LookAndFeel::drawComboBox (Graphics& g, int, int, bool isButtonDown, int, int, int, int, ComboBox& box)
{
    if (isButtonDown || box.hasKeyboardFocus (true))
        g.fillAll (Colours::white.withAlpha (uint8 (12)));
}

void LookAndFeel::positionComboBoxText (ComboBox& box, juce::Label& label)
{
    label.setBounds (box.getLocalBounds());
    label.setFont (getComboBoxFont (box));
}

void LookAndFeel::drawPopupMenuBackground (Graphics& g, int, int)
{
    g.fillAll (findColour (PopupMenu::backgroundColourId));
}

void LookAndFeel::drawPopupMenuItem (Graphics& g, const Rectangle<int>& area,
                                     bool isSeparator, bool isActive, bool isHighlighted,
                                     bool isTicked, bool hasSubMenu, const String& text,
                                     const String&, const Drawable*, const Colour*)
{
    if (isSeparator)
    {
        auto r = area.reduced (5, 0);
        r.removeFromTop (roundToInt ((r.getHeight() * 0.5f) - 0.5f));

        g.setColour (findColour (PopupMenu::textColourId).withAlpha (0.3f));
        g.fillRect (r.removeFromTop (1));

        return;
    }

    auto textColour = findColour (PopupMenu::textColourId);

    auto r = area.reduced (1);

    if (isHighlighted && isActive)
    {
        g.setColour (findColour (PopupMenu::highlightedBackgroundColourId));
        g.fillRect (r);
    }

    r.reduce (jmin (5, area.getWidth() / 20), 0);

    if (isTicked)
    {
        g.fillAll (findColour (PopupMenu::backgroundColourId).darker (0.2f));
    }

    if (hasSubMenu)
    {
        auto arrowH = 0.6f * getPopupMenuFont().getAscent();

        auto x = static_cast<float> (r.removeFromRight ((int) arrowH).getX());
        auto halfH = static_cast<float> (r.getCentreY());

        Path path;
        path.startNewSubPath (x, halfH - arrowH * 0.5f);
        path.lineTo (x + arrowH * 0.6f, halfH);
        path.lineTo (x, halfH + arrowH * 0.5f);

        g.strokePath (path, PathStrokeType (2.0f));
    }

    auto font = fontAudio;
    font.setHeight (area.getHeight() * 0.86);
    g.setFont (font);

    g.setColour (textColour);
    g.drawText (text, r, Justification::centred, false);
}

int LookAndFeel::getSliderThumbRadius (Slider&)
{
    return 0;
}

void LookAndFeel::drawLinearSlider (Graphics& g, int x, int y, int width, int height,
                                    float sliderPos, float, float, const Slider::SliderStyle, Slider& slider)
{
    auto fill    = slider.findColour (Slider::rotarySliderFillColourId);
    auto outline = slider.findColour (Slider::rotarySliderOutlineColourId);

    if (slider.hasKeyboardFocus (true))
    {
        g.fillAll (Colours::white.withAlpha (0.04f));

        fill = fill.brighter (0.01f);
        outline = outline.brighter (0.4f);
    }

    g.setColour (fill);
    g.fillRect (x, y, width, height);

    g.setColour (outline);

    Rectangle<float> rect = slider.isHorizontal()? Rectangle<float> (x, y, sliderPos - x, height)
                                                 : Rectangle<float> (x, sliderPos, width, y + (height - sliderPos));
    g.fillRect (rect.reduced (slider.isHorizontal()? height * 0.2f : width * 0.2f).toNearestIntEdges());
}

juce::Label* LookAndFeel::createSliderTextBox (Slider& slider)
{
    return new witte::Label (slider.getValueObject());
}

Slider::SliderLayout LookAndFeel::getSliderLayout (Slider& slider)
{
    auto bounds = slider.getLocalBounds();
    float w = bounds.getWidth();

    Slider::SliderLayout layout;

    if (slider.isVertical())
    {
        layout.textBoxBounds = bounds.removeFromBottom (w * 0.44f);
        bounds.removeFromTop (w * 0.18f);
        layout.sliderBounds = bounds.reduced (w * 0.42f, 0.0f);
    }

    return layout;
}

void LookAndFeel::drawLabel (Graphics& g, juce::Label& label)
{
    if (label.isBeingEdited()) return;

    auto color = Colours::white.darker (0.2f);
    if (!label.isEnabled()) color = color.darker (0.8f);

    g.setFont (getLabelFont (label));
    g.setColour (color);
    g.drawText (label.getText(), label.getLocalBounds(), label.getJustificationType(), false);
}

}
