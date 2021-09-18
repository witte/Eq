#pragma once
#include <juce_gui_basics/juce_gui_basics.h>

using namespace juce;

namespace witte
{

class LookAndFeel : public LookAndFeel_V4
{
    public:
        LookAndFeel();

        void drawCornerResizer (Graphics&, int w, int h, bool isMouseOver, bool isMouseDragging) override;

        void fillTextEditorBackground (Graphics&, int, int, TextEditor&) override {}
        void drawTextEditorOutline    (Graphics&, int, int, TextEditor&) override {}

        Font getTextButtonFont (TextButton&, int buttonHeight) override
        {
            Font font (fontAudio);
            font.setHeight (int (buttonHeight * 0.58f));

            return font;
        }

        Font getComboBoxFont (ComboBox& box) override
        {
            Font font (fontAudio);
            font.setHeight (box.getHeight() * 0.82f);

            return font;
        }

        void drawButtonBackground (Graphics&, Button&, const Colour& backgroundColour,
                                   bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown) override;

        void drawToggleButton (Graphics&, ToggleButton&,
                               bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown) override;

        void drawComboBox (Graphics&, int width, int height, bool isButtonDown,
                           int buttonX, int buttonY, int buttonW, int buttonH,
                           ComboBox&) override;

        void positionComboBoxText (ComboBox&, Label&) override;

        void drawPopupMenuBackground (Graphics&, int width, int height) override;

        void drawPopupMenuItem (Graphics&, const Rectangle<int>& area,
                                bool isSeparator, bool isActive, bool isHighlighted, bool isTicked, bool hasSubMenu,
                                const String& text, const String& shortcutKeyText,
                                const Drawable* icon, const Colour* textColourToUse) override;

        int getSliderThumbRadius (Slider&) override;

        Slider::SliderLayout getSliderLayout (Slider& slider) override;

        void drawLinearSlider (Graphics&, int x, int y, int width, int height,
                               float sliderPos, float minSliderPos, float maxSliderPos,
                               const Slider::SliderStyle, Slider&) override;

        void drawRotarySlider (Graphics&, int x, int y, int width, int height,
                               float sliderPosProportional, float rotaryStartAngle,
                               float rotaryEndAngle, Slider&) override;

        Label* createSliderTextBox (Slider&) override;

        Font getLabelFont (Label& label) override;
        void drawLabel (Graphics& g, Label& label) override;


    private:
        Font fontDefault;
        Font fontAudio;


        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (LookAndFeel)
};

}
