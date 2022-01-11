#pragma once
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_gui_basics/juce_gui_basics.h>


namespace witte
{

class XYPad : public juce::Component
{
    public:
        XYPad (std::initializer_list<std::pair<juce::RangedAudioParameter*, juce::RangedAudioParameter*>> parameters);
        ~XYPad() override;

        void paint (juce::Graphics&) override;
        void resized() override;


    private:
        class XYPadHandle
        {
            public:
                XYPadHandle (XYPad& _pad, juce::RangedAudioParameter* _parameterX, juce::RangedAudioParameter* _parameterY) : pad {_pad}
                {
                    parameterX = _parameterX;
                    parameterY = _parameterY;

                    attachmentX = std::make_unique<juce::ParameterAttachment>(*parameterX, prmChangedCallback);
                    attachmentY = std::make_unique<juce::ParameterAttachment>(*parameterY, prmChangedCallback);
                }

                juce::Point<float>& getPos() { return pos; }

                juce::RangedAudioParameter* getX() { return parameterX; }
                juce::RangedAudioParameter* getY() { return parameterY; }


            private:
                std::function<void(float)> prmChangedCallback = [&] (float)
                {
                    pos.setX (parameterX->getValue() * pad.getWidth());
                    pos.setY (pad.getHeight() - (parameterY->getValue() * pad.getHeight()));

                    pad.repaint();
                };

                XYPad& pad;

                juce::Point<float> pos;

                juce::RangedAudioParameter* parameterX {nullptr};
                juce::RangedAudioParameter* parameterY {nullptr};

                std::unique_ptr<juce::ParameterAttachment> attachmentX {nullptr};
                std::unique_ptr<juce::ParameterAttachment> attachmentY {nullptr};
        };

        std::vector<std::unique_ptr<XYPadHandle>> prmHandles;
        XYPadHandle* draggingPad {nullptr};


        constexpr static float handleRadius = 2.5f;
        constexpr static float hoveredHandleRadius = handleRadius * 5.0f;
        const juce::Colour baseColor {0xff011523};

        juce::Path bandsPositionsPath;
        juce::CriticalSection freqPathCreationLock;

        void mouseDown (const juce::MouseEvent& event) override;
        void mouseMove (const juce::MouseEvent& event) override;
        void mouseDrag (const juce::MouseEvent& event) override;
        void mouseUp   (const juce::MouseEvent&) override;


        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (XYPad)
};

}
