#pragma once
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_gui_basics/juce_gui_basics.h>

using namespace juce;

namespace witte
{

class XYPad : public Component
{
    public:
        XYPad (std::initializer_list<std::pair<RangedAudioParameter*, RangedAudioParameter*>> parameters);
        ~XYPad() override;

        void paint (Graphics&) override;
        void resized() override;

        float handleRadius = 12.0f;


    private:
        class XYPadHandle
        {
            public:
                XYPadHandle (XYPad& _pad, RangedAudioParameter* _parameterX, RangedAudioParameter* _parameterY) : pad {_pad}
                {
                    parameterX = _parameterX;
                    parameterY = _parameterY;

                    attachmentX = std::make_unique<ParameterAttachment>(*parameterX, prmChangedCallback);
                    attachmentY = std::make_unique<ParameterAttachment>(*parameterY, prmChangedCallback);
                }

                Point<float>& getPos() { return pos; }

                RangedAudioParameter* getX() { return parameterX; }
                RangedAudioParameter* getY() { return parameterY; }


            private:
                std::function<void(float)> prmChangedCallback = [&] (float)
                {
                    pos.setX (parameterX->getValue() * pad.getWidth());
                    pos.setY (pad.getHeight() - (parameterY->getValue() * pad.getHeight()));

                    pad.repaint();
                };

                XYPad& pad;

                Point<float> pos;

                RangedAudioParameter* parameterX {nullptr};
                RangedAudioParameter* parameterY {nullptr};

                std::unique_ptr<ParameterAttachment> attachmentX {nullptr};
                std::unique_ptr<ParameterAttachment> attachmentY {nullptr};
        };

        std::vector<std::unique_ptr<XYPadHandle>> prmHandles;
        XYPadHandle* draggingPad {nullptr};


        Colour baseColor {0xff011523};

        Path bandsPositionsPath;
        CriticalSection freqPathCreationLock;

        int movingBand {-1};
        int hoveringBand {-1};

        void mouseDown (const MouseEvent& event) override;
        void mouseMove (const MouseEvent& event) override;
        void mouseDrag (const MouseEvent& event) override;
        void mouseUp   (const MouseEvent&) override;


        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (XYPad)
};

}
