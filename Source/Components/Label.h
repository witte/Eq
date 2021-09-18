#pragma once
#include <juce_gui_basics/juce_gui_basics.h>

#include "TextEditor.h"

using namespace juce;

namespace witte
{

class Label : public juce::Label
{
    public:
        Label (Value& _value, Range<double> _range);

        bool keyPressed (const KeyPress& key) override;

    protected:
        void editorShown (juce::TextEditor* labelEditor) override;

    private:
        Value& value;
        Range<double> range;
        juce::TextEditor* createEditorComponent() override;


        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Label)
};

}
