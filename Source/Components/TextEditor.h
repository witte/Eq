#pragma once
#include <juce_gui_basics/juce_gui_basics.h>


namespace witte
{

class TextEditor : public juce::TextEditor,
                   public juce::TextEditor::InputFilter,
                   private juce::Value::Listener
{
    public:
        TextEditor (juce::Value& _value, juce::Range<double>& _range);
        ~TextEditor() override;

        juce::String filterNewText (juce::TextEditor& ed, const juce::String& newInput) override;

        void resized() override;

        void clearCharacters();


    private:
        juce::Value& value;
        juce::Range<double>& range;
        juce::String filterString {"-.0123456789"};

        void valueChanged (juce::Value&) override;

        int lastSelectionStart {-1};
        int lastSelectionEnd   {-1};


        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TextEditor)
};

}
