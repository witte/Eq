#include "Label.h"
#include "TextEditor.h"

namespace witte
{

Label::Label (Value& _value) : value {_value}
{
    juce::Label();

    setJustificationType (Justification::centred);
}

void Label::resized()
{
    setFont (std::max (16.0f, getHeight() * 0.94f));

    juce::Label::resized();
}

TextEditor* Label::createEditorComponent()
{
    return new witte::TextEditor (value, 2, true, {});
}

}
