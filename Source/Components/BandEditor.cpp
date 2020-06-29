#include "BandEditor.h"

namespace witte
{

static String getPrmName (int bandNumber, const String& paramName)
{
    String str (bandNumber);
    str << paramName;
    return  str;
}

BandEditor::BandEditor (AudioProcessorValueTreeState& _tree, int bandNumber) : tree {_tree},
    attOn   {tree, getPrmName (bandNumber, "On"  ), on},
    attType {tree, getPrmName (bandNumber, "Type"), type},
    attFreq {tree, getPrmName (bandNumber, "Freq"), freq},
    attGain {tree, getPrmName (bandNumber, "Gain"), gain},
    attQ    {tree, getPrmName (bandNumber, "Q"   ), q}
{
    setPaintingIsUnclipped (true);

    freq.setSliderStyle  (Slider::RotaryVerticalDrag);
    gain.setSliderStyle  (Slider::RotaryVerticalDrag);
    q   .setSliderStyle  (Slider::RotaryVerticalDrag);

    freq.setTextBoxStyle (Slider::TextBoxLeft, false, 60, 28);
    gain.setTextBoxStyle (Slider::TextBoxLeft, false, 60, 28);
    q   .setTextBoxStyle (Slider::TextBoxLeft, false, 60, 28);

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
    type.setJustificationType (Justification::centred);
    type.onChange = [&] ()
    {
        gain.setEnabled (on.getToggleState() && type.getSelectedId() != 1 && type.getSelectedId() != 5);
    };

    on.onStateChange = [&] ()
    {
        bool isOn = on.getToggleState();

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
    FlexBox row;
    row.items.add (FlexItem (type).withFlex (1.8f));
    row.items.add (FlexItem (on)  .withFlex (1.0f));

    FlexBox band;
    band.flexDirection = FlexBox::Direction::column;

    band.items.add (FlexItem (row) .withFlex (0.58f));
    band.items.add (FlexItem (freq).withFlex (1.0f));
    band.items.add (FlexItem (gain).withFlex (1.0f));
    band.items.add (FlexItem (q)   .withFlex (1.0f));

    band.performLayout (getLocalBounds().reduced (3));
}

void BandEditor::paint (Graphics& g)
{
    g.setColour (Colour {0x2a000000});
    g.fillRoundedRectangle (getLocalBounds().toFloat(), 6);
}

}
