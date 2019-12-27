#pragma once
#include "JuceHeader.h"

namespace witte
{
class Knob;

class TextEditor : public juce::TextEditor,
                   public juce::TextEditor::InputFilter,
                   private Value::Listener
{
    public:
        TextEditor (Value& refValue, int decimals, bool acceptsNegativeValues, String initialValue, bool initialValueSelected);
        ~TextEditor() override;

        void setText (const String& newText, bool selectAllText = false, bool sendTextChangeMessage = true);
        String filterNewText (juce::TextEditor& ed, const String& newInput) override;

        void resized() override;

        std::function<bool (const KeyPress&)> onKeyPressed {nullptr};


    private:
        Value& value;
        String filterString {};
        int decimals {0};

        bool keyPressed (const KeyPress& key) override;
        void valueChanged (Value& value) override;

        int lastSelectionStart {-1};
        int lastSelectionEnd   {-1};


        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TextEditor)
};

}
