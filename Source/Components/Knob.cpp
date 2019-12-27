#include "Knob.h"

namespace witte
{

Knob::Knob (int _decimals) : Slider(), decimals {_decimals}
{
    setSliderStyle  (Slider::RotaryVerticalDrag);
    setTextBoxStyle (Slider::TextBoxLeft, true, 0, 0);

    setWantsKeyboardFocus (true);
}

Knob::~Knob()
{
    closeEditor (false);
    setLookAndFeel (nullptr);
}

void Knob::mouseDown (const MouseEvent& event)
{
    lastEditorWasOpen = false;
    if (textEditor != nullptr) isOpeningEditor = true;

    Slider::mouseDown (event);
}

void Knob::mouseUp (const MouseEvent& event)
{
    if (event.mouseWasClicked() && isEnabled() && !isOpeningEditor)
        openEditor();

    Slider::mouseUp (event);
}

void Knob::focusGained (Component::FocusChangeType)
{
    if (lastEditorWasOpen)
        openEditor();
}

void Knob::resized()
{
    auto bounds = getLocalBounds().toFloat();

    float x = bounds.getX();
    float y = bounds.getY();
    float w = bounds.getWidth();
    float h = bounds.getHeight();
    float ratio = h * 3.0f;

    if (ratio < w)
    {
        x = (w * 0.5f) - (ratio * 0.5f);
        w = ratio;
    }
    else
    {
        ratio = w / 3.0f;
        y = (h * 0.5f) - (ratio * 0.5f);
        h = ratio;
    }

    editorBounds = Rectangle<float> (x, y, w, h).reduced (1);
    knobBounds   = editorBounds.removeFromRight (h);
}

void Knob::paint (Graphics& g)
{
    auto outline = findColour (Slider::rotarySliderOutlineColourId);
    auto fill    = findColour (Slider::rotarySliderFillColourId);
    auto text    = Colours::white.darker (0.2f);

    const RotaryParameters& rp = getRotaryParameters();
    auto toAngle = rp.startAngleRadians + valueToProportionOfLength (getValue()) *
                   (rp.endAngleRadians - rp.startAngleRadians);

    if (!isEnabled())
    {
        fill = fill.darker (0.8f);
        outline = outline.darker (0.8f);
        text = text.darker (0.8f);
    }
    else if (hasKeyboardFocus (true))
    {
        g.setColour (Colours::white.withAlpha (0.04f));
        g.fillRect (getLocalBounds());

        fill = fill.brighter (0.01f);
        outline = outline.brighter (0.4f);
    }

    g.setColour (fill);
    g.fillEllipse (knobBounds);

    auto height = knobBounds.getHeight();
    auto arcBounds = knobBounds.reduced (height * 0.12f);

    Path valueArc;
    valueArc.addArc (arcBounds.getX(),     arcBounds.getY(),
                     arcBounds.getWidth(), arcBounds.getHeight(),
                     rp.startAngleRadians, toAngle, true);

    g.setColour (outline);
    g.strokePath (valueArc, PathStrokeType (height * 0.14f, PathStrokeType::curved, PathStrokeType::butt));

    if (textEditor) return;

    g.setColour (text);
    g.setFont (height * 0.82f);

    g.drawText (getTextFromValue (getValue()), editorBounds, Justification::centred);
}

void Knob::textEditorFocusLost (juce::TextEditor&)
{
    closeEditor (true);
}

void Knob::textEditorEscapeKeyPressed (juce::TextEditor&)
{
    lastEditorWasOpen = false;
    closeEditor (false);
}

void Knob::textEditorReturnKeyPressed (juce::TextEditor&)
{
    lastEditorWasOpen = false;
    closeEditor (true);
}


bool Knob::keyPressed (const KeyPress& key)
{
    int code = key.getKeyCode();

    if (code == KeyPress::returnKey)
    {
        openEditor();
        return true;
    }
    else if (code == KeyPress::tabKey)
    {
        if (textEditor)
            closeEditor (true);

        return Slider::keyPressed (key);
    }
    else if (code == KeyPress::spaceKey)
    {
        return Slider::keyPressed (key);
    }
    else
    {
        String allowedCharacters {(getMinimum() < 0.0)? "-.0123456789" : ".0123456789"};
        String keyChar {key.getTextDescription ()};

        if (allowedCharacters.contains (keyChar))
        {
            openEditor (String {keyChar}, false);
            return true;
        }
        else
        {
            return Slider::keyPressed (key);
        }
    }
}

void Knob::openEditor (String initialValue, bool initialValueSelected)
{
    if (textEditor != nullptr) return;

    textEditor = std::make_unique<witte::TextEditor> (getValueObject(),
                                                      decimals,
                                                      getMinimum() < 0.0,
                                                      initialValue,
                                                      initialValueSelected);
    witte::TextEditor& ed = *textEditor;
    addAndMakeVisible (textEditor.get());
    ed.addListener (this);
    ed.setBounds (editorBounds.toNearestIntEdges());
    ed.grabKeyboardFocus();

    lastEditorWasOpen = true;
}

void Knob::closeEditor (bool updateValue)
{
    if (!textEditor) return;

    if (updateValue)
    {
        String value {textEditor->getText()};

        if (value.length() > 0)
            setValue (value.getDoubleValue());
    }

    textEditor->removeListener (this);
    removeChildComponent (textEditor.get());
    textEditor.reset ();
    repaint();
}

//==============================================================================
// We'll use a custom focus traverser here to make sure focus goes from the
// text editor to another component rather than back to the label itself.
//
// witte: code taken from the original juce::Label class.
class LabelKeyboardFocusTraverser   : public KeyboardFocusTraverser
{
public:
    LabelKeyboardFocusTraverser() {}

    Component* getNextComponent     (Component* c) override { return KeyboardFocusTraverser::getNextComponent (getComp (c)); }
    Component* getPreviousComponent (Component* c) override { return KeyboardFocusTraverser::getPreviousComponent (getComp (c)); }

    static Component* getComp (Component* current)
    {
        return dynamic_cast<witte::TextEditor*> (current) != nullptr? current->getParentComponent() : current;
    }
};

KeyboardFocusTraverser* Knob::createFocusTraverser()
{
    return new LabelKeyboardFocusTraverser();
}


}
