#pragma once
#include <juce_gui_basics/juce_gui_basics.h>

using namespace juce;

namespace witte
{

class TextEditor : public juce::TextEditor,
                   public juce::TextEditor::InputFilter,
                   private Value::Listener
{
    public:
        TextEditor (Value& _value, Range<double>& _range);
        ~TextEditor() override;

        String filterNewText (juce::TextEditor& ed, const String& newInput) override;

        void resized() override;

        void clearCharacters();


    private:
        Value& value;
        Range<double>& range;
        String filterString {"-.0123456789"};

        void valueChanged (Value&) override;

        int lastSelectionStart {-1};
        int lastSelectionEnd   {-1};


        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TextEditor)
};

}
