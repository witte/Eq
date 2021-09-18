#include "Label.h"
#include "TextEditor.h"

namespace witte
{

Label::Label (Value& _value, Range<double> _range) : value {_value}, range {_range}
{
    juce::Label();

    setJustificationType (Justification::centred);
}

juce::TextEditor* Label::createEditorComponent()
{
    auto* labelEditor = new witte::TextEditor (value, range);

    return labelEditor;
}

bool Label::keyPressed (const KeyPress& key)
{
    int code = key.getKeyCode();

    if (code == KeyPress::returnKey)
    {
        showEditor();

        return true;
    }
    else
    {
        String allowedCharacters {range.getStart() < 0.0? "-.0123456789" : ".0123456789"};
        String keyChar;
        keyChar << key.getTextCharacter();

        if (allowedCharacters.contains (keyChar))
        {
            showEditor();
            getCurrentTextEditor()->setText (keyChar, false);

            return true;
        }
    }

    return juce::Label::keyPressed (key);
}

void Label::editorShown (juce::TextEditor* labelEditor)
{
    auto& ed = dynamic_cast<witte::TextEditor&> (*labelEditor);

    ed.clearCharacters();

    String str = ed.getText();
    ed.setHighlightedRegion ({str.contains ("-")? 1 : 0, str.length()});
}

}
