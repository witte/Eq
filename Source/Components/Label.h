#pragma once
#include "JuceHeader.h"
#include "TextEditor.h"

namespace witte
{

class Label : public juce::Label
{
    public:
        Label (Value& _value);

        void resized() override;


    private:
        Value& value;
        TextEditor* createEditorComponent() override;


        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Label)
};

}
