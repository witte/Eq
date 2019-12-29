#pragma once
#include "JuceHeader.h"
#include "TextEditor.h"

namespace witte
{
    static bool lastEditorWasOpen;

class Knob : public juce::Slider, private juce::TextEditor::Listener
{
    public:
        Knob (int _decimals = 0);
        ~Knob() override;

        void mouseDown (const MouseEvent& event) override;
        void mouseUp   (const MouseEvent& event) override;

        void focusGained (FocusChangeType) override;

        void resized() override;

        void paint (Graphics& g) override;


    protected:
        KeyboardFocusTraverser* createFocusTraverser() override;


    private:
        Rectangle<float> knobBounds;
        Rectangle<float> editorBounds;
        std::unique_ptr<witte::TextEditor> textEditor {nullptr};
        bool isOpeningEditor {false};

        void textEditorFocusLost        (juce::TextEditor& ed) override;
        void textEditorEscapeKeyPressed (juce::TextEditor& ed) override;
        void textEditorReturnKeyPressed (juce::TextEditor& ed) override;

        bool keyPressed (const KeyPress& key) override;

        void openEditor (String initialValue = {});
        void closeEditor (bool updateValue = true);

        int decimals {0};


        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Knob)
};

}
