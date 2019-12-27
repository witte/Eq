#include "TextEditor.h"
#include "Knob.h"

namespace witte
{

TextEditor::TextEditor (Value& refValue, int _decimals = 0, bool acceptsNegativeValues = true, String initialValue = {},
                        bool initialValueSelected = true) :
    juce::TextEditor(), value {refValue}, decimals {_decimals}
{
    setJustification (Justification::centred);
    value.addListener (this);
    setInputFilter (this, false);
    filterString = acceptsNegativeValues? "-.0123456789" : ".0123456789";

    onTextChange = [&] ()
    {
        if (lastSelectionStart >= 0)
        {
            setHighlightedRegion ({lastSelectionStart, lastSelectionEnd});
            lastSelectionStart = -1;
            lastSelectionEnd   = -1;
        }
    };

    if (initialValue.isEmpty())
    {
        initialValue = String (static_cast<double> (refValue.getValue()), decimals);
    }

    setText (initialValue, initialValueSelected, true);
}

TextEditor::~TextEditor()
{
    value.removeListener (this);
    setLookAndFeel (nullptr);
}

void TextEditor::resized()
{
    applyFontToAllText (Font (std::max (16.0f, getHeight() * 0.94f)), true);

    juce::TextEditor::resized();
}

bool TextEditor::keyPressed (const KeyPress& key)
{
    if ((onKeyPressed != nullptr) && onKeyPressed (key))
        return true;

    return juce::TextEditor::keyPressed (key);
}

void TextEditor::setText (const String& newText, bool selectAllText, bool sendTextChangeMessage)
{
    if (selectAllText)
    {
        lastSelectionStart = value < 0.0? 1 : 0;
        lastSelectionEnd = newText.length();
    }

    juce::TextEditor::setText (newText, sendTextChangeMessage);
}

String TextEditor::filterNewText (juce::TextEditor&, const String& newInput)
{
    if (!newInput.containsAnyOf (filterString))
    {
        if (newInput.length() > 0)
        {
            lastSelectionStart = getHighlightedRegion().getStart();
            lastSelectionEnd   = getHighlightedRegion().getEnd();
            setHighlightedRegion ({0, 0});
        }

        return {};
    }

    String value = getText();

    if (value.contains (".") && newInput.contains (".") && getHighlightedRegion().isEmpty()) return {};

    int pos = getCaretPosition();
    if (newInput.contains ("-"))
    {
        bool hasMinus = value.contains ("-");
        int posShift = hasMinus? -1 : 1;

        lastSelectionStart = getHighlightedRegion().getStart() + posShift;
        lastSelectionEnd   = getHighlightedRegion().getEnd()   + posShift;

        setText (hasMinus? value.replace ("-", "") : "-" + value);

        return {};
    }
    else
    {
        if (value.contains ("-") && pos == 0) lastSelectionStart = 1;
    }

    return newInput;
}

void TextEditor::valueChanged (Value& value)
{
    setText (String (static_cast<double> (value.getValue()), decimals), true, true);
}


}
