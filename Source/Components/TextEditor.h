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
        TextEditor (Value& refValue, int decimals, bool acceptsNegativeValues, String initialValue);
        ~TextEditor() override;

        String filterNewText (juce::TextEditor& ed, const String& newInput) override;

        void resized() override;

        void focusGained (FocusChangeType) override;

        std::function<bool (const KeyPress&)> onKeyPressed {nullptr};

        void clearCharacters();

    private:
        Value& value;
        String filterString {};
        int decimals {0};

        bool keyPressed (const KeyPress& key) override;
        void valueChanged (Value&) override;

        int lastSelectionStart {-1};
        int lastSelectionEnd   {-1};


        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TextEditor)
};

}
