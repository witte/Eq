#include "BandEditor.h"


namespace witte
{


static juce::String getPrmName (const int bandNumber, const juce::String& paramName)
{
    juce::String str (bandNumber);
    str << paramName;
    return  str;
}

BandEditor::BandEditor (juce::AudioProcessorValueTreeState& _tree, const int bandNumber) : tree {_tree},
    attOn   {tree, getPrmName (bandNumber, "On"  ), on},
    attType {tree, getPrmName (bandNumber, "Type"), type},
    attFreq {tree, getPrmName (bandNumber, "Freq"), freq},
    attGain {tree, getPrmName (bandNumber, "Gain"), gain},
    attQ    {tree, getPrmName (bandNumber, "Q"   ), q}
{
    setPaintingIsUnclipped (true);

    freq.setSliderStyle  (juce::Slider::RotaryVerticalDrag);
    gain.setSliderStyle  (juce::Slider::RotaryVerticalDrag);
    q   .setSliderStyle  (juce::Slider::RotaryVerticalDrag);

    freq.setTextBoxStyle (juce::Slider::TextBoxLeft, false, 60, 28);
    gain.setTextBoxStyle (juce::Slider::TextBoxLeft, false, 60, 28);
    q   .setTextBoxStyle (juce::Slider::TextBoxLeft, false, 60, 28);

    freq.setMouseDragSensitivity (520);
    gain.setMouseDragSensitivity (520);
    q.setMouseDragSensitivity    (520);

    addAndMakeVisible (on);
    addAndMakeVisible (type);
    addAndMakeVisible (freq);
    addAndMakeVisible (gain);
    addAndMakeVisible (q);

    type.addItem ("1", 1);
    type.addItem ("2", 2);
    type.addItem ("3", 3);
    type.addItem ("4", 4);
    type.addItem ("5", 5);


    auto* lastType = tree.getRawParameterValue (getPrmName (bandNumber, "Type"));
    type.setSelectedId (int (*lastType + 1.0f));
    type.setJustificationType (juce::Justification::centred);
    type.onChange = [&] ()
    {
        gain.setEnabled (on.getToggleState() && type.getSelectedId() != 1 && type.getSelectedId() != 5);
    };

    on.onStateChange = [&] ()
    {
        const auto isOn = on.getToggleState();

        type.setEnabled (isOn);
        freq.setEnabled (isOn);
        gain.setEnabled (isOn && type.getSelectedId() != 1 && type.getSelectedId() != 5);
        q   .setEnabled (isOn);
    };

    on.onStateChange();
    type.onChange();
}

void BandEditor::resized()
{
    juce::FlexBox row;
    row.items.add (juce::FlexItem (type).withFlex (1.8f));
    row.items.add (juce::FlexItem (on)  .withFlex (1.0f));

    juce::FlexBox band;
    band.flexDirection = juce::FlexBox::Direction::column;

    band.items.add (juce::FlexItem (row) .withFlex (0.58f));
    band.items.add (juce::FlexItem (freq).withFlex (1.0f));
    band.items.add (juce::FlexItem (gain).withFlex (1.0f));
    band.items.add (juce::FlexItem (q)   .withFlex (1.0f));

    band.performLayout (getLocalBounds().reduced (3));
}

void BandEditor::paint (juce::Graphics& g)
{
    g.setColour (juce::Colour {0x2a000000});
    g.fillRoundedRectangle (getLocalBounds().toFloat(), 6);
}


}
