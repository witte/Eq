#include "LookAndFeel.h"
#include <BinaryData.h>
#include "../Components/Label.h"


namespace witte
{

LookAndFeel::LookAndFeel()
{
    setColour (juce::ResizableWindow::backgroundColourId,      juce::Colour {0xff0b4b5d});

    setColour (juce::Slider::rotarySliderFillColourId,         juce::Colour {0xff1d2731});
    setColour (juce::Slider::rotarySliderOutlineColourId,      juce::Colour {0xffd9b310});

    setColour (juce::TextEditor::backgroundColourId,           juce::Colour {0xff336472});

    setColour (juce::PopupMenu::backgroundColourId,            juce::Colour {0xff0b4b5d});
    setColour (juce::PopupMenu::highlightedBackgroundColourId, juce::Colour {0xff062e3a});
    setColour (juce::PopupMenu::highlightedTextColourId,       juce::Colour {0xff3d3d3d});
    setColour (juce::PopupMenu::textColourId,                  juce::Colour {0xffd4d4d4});

    setColour (juce::ToggleButton::textColourId,               juce::Colour {0xffd4d4d4});

    getDefaultLookAndFeel().setDefaultSansSerifTypeface (fontDefault.getTypefacePtr());
}

void LookAndFeel::drawCornerResizer (juce::Graphics& g, const int w, const int h, const bool isMouseOver, const bool isMouseDragging)
{
    g.setColour (juce::Colour {(isMouseOver || isMouseDragging)? 0x80328cc1 : 0x681d2731});

    const auto width = static_cast<float>(w);
    const auto height = static_cast<float>(h);
    const float lineThickness = std::min (width, height) * 0.14f;

    float i = 0.5f;
    while (i < 1.0f)
    {
        g.drawLine (width * i,
                    height + 1.0f,
                    width + 1.0f,
                    height * i,
                    lineThickness);

        i += 0.3f;
    }
}

void LookAndFeel::drawButtonBackground (juce::Graphics& g, juce::Button&, const juce::Colour&,
                                        const bool shouldDrawButtonAsHighlighted, const bool shouldDrawButtonAsDown)
{
    g.fillAll (juce::Colours::white.withAlpha (
      static_cast<juce::uint8>(100 + (shouldDrawButtonAsDown * 10) +
                               (shouldDrawButtonAsHighlighted * 10))));
}

void LookAndFeel::drawToggleButton (juce::Graphics& g, juce::ToggleButton& button, const bool shouldDrawButtonAsHighlighted, bool)
{
    if (shouldDrawButtonAsHighlighted || button.hasKeyboardFocus (true))
        g.fillAll (juce::Colour {0x0cffffff});

    if (! button.isEnabled())
        g.setOpacity (0.5f);

    const auto textColor = button.getToggleState()? button.findColour (juce::ToggleButton::textColourId) :
                                                    button.findColour (juce::ToggleButton::textColourId).darker (0.8f);

    g.setColour (textColor);
    g.setFont (fontAudio);
    g.setFont (static_cast<float>(button.getHeight()) * 0.58f);

    g.drawText (button.getButtonText(), button.getLocalBounds(), juce::Justification::centred, false);
}

void LookAndFeel::drawComboBox (juce::Graphics& g, int, int, const bool isButtonDown, int, int, int, int, juce::ComboBox& box)
{
    if (isButtonDown || box.hasKeyboardFocus (true))
        g.fillAll (juce::Colour {0x0cffffff});
}

void LookAndFeel::positionComboBoxText (juce::ComboBox& box, juce::Label& label)
{
    label.setBounds (box.getLocalBounds());
    label.setFont (getComboBoxFont (box));
}

void LookAndFeel::drawPopupMenuBackground (juce::Graphics& g, int, int)
{
    g.fillAll (findColour (juce::PopupMenu::backgroundColourId));
}

void LookAndFeel::drawPopupMenuItem (juce::Graphics& g, const juce::Rectangle<int>& area,
                                     const bool isSeparator, const bool isActive, const bool isHighlighted,
                                     const bool isTicked, const bool hasSubMenu, const juce::String& text,
                                     const juce::String&, const juce::Drawable*, const juce::Colour*)
{
    if (isSeparator)
    {
        auto r = area.reduced (5, 0);
        r.removeFromTop (juce::roundToInt ((static_cast<float>(r.getHeight()) * 0.5f) - 0.5f));

        g.setColour (findColour (juce::PopupMenu::textColourId).withAlpha (0.3f));
        g.fillRect (r.removeFromTop (1));

        return;
    }

    const auto textColour = findColour (juce::PopupMenu::textColourId);

    auto r = area.reduced (1);

    if (isHighlighted && isActive)
    {
        g.setColour (findColour (juce::PopupMenu::highlightedBackgroundColourId));
        g.fillRect (r);
    }

    r.reduce (juce::jmin (5, area.getWidth() / 20), 0);

    if (isTicked)
    {
        g.fillAll (findColour (juce::PopupMenu::backgroundColourId).darker (0.2f));
    }

    if (hasSubMenu)
    {
        const auto arrowH = 0.6f * getPopupMenuFont().getAscent();

        const auto x = static_cast<float> (r.removeFromRight (static_cast<int>(arrowH)).getX());
        const auto halfH = static_cast<float> (r.getCentreY());

        juce::Path path;
        path.startNewSubPath (x, halfH - arrowH * 0.5f);
        path.lineTo (x + arrowH * 0.6f, halfH);
        path.lineTo (x, halfH + arrowH * 0.5f);

        g.strokePath (path, juce::PathStrokeType (2.0f));
    }

    auto font = fontAudio;
    font.setHeight (static_cast<float>(area.getHeight()) * 0.68f);
    g.setFont (font);

    g.setColour (textColour);
    g.drawText (text, r, juce::Justification::centred, false);
}

int LookAndFeel::getSliderThumbRadius (juce::Slider&)
{
    return 0;
}

void LookAndFeel::drawLinearSlider (juce::Graphics& g, const int x, const int y, const int width, const int height,
                                    const float sliderPos, float, float, const juce::Slider::SliderStyle, juce::Slider& slider)
{
    auto fill    = slider.findColour (juce::Slider::rotarySliderFillColourId);
    auto outline = slider.findColour (juce::Slider::rotarySliderOutlineColourId);

    if (slider.hasKeyboardFocus (true))
    {
        g.fillAll (juce::Colour {0x0affffff});

        fill = fill.brighter (0.01f);
        outline = outline.brighter (0.4f);
    }

    g.setColour (fill);
    g.fillRect (x, y, width, height);

    g.setColour (outline);

    const auto fx = static_cast<float>(x);
    const auto fy = static_cast<float>(y);
    const auto fw = static_cast<float>(width);
    const auto fh = static_cast<float>(height);
    const juce::Rectangle<float> rect = slider.isHorizontal()? juce::Rectangle (fx, fy, sliderPos - fx, fh)
                                                             : juce::Rectangle (fx, sliderPos, fw, fy + (fh - sliderPos));
    g.fillRect (rect.reduced (slider.isHorizontal()? fh * 0.2f : fw * 0.2f).toNearestIntEdges());
}

void LookAndFeel::drawRotarySlider (juce::Graphics& g, const int x, const int y, const int width, const int height,
                                    const float sliderPosProportional, const float rotaryStartAngle, const float rotaryEndAngle, juce::Slider& slider)
{
    auto outline = findColour (juce::Slider::rotarySliderOutlineColourId);
    auto fill    = findColour (juce::Slider::rotarySliderFillColourId);
    const auto toAngle = rotaryStartAngle + sliderPosProportional * (rotaryEndAngle - rotaryStartAngle);

    if (!slider.isEnabled())
    {
        fill = fill.darker (0.8f);
        outline = outline.darker (0.8f);
    }
    else if (slider.hasKeyboardFocus (true))
    {
        g.setColour (juce::Colour {0x0affffff});
        g.fillAll();

        fill = fill.brighter (0.01f);
        outline = outline.brighter (0.4f);
    }

    const auto fx = static_cast<float>(x);
    const auto fy = static_cast<float>(y);
    const auto fw = static_cast<float>(width);
    const auto fh = static_cast<float>(height);

    g.setColour (fill);
    g.fillEllipse (fx, fy, fw, fh);

    const auto arcBounds = juce::Rectangle (fx, fy, fw, fh).reduced (fh * 0.12f);

    juce::Path valueArc;
    valueArc.addArc (arcBounds.getX(),     arcBounds.getY(),
                     arcBounds.getWidth(), arcBounds.getHeight(),
                     rotaryStartAngle, toAngle, true);

    g.setColour (outline);
    g.strokePath (valueArc, juce::PathStrokeType (fh * 0.14f, juce::PathStrokeType::curved, juce::PathStrokeType::butt));
}

juce::Label* LookAndFeel::createSliderTextBox (juce::Slider& slider)
{
    return new witte::Label (slider.getValueObject(), slider.getRange());
}

juce::Font LookAndFeel::getLabelFont (juce::Label& label)
{
    juce::Font font = label.getFont();
    font.setHeight (static_cast<float>(label.getHeight()) * 0.86f);

    return font;
}

juce::Slider::SliderLayout LookAndFeel::getSliderLayout (juce::Slider& slider)
{
    auto bounds = slider.getLocalBounds();
    auto w = static_cast<float>(bounds.getWidth());

    juce::Slider::SliderLayout layout;

    if (slider.isVertical())
    {
        layout.textBoxBounds = bounds.removeFromBottom (static_cast<int>(w * 0.44f));
        bounds.removeFromTop (static_cast<int>(w * 0.18f));
        layout.sliderBounds = bounds.reduced (static_cast<int>(w * 0.42f), 0);
    }
    else if (slider.isHorizontal())
    {
        // TODO
    }
    else if (slider.isRotary())
    {
        auto x = static_cast<float>(bounds.getX());
        auto y = static_cast<float>(bounds.getY());
        auto h = static_cast<float>(bounds.getHeight());

        if (auto ratio = h * 3.0f; ratio < w)
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

        layout.textBoxBounds = juce::Rectangle (static_cast<int>(x), static_cast<int>(y), static_cast<int>(w), static_cast<int>(h)).reduced (1);
        layout.sliderBounds  = layout.textBoxBounds.removeFromRight (static_cast<int>(h));
    }

    return layout;
}

void LookAndFeel::drawLabel (juce::Graphics& g, juce::Label& label)
{
    if (label.isBeingEdited()) return;

    g.setFont (getLabelFont (label));
    g.setColour (juce::Colour {label.isEnabled()? 0xffd4d4d4 : 0xff757575});
    g.drawText (label.getText(), label.getLocalBounds(), label.getJustificationType(), false);
}

}
