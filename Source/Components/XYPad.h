#pragma once
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_gui_basics/juce_gui_basics.h>


namespace witte
{


class XYPad final : public juce::Component
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

                [[nodiscard]] juce::RangedAudioParameter* getX() const { return parameterX; }
                [[nodiscard]] juce::RangedAudioParameter* getY() const { return parameterY; }


            private:
                XYPad& pad;
                juce::Point<float> pos;
                juce::RangedAudioParameter* parameterX {nullptr};
                juce::RangedAudioParameter* parameterY {nullptr};

                std::unique_ptr<juce::ParameterAttachment> attachmentX {nullptr};
                std::unique_ptr<juce::ParameterAttachment> attachmentY {nullptr};

                std::function<void(float)> prmChangedCallback = [&] (float)
                {
                    const auto padWidth  = static_cast<float> (pad.getWidth());
                    const auto padHeight = static_cast<float> (pad.getHeight());

                    pos.setX (parameterX->getValue() * padWidth);
                    pos.setY (padHeight - (parameterY->getValue() * padHeight));

                    pad.repaint();
                };
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
