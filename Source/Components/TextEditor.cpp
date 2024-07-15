#include "TextEditor.h"


namespace witte
{


TextEditor::TextEditor (juce::Value& _value, juce::Range<double>& _range) : value {_value}, range {_range}
{
    setJustification (juce::Justification::centred);
    value.addListener (this);
    setInputFilter (this, false);
    filterString = range.getStart() < 0.0? "-.0123456789" : ".0123456789";

    onTextChange = [&] ()
    {
        if (lastSelectionStart >= 0)
        {
            setHighlightedRegion ({lastSelectionStart, lastSelectionEnd});
            lastSelectionStart = -1;
            lastSelectionEnd   = -1;
        }
    };
}

TextEditor::~TextEditor()
{
    value.removeListener (this);
    setLookAndFeel (nullptr);
}

void TextEditor::resized()
{
    const juce::FontOptions options {std::max (16.0f, static_cast<float>(getHeight()) * 0.86f) };
    applyFontToAllText (juce::Font (options), true);

    juce::TextEditor::resized();
}

void TextEditor::clearCharacters()
{
    setText (getText().removeCharacters (" abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ!?/*+"), false);
}

juce::String TextEditor::filterNewText (juce::TextEditor&, const juce::String& newInput)
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

    const juce::String currentValue = getText();

    if (currentValue.contains (".") && newInput.contains (".") && getHighlightedRegion().isEmpty()) return {};

    const auto pos = getCaretPosition();
    if (newInput.contains ("-"))
    {
        const auto hasMinus = currentValue.contains ("-");
        const auto posShift = hasMinus? -1 : 1;

        lastSelectionStart = getHighlightedRegion().getStart() + posShift;
        lastSelectionEnd   = getHighlightedRegion().getEnd()   + posShift;

        setText (hasMinus? currentValue.replace ("-", "") : "-" + currentValue);

        return {};
    }

    if (currentValue.contains ("-") && pos == 0) lastSelectionStart = 1;

    return newInput;
}

void TextEditor::valueChanged (juce::Value& newValue)
{
    setText (newValue.toString(), true);
    clearCharacters();
}


}
