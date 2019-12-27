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
    freq.setMouseDragSensitivity (520);
    gain.setMouseDragSensitivity (520);
    q.setMouseDragSensitivity    (520);

    addAndMakeVisible (on);
    addAndMakeVisible (type);
    addAndMakeVisible (freq);
    addAndMakeVisible (gain);
    addAndMakeVisible (q);

    type.addItem (String::fromUTF8 (u8"\uf132"), 1);
    type.addItem (String::fromUTF8 (u8"\uf138"), 2);
    type.addItem (String::fromUTF8 (u8"\uf130"), 3);
    type.addItem (String::fromUTF8 (u8"\uf137"), 4);
    type.addItem (String::fromUTF8 (u8"\uf133"), 5);

    auto* lastType = tree.getRawParameterValue (getPrmName (bandNumber, "Type"));
    type.setSelectedId (int (*lastType + 1.0f));
    type.setJustificationType (Justification::centred);

    on.onStateChange = [&] ()
    {
        bool isOn = on.getToggleState();

        type.setEnabled (isOn);
        freq.setEnabled (isOn);
        gain.setEnabled (isOn);
        q   .setEnabled (isOn);
    };

    on.onStateChange();
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
    g.setColour (Colours::black.withAlpha (uint8 (42)));
    g.fillRoundedRectangle (getLocalBounds().toFloat(), 6);
}

}