#include "XYPad.h"


namespace witte
{


XYPad::XYPad (std::initializer_list<std::pair<juce::RangedAudioParameter*, juce::RangedAudioParameter*>> parameters)
{
    setPaintingIsUnclipped (true);

    for (auto& [parameterX, parameterY] : parameters)
    {
        prmHandles.push_back (std::make_unique<XYPadHandle> (*this, parameterX, parameterY));
    }
}

XYPad::~XYPad() = default;

void XYPad::paint (juce::Graphics& g)
{
    bandsPositionsPath.clear();

    for (const auto& prmAttachment : prmHandles)
    {
        auto[x, y] = prmAttachment->getPos();

        bandsPositionsPath.addEllipse (x - handleRadius, y - handleRadius, handleRadius * 2.0f, handleRadius * 2.0f);
    }

    g.setColour (juce::Colour {0xff4ea5ac});
    g.fillPath (bandsPositionsPath);

    if (draggingPad != nullptr)
    {
        g.setColour (juce::Colour {0x80663399});

        auto[x, y] = draggingPad->getPos();
        g.fillEllipse (x - hoveredHandleRadius,    y - hoveredHandleRadius,
                       hoveredHandleRadius * 2.0f, hoveredHandleRadius * 2.0f);
    }
}

void XYPad::resized()
{
    for (const auto& handle : prmHandles)
    {
        handle->getX()->sendValueChangedMessageToListeners (handle->getX()->getValue());
        handle->getY()->sendValueChangedMessageToListeners (handle->getY()->getValue());
    }
}

void XYPad::mouseDown (const juce::MouseEvent&)
{
    if (draggingPad == nullptr) return;

    draggingPad->getX()->beginChangeGesture();
    draggingPad->getY()->beginChangeGesture();
}

void XYPad::mouseMove (const juce::MouseEvent& event)
{
    for (const auto& prmAttachment : prmHandles)
    {
        if (prmAttachment->getPos().getDistanceFrom (event.getPosition().toFloat()) < hoveredHandleRadius)
        {
            if (draggingPad != nullptr)
                return;

            draggingPad = prmAttachment.get();
            repaint();

            return;
        }
    }

    if (draggingPad != nullptr)
    {
        draggingPad = nullptr;
        repaint();
    }
}

void XYPad::mouseDrag (const juce::MouseEvent& event)
{
    if (draggingPad == nullptr) return;

    const auto bounds = getLocalBounds().toFloat();
    const auto width  = bounds.getWidth();
    const auto height = bounds.getHeight();

    draggingPad->getX()->setValueNotifyingHost (event.position.getX() / width);
    draggingPad->getY()->setValueNotifyingHost (1 - (event.position.getY() / height));

    repaint();
}

void XYPad::mouseUp (const juce::MouseEvent&)
{
    if (draggingPad == nullptr) return;

    draggingPad->getX()->endChangeGesture();
    draggingPad->getY()->endChangeGesture();
}


}
