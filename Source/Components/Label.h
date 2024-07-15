#pragma once
#include <juce_gui_basics/juce_gui_basics.h>


namespace witte
{


class Label final : public juce::Label
{
    public:
        Label (juce::Value& _value, juce::Range<double> _range);

        bool keyPressed (const juce::KeyPress& key) override;


    protected:
        void editorShown (juce::TextEditor* labelEditor) override;


    private:
        juce::Value& value;
        juce::Range<double> range;
        juce::TextEditor* createEditorComponent() override;


        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Label)
};


}
