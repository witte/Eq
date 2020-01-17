#pragma once
#include "JuceHeader.h"
#include "TextEditor.h"

namespace witte
{

class Label : public juce::Label
{
    public:
        Label (Value& _value, Range<double> _range);

        bool keyPressed (const KeyPress& key) override;

    protected:
        void editorShown (juce::TextEditor*) override;

    private:
        Value& value;
        Range<double> range;
        TextEditor* createEditorComponent() override;


        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Label)
};

}
