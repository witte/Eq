#include "EqEditor.h"

namespace witte
{

EqAudioProcessorEditor::EqAudioProcessorEditor (EqAudioProcessor& p, AudioProcessorValueTreeState& vts)
    : AudioProcessorEditor {&p}, processor {p}, tree {vts},
      analyzer {processor},
      band1 {tree, 1},
      band2 {tree, 2},
      band3 {tree, 3},
      band4 {tree, 4},
      band5 {tree, 5},
      attOutputGain {vts, "OutGain", outputGain}
{
    setOpaque (true);
    setWantsKeyboardFocus (true);

    outputGain.setSliderStyle  (Slider::SliderStyle::LinearVertical);
    outputGain.setTextBoxStyle (Slider::TextBoxBelow, false, 62, 22);

    addAndMakeVisible (analyzer);
    addAndMakeVisible (band1);
    addAndMakeVisible (band2);
    addAndMakeVisible (band3);
    addAndMakeVisible (band4);
    addAndMakeVisible (band5);
    addAndMakeVisible (outputGain);
    
    auto size = processor.getSavedEditorSize();
    setResizable (true, true);
    setSize (size.x, size.y);

    int defaultW = 768;
    int defaultH = 482;

    setResizeLimits (defaultW / 2, defaultH / 2, defaultW * 2, defaultH * 2);

#ifdef JUCE_OPENGL
    openGLContext.attachTo (*getTopLevelComponent());
#endif
}

EqAudioProcessorEditor::~EqAudioProcessorEditor()
{
    setLookAndFeel (nullptr);

#ifdef JUCE_OPENGL
    openGLContext.detach();
#endif
}

void EqAudioProcessorEditor::paint (Graphics& g)
{
    g.fillAll (getLookAndFeel().findColour (ResizableWindow::backgroundColourId));
    g.setTiledImageFill (background, 0, 0, 0.4f);
    g.fillAll();
}

void EqAudioProcessorEditor::resized()
{
    auto bounds = getLocalBounds();
    processor.setSavedEditorSize ({ bounds.getWidth(), bounds.getHeight() });

    float bandsWidth = std::clamp (bounds.getHeight() * 1.16f, 384.0f, std::min (632.0f, float (bounds.getWidth())));
    float bandsHeight = bandsWidth * 0.24f;

    FlexBox bands;
    bands.items.add (FlexItem (band1).withFlex (1.0f).withMargin (1));
    bands.items.add (FlexItem (band2).withFlex (1.0f).withMargin (1));
    bands.items.add (FlexItem (band3).withFlex (1.0f).withMargin (1));
    bands.items.add (FlexItem (band4).withFlex (1.0f).withMargin (1));
    bands.items.add (FlexItem (band5).withFlex (1.0f).withMargin (1));
    bands.items.add (FlexItem (outputGain).withWidth (bandsWidth * 0.1f).withMargin (4));
    bands.performLayout (bounds.removeFromBottom (bandsHeight).reduced ((bounds.getWidth() - bandsWidth) * 0.5f, 1));

    analyzer.setBounds (bounds);
}

}
