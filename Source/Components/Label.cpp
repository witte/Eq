#include "Label.h"
#include "TextEditor.h"

namespace witte
{

Label::Label (Value& _value, Range<double> _range) : value {_value}, range {_range}
{
    juce::Label();

    setJustificationType (Justification::centred);
}

TextEditor* Label::createEditorComponent()
{
    auto* editor = new witte::TextEditor (value, range);

    return editor;
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

void Label::editorShown (juce::TextEditor* editor)
{
    auto& ed = dynamic_cast<witte::TextEditor&> (*editor);

    ed.clearCharacters();

    String str = ed.getText();
    ed.setHighlightedRegion ({str.contains ("-")? 1 : 0, str.length()});
}

}
