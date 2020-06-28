#include "XYPad.h"

namespace witte
{

XYPad::XYPad (std::initializer_list<std::pair<RangedAudioParameter*, RangedAudioParameter*>> parameters)
{
    setPaintingIsUnclipped (true);

    for (auto& parameterPair : parameters)
    {
        prmHandles.push_back (std::make_unique<XYPadHandle> (*this, parameterPair.first, parameterPair.second));
    }
}

XYPad::~XYPad()
{
}

void XYPad::paint (Graphics& g)
{
    bandsPositionsPath.clear();

    for (auto& prmAttachment : prmHandles)
    {
        auto[x, y] = prmAttachment->getPos();

        bandsPositionsPath.addEllipse (x - handleRadius, y - handleRadius, handleRadius * 2.0f, handleRadius * 2.0f);
    }

    g.setColour (Colour {0xff4ea5ac});
    g.fillPath (bandsPositionsPath);

    if (draggingPad != nullptr)
    {
        g.setColour (Colours::rebeccapurple.withAlpha (0.5f));

        auto[x, y] = draggingPad->getPos();
        g.fillEllipse (x - hoveredHandleRadius,    y - hoveredHandleRadius,
                       hoveredHandleRadius * 2.0f, hoveredHandleRadius * 2.0f);
    }
}

void XYPad::resized()
{
    for (auto& handle : prmHandles)
    {
        handle->getX()->sendValueChangedMessageToListeners (handle->getX()->getValue());
        handle->getY()->sendValueChangedMessageToListeners (handle->getY()->getValue());
    }
}

void XYPad::mouseDown (const MouseEvent&)
{
    if (draggingPad == nullptr) return;

    draggingPad->getX()->beginChangeGesture();
    draggingPad->getY()->beginChangeGesture();
}

void XYPad::mouseMove (const MouseEvent& event)
{
    for (auto& prmAttachment : prmHandles)
    {
        if (prmAttachment->getPos().getDistanceFrom (event.getPosition().toFloat()) < hoveredHandleRadius)
        {
            draggingPad = prmAttachment.get();
            repaint();

            return;
        }
    }

    draggingPad = nullptr;
}

void XYPad::mouseDrag (const MouseEvent& event)
{
    if (draggingPad == nullptr) return;

    const auto bounds = getLocalBounds().toFloat();
    const auto width  = bounds.getWidth();
    const auto height = bounds.getHeight();

    draggingPad->getX()->setValueNotifyingHost (event.position.getX() / width);
    draggingPad->getY()->setValueNotifyingHost (1 - (event.position.getY() / height));

    repaint();
}

void XYPad::mouseUp (const MouseEvent&)
{
    if (draggingPad == nullptr) return;

    draggingPad->getX()->endChangeGesture();
    draggingPad->getY()->endChangeGesture();
}

}
