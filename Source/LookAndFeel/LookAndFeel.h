#pragma once
#include <BinaryData.h>
#include <juce_gui_basics/juce_gui_basics.h>


namespace witte
{


class LookAndFeel final : public juce::LookAndFeel_V4
{
    public:
        LookAndFeel();

        void drawCornerResizer (juce::Graphics&, int w, int h, bool isMouseOver, bool isMouseDragging) override;

        void fillTextEditorBackground (juce::Graphics&, int, int, juce::TextEditor&) override {}
        void drawTextEditorOutline    (juce::Graphics&, int, int, juce::TextEditor&) override {}

        juce::Font getTextButtonFont (juce::TextButton&, int buttonHeight) override
        {
            juce::Font font (fontAudio);
            font.setHeight (int (buttonHeight * 0.58f));

            return font;
        }

        juce::Font getComboBoxFont (juce::ComboBox& box) override
        {
            juce::Font font (fontAudio);
            font.setHeight (box.getHeight() * 0.82f);

            return font;
        }

        void drawButtonBackground (juce::Graphics&, juce::Button&, const juce::Colour& backgroundColour,
                                   bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown) override;

        void drawToggleButton (juce::Graphics&, juce::ToggleButton&,
                               bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown) override;

        void drawComboBox (juce::Graphics&, int width, int height, bool isButtonDown,
                           int buttonX, int buttonY, int buttonW, int buttonH,
                           juce::ComboBox&) override;

        void positionComboBoxText (juce::ComboBox&, juce::Label&) override;

        void drawPopupMenuBackground (juce::Graphics&, int width, int height) override;

        void drawPopupMenuItem (juce::Graphics&, const juce::Rectangle<int>& area,
                                bool isSeparator, bool isActive, bool isHighlighted, bool isTicked, bool hasSubMenu,
                                const juce::String& text, const juce::String& shortcutKeyText,
                                const juce::Drawable* icon, const juce::Colour* textColourToUse) override;

        int getSliderThumbRadius (juce::Slider&) override;

        juce::Slider::SliderLayout getSliderLayout (juce::Slider& slider) override;

        void drawLinearSlider (juce::Graphics&, int x, int y, int width, int height,
                               float sliderPos, float minSliderPos, float maxSliderPos,
                               const juce::Slider::SliderStyle, juce::Slider&) override;

        void drawRotarySlider (juce::Graphics&, int x, int y, int width, int height,
                               float sliderPosProportional, float rotaryStartAngle,
                               float rotaryEndAngle, juce::Slider&) override;

        juce::Label* createSliderTextBox (juce::Slider&) override;

        juce::Font getLabelFont (juce::Label& label) override;
        void drawLabel (juce::Graphics& g, juce::Label& label) override;


    private:
        juce::Font fontDefault{ juce::FontOptions{ juce::Typeface::createSystemTypefaceFor
            (BinaryData::OpenSansCondensedLight_ttf, BinaryData::OpenSansCondensedLight_ttfSize)} };
        juce::Font fontAudio{ juce::FontOptions{ juce::Typeface::createSystemTypefaceFor
            (BinaryData::fontaudio_ttf, BinaryData::fontaudio_ttfSize) } };


        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (LookAndFeel)
};

}
