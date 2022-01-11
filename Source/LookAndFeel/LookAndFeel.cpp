#include "LookAndFeel.h"
#include <BinaryData.h>
#include "../Components/Label.h"

namespace witte
{

LookAndFeel::LookAndFeel() : juce::LookAndFeel_V4 ()
{
    setColour (ResizableWindow::backgroundColourId,      Colour {0xff0b4b5d});

    setColour (Slider::rotarySliderFillColourId,         Colour {0xff1d2731});
    setColour (Slider::rotarySliderOutlineColourId,      Colour {0xffd9b310});

    setColour (TextEditor::backgroundColourId,           Colour {0xff336472});

    setColour (PopupMenu::backgroundColourId,            Colour {0xff0b4b5d});
    setColour (PopupMenu::highlightedBackgroundColourId, Colour {0xff062e3a});
    setColour (PopupMenu::highlightedTextColourId,       Colour {0xff3d3d3d});
    setColour (PopupMenu::textColourId,                  Colour {0xffd4d4d4});

    setColour (ToggleButton::textColourId,               Colour {0xffd4d4d4});

    fontDefault = Typeface::createSystemTypefaceFor (BinaryData::OpenSansCondensedLight_ttf, BinaryData::OpenSansCondensedLight_ttfSize);
    LookAndFeel::getDefaultLookAndFeel().setDefaultSansSerifTypeface (fontDefault.getTypefacePtr());

    fontAudio = Typeface::createSystemTypefaceFor (BinaryData::fontaudio_ttf, BinaryData::fontaudio_ttfSize);
}

void LookAndFeel::drawCornerResizer (Graphics& g, int w, int h, bool isMouseOver, bool isMouseDragging)
{
    g.setColour (Colour {(isMouseOver || isMouseDragging)? 0x80328cc1 : 0x681d2731});

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

void LookAndFeel::drawToggleButton (Graphics& g, ToggleButton& button, bool shouldDrawButtonAsHighlighted, bool)
{
    if (shouldDrawButtonAsHighlighted || button.hasKeyboardFocus (true))
        g.fillAll (Colour {0x0cffffff});

    if (! button.isEnabled())
        g.setOpacity (0.5f);

    auto textColor = button.getToggleState()? button.findColour (ToggleButton::textColourId) :
                                              button.findColour (ToggleButton::textColourId).darker (0.8f);

    g.setColour (textColor);
    g.setFont (fontAudio);
    g.setFont (button.getHeight() * 0.58f);

    g.drawText (button.getButtonText(), button.getLocalBounds(), Justification::centred, false);
}

void LookAndFeel::drawComboBox (Graphics& g, int, int, bool isButtonDown, int, int, int, int, ComboBox& box)
{
    if (isButtonDown || box.hasKeyboardFocus (true))
        g.fillAll (Colour {0x0cffffff});
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
    font.setHeight (area.getHeight() * 0.68f);
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
        g.fillAll (Colour {0x0affffff});

        fill = fill.brighter (0.01f);
        outline = outline.brighter (0.4f);
    }

    g.setColour (fill);
    g.fillRect (x, y, width, height);

    g.setColour (outline);

    Rectangle<float> rect = slider.isHorizontal()? Rectangle<float> (float (x), float (y), float (sliderPos - x), float (height))
                                                 : Rectangle<float> (float (x), float (sliderPos), float (width), float (y + (height - sliderPos)));
    g.fillRect (rect.reduced (slider.isHorizontal()? height * 0.2f : width * 0.2f).toNearestIntEdges());
}

void LookAndFeel::drawRotarySlider (Graphics& g, int x, int y, int width, int height,
                                    float sliderPosProportional, float rotaryStartAngle, float rotaryEndAngle, Slider& slider)
{
    auto outline = findColour (Slider::rotarySliderOutlineColourId);
    auto fill    = findColour (Slider::rotarySliderFillColourId);
    auto text    = Colour {0xffd4d4d4};
    auto toAngle = rotaryStartAngle + sliderPosProportional * (rotaryEndAngle - rotaryStartAngle);

    if (!slider.isEnabled())
    {
        fill = fill.darker (0.8f);
        outline = outline.darker (0.8f);
        text = text.darker (0.8f);
    }
    else if (slider.hasKeyboardFocus (true))
    {
        g.setColour (Colour {0x0affffff});
        g.fillAll();

        fill = fill.brighter (0.01f);
        outline = outline.brighter (0.4f);
    }

    g.setColour (fill);
    g.fillEllipse (float (x), float (y), float (width), float (height));

    auto arcBounds = Rectangle<float> (float (x), float (y), float (width), float (height))
                        .reduced (height * 0.12f);

    Path valueArc;
    valueArc.addArc (arcBounds.getX(),     arcBounds.getY(),
                     arcBounds.getWidth(), arcBounds.getHeight(),
                     rotaryStartAngle, toAngle, true);

    g.setColour (outline);
    g.strokePath (valueArc, PathStrokeType (height * 0.14f, PathStrokeType::curved, PathStrokeType::butt));
}

juce::Label* LookAndFeel::createSliderTextBox (Slider& slider)
{
    return new witte::Label (slider.getValueObject(), slider.getRange());
}

Font LookAndFeel::getLabelFont (juce::Label& label)
{
    Font font = label.getFont();
    font.setHeight (label.getHeight() * 0.86f);

    return font;
}

Slider::SliderLayout LookAndFeel::getSliderLayout (Slider& slider)
{
    auto bounds = slider.getLocalBounds();
    float w = float (bounds.getWidth());

    Slider::SliderLayout layout;

    if (slider.isVertical())
    {
        layout.textBoxBounds = bounds.removeFromBottom (int (w * 0.44f));
        bounds.removeFromTop (int (w * 0.18f));
        layout.sliderBounds = bounds.reduced (int (w * 0.42f), 0);
    }
    else if (slider.isHorizontal())
    {
        // TODO
    }
    else if (slider.isRotary())
    {
        float x = float (bounds.getX());
        float y = float (bounds.getY());
        float h = float (bounds.getHeight());
        float ratio = h * 3.0f;

        if (ratio < w)
        {
            x = (w * 0.5f) - (ratio * 0.5f);
            w = ratio;
        }
        else
        {
            ratio = w / 3.0f;
            y = (h * 0.5f) - (ratio * 0.5f);
            h = ratio;
        }

        layout.textBoxBounds = Rectangle<int> (int (x), int(y), int (w), int (h)).reduced (1);
        layout.sliderBounds  = layout.textBoxBounds.removeFromRight (int (h));
    }

    return layout;
}

void LookAndFeel::drawLabel (Graphics& g, juce::Label& label)
{
    if (label.isBeingEdited()) return;

    g.setFont (getLabelFont (label));
    g.setColour (Colour {label.isEnabled()? 0xffd4d4d4 : 0xff757575});
    g.drawText (label.getText(), label.getLocalBounds(), label.getJustificationType(), false);
}

}
