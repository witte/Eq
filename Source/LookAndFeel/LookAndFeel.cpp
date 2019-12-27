#include "LookAndFeel.h"

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

//void LookAndFeel::drawRotarySlider (Graphics& g, int x, int y, int width, int height, float sliderPos,
//                                    const float rotaryStartAngle, const float rotaryEndAngle, Slider& slider)
//{
//    auto outline = findColour (Slider::rotarySliderOutlineColourId);
//    auto fill    = findColour (Slider::rotarySliderFillColourId);

//    Rectangle<float> bounds (x, y, width, height);
//    DBG (bounds.toString());

////    const RotaryParameters& rp = getRotaryParameters();
////    auto toAngle = rp.startAngleRadians + valueToProportionOfLength (getValue()) *
////                   (rp.endAngleRadians - rp.startAngleRadians);

//    if (!slider.isEnabled())
//    {
//        fill = fill.darker (0.6);
//        outline = outline.darker (0.6);
//    }
//    else if (slider.hasKeyboardFocus (true))
//    {
//        g.setColour (Colours::white.withAlpha (0.04f));
////        g.fillRect (bounds);
//        g.fillAll ();

//        fill = fill.brighter (0.01);
//        outline = outline.brighter (0.4);
//    }

//    g.setColour (fill);
//    g.fillEllipse (bounds);

////    auto height = knobBounds.getHeight();
//    auto arcBounds = bounds.reduced (height * 0.12f);

//    Path valueArc;
//    valueArc.addArc (arcBounds.getX(),     arcBounds.getY(),
//                     arcBounds.getWidth(), arcBounds.getHeight(),
//                     rotaryStartAngle, sliderPos, true);

//    g.setColour (outline);
//    g.strokePath (valueArc, PathStrokeType (height * 0.14f, PathStrokeType::curved, PathStrokeType::butt));
//}

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

//void LookAndFeel::drawButtonBackground (Graphics& g, Button& button, const Colour& backgroundColour,
//                                        bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown)
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

void LookAndFeel::positionComboBoxText (ComboBox& box, Label& label)
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



//Slider::SliderLayout LookAndFeel::getSliderLayout (Slider& slider)
//{
//    auto bounds = slider.getBounds().toFloat();
//    DBG (bounds.toString());

//    float x = bounds.getX();
//    float y = bounds.getY();
//    float w = bounds.getWidth();
//    float h = bounds.getHeight();
//    float ratio = h * 3.0f;

//    if (ratio < w)
//    {
//        x = (w * 0.5f) - (ratio * 0.5f);
//        w = ratio;
//    }
//    else
//    {
//        ratio = w / 3.0f;
//        y = (h * 0.5f) - (ratio * 0.5f);
//        h = ratio;
//    }

//    Slider::SliderLayout layout;
//    layout.textBoxBounds = Rectangle<int> (x, y, w, h).reduced (1);
//    layout.sliderBounds = layout.textBoxBounds.removeFromRight (h);

//    return layout;
//}

void LookAndFeel::drawLabel (Graphics& g, Label& label)
{
    if (label.isBeingEdited()) return;

    auto color = Colours::white.darker (0.4f);
    if (!label.isEnabled()) color = color.darker (0.8f);

    g.setFont (getLabelFont (label));
    g.setColour (color);
    g.drawText (label.getText(), label.getLocalBounds(), label.getJustificationType(), false);
}

}
