#include "Label.h"
#include "TextEditor.h"


namespace witte
{


Label::Label (juce::Value& _value, const juce::Range<double> _range) : value {_value}, range {_range}
{
    setJustificationType (juce::Justification::centred);
}

juce::TextEditor* Label::createEditorComponent()
{
    // Not a leak, no worries
    auto* labelEditor = new TextEditor (value, range);

    return labelEditor;
}

bool Label::keyPressed (const juce::KeyPress& key)
{
    if (const auto code = key.getKeyCode(); code == juce::KeyPress::returnKey)
    {
        showEditor();

        return true;
    }

    if (const juce::String allowedCharacters {range.getStart() < 0.0? "-.0123456789" : ".0123456789"};
        allowedCharacters.containsChar (key.getTextCharacter()))
    {
        showEditor();

        const double valueAsDouble = value.getValue();

        juce::String keyChar;
        if (valueAsDouble < 0.0 && key.getTextCharacter() != '-')
            keyChar << "-";

        keyChar << key.getTextCharacter();

        getCurrentTextEditor()->setText (keyChar, false);

        return true;
    }

    return juce::Label::keyPressed (key);
}

void Label::editorShown (juce::TextEditor* labelEditor)
{
    const double valueAsDouble = value.getValue();
    const juce::String str(valueAsDouble, 2);

    auto& editor = dynamic_cast<TextEditor&> (*labelEditor);
    editor.clearCharacters();
    editor.setText(str);
    editor.setHighlightedRegion ({str.contains ("-")? 1 : 0, str.length()});
}


}
