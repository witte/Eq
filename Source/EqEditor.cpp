#include "EqEditor.h"
#include <BinaryData.h>


namespace witte
{


EqAudioProcessorEditor::EqAudioProcessorEditor (EqAudioProcessor& p, juce::AudioProcessorValueTreeState& vts)
    : AudioProcessorEditor {&p},
      processor {p},
      tree {vts},
      background {juce::ImageCache::getFromMemory (BinaryData::texture_png, BinaryData::texture_pngSize)},
      frame {defaultWidth * 2, defaultHeight * 2},
      analyzer {processor},
      frequencyCurve {p},
      xyPad
      {{
          { tree.getParameter("1Freq"), tree.getParameter("1Gain") },
          { tree.getParameter("2Freq"), tree.getParameter("2Gain") },
          { tree.getParameter("3Freq"), tree.getParameter("3Gain") },
          { tree.getParameter("4Freq"), tree.getParameter("4Gain") },
          { tree.getParameter("5Freq"), tree.getParameter("5Gain") },
      }},
      band1 {tree, 1},
      band2 {tree, 2},
      band3 {tree, 3},
      band4 {tree, 4},
      band5 {tree, 5},
      attOutputGain {vts, "OutGain", outputGain}
{
    setOpaque (true);
    setPaintingIsUnclipped (true);
    setWantsKeyboardFocus (true);

    outputGain.setSliderStyle (juce::Slider::SliderStyle::LinearVertical);
    outputGain.setTextBoxStyle (juce::Slider::TextBoxLeft, false, 62, 22);

    addAndMakeVisible (frame);
        frame.addAndMakeVisible (analyzer);
        frame.addAndMakeVisible (frequencyCurve);
        frame.addAndMakeVisible (xyPad);
    addAndMakeVisible (band1);
    addAndMakeVisible (band2);
    addAndMakeVisible (band3);
    addAndMakeVisible (band4);
    addAndMakeVisible (band5);
    addAndMakeVisible (outputGain);

    setResizable (true, true);
    const auto size = processor.getSavedEditorSize();
    setSize (size.x, size.y);
    setResizeLimits (defaultWidth / 2, defaultHeight / 2,
        defaultWidth * 2, defaultHeight * 2);

    setLookAndFeel (&lookAndFeel);

    processor.setCopyToFifo (true);
}

EqAudioProcessorEditor::~EqAudioProcessorEditor()
{
    setLookAndFeel (nullptr);
    processor.setCopyToFifo (false);
}

void EqAudioProcessorEditor::paint (juce::Graphics& g)
{
    g.fillAll(juce::Colour {0xff000000});

    g.setColour(getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));
    g.fillRect (0, frame.getBottom(), getWidth(), getHeight() - frame.getBottom());
    g.setTiledImageFill (background, 0, 0, 0.6f);
    g.fillRect (0, frame.getBottom(), getWidth(), getHeight() - frame.getBottom());
}

void EqAudioProcessorEditor::resized()
{
    auto bounds = getLocalBounds();
    const auto width = static_cast<float> (bounds.getWidth());
    const auto height = static_cast<float> (bounds.getHeight());
    processor.setSavedEditorSize ({ bounds.getWidth(), bounds.getHeight() });

    const auto bandsWidth = std::clamp (height * 1.16f, 384.0f, std::min (632.0f, width));
    const auto bandsHeight = juce::roundToInt (bandsWidth * 0.24f);

    juce::FlexBox bands;
    bands.items.add (juce::FlexItem (band1).withFlex (1.0f).withMargin (1));
    bands.items.add (juce::FlexItem (band2).withFlex (1.0f).withMargin (1));
    bands.items.add (juce::FlexItem (band3).withFlex (1.0f).withMargin (1));
    bands.items.add (juce::FlexItem (band4).withFlex (1.0f).withMargin (1));
    bands.items.add (juce::FlexItem (band5).withFlex (1.0f).withMargin (1));
    bands.items.add (juce::FlexItem (outputGain).withWidth (bandsWidth * 0.1f).withMargin (4));

    bands.performLayout (bounds.removeFromBottom (bandsHeight)
                               .reduced (juce::roundToInt ((width - bandsWidth) * 0.5f), 1));

    frame.setBounds (bounds);
        analyzer.setBounds (bounds);
        frequencyCurve.setBounds (bounds);
        xyPad.setBounds (bounds);
}


}
