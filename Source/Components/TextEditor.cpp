#include "TextEditor.h"
#include "Knob.h"

namespace witte
{

TextEditor::TextEditor (Value& refValue, int _decimals = 0, bool acceptsNegativeValues = true, String initialValue = {}) :
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

    if (! initialValue.isEmpty())
        setText (initialValue);
    else
        valueChanged (value);
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

void TextEditor::focusGained (Component::FocusChangeType)
{
    clearCharacters();

    lastSelectionStart = value < 0.0? 1 : 0;
    lastSelectionEnd = getText().length();

    setHighlightedRegion ({lastSelectionStart, lastSelectionEnd});
}

void TextEditor::clearCharacters()
{
    setText (getText().removeCharacters (" abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ!?/*+"), false);
}

bool TextEditor::keyPressed (const KeyPress& key)
{
    if ((onKeyPressed != nullptr) && onKeyPressed (key))
        return true;

    return juce::TextEditor::keyPressed (key);
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

void TextEditor::valueChanged (Value&)
{
    setText (String (static_cast<double> (value.getValue()), decimals), true);
    focusGained (FocusChangeType::focusChangedDirectly);
}


}
