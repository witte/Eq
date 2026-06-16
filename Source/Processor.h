#pragma once
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_dsp/juce_dsp.h>
#include "Helpers/ParameterHelpers.h"
#include "Band.h"


class Processor final : public juce::AudioProcessor, public juce::AudioProcessorValueTreeState::Listener
{
    public:
        Processor();

        const juce::Identifier idOutputGain {"OutGain"};

        const juce::String getName()    const override { return JucePlugin_Name; }

        bool   hasEditor()              const override { return true;  }
        bool   acceptsMidi()            const override { return false; }
        bool   producesMidi()           const override { return false; }
        bool   isMidiEffect()           const override { return false; }
        int    getNumPrograms()               override { return 1;     }
        int    getCurrentProgram()            override { return 0;     }
        void   setCurrentProgram (int)        override {               }
        double getTailLengthSeconds()   const override { return 0.0;   }

        const juce::String getProgramName    (int)                      override { return "Default"; }
        void               changeProgramName (int, const juce::String&) override {}

        void prepareToPlay (double _sampleRate, int samplesPerBlock) override;
        void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&)   override;
        void releaseResources()                                      override {}

        void getStateInformation (juce::MemoryBlock& destData)       override;
        void setStateInformation (const void* data, int sizeInBytes) override;

        juce::Point<int> getSavedEditorSize() const            { return editorSize; }
        void setSavedEditorSize (const juce::Point<int>& size) { editorSize = size; }

        std::atomic<float>* prmOutputGain {nullptr};
        void parameterChanged (const juce::String&, float newValue) override;

        void setCopyToFifo (bool _copyToFifo);

        enum
        {
            fftOrder  = 11,
            fftSize   = 1 << fftOrder,
        };
        std::atomic<bool> nextFFTBlockReady {false};

        juce::AbstractFifo abstractFifoInput {1};
        juce::AudioBuffer<float> audioFifoInput;

        juce::AbstractFifo abstractFifoOutput {1};
        juce::AudioBuffer<float> audioFifoOutput;

        const std::array<Band, 5>& getBands() { return bands; }

        juce::AudioProcessorValueTreeState& getVTS() { return parameters; }


    protected:
        bool isBusesLayoutSupported (const BusesLayout& layouts) const override
        {
            for (const auto& bus : layouts.outputBuses)
            {
                if (bus.getChannelTypes().size() == 0)
                    return false;
            }

            return (layouts.getMainInputChannelSet() == layouts.getMainOutputChannelSet());
        }


    private:
        juce::AudioProcessorEditor* createEditor() override;


        const juce::StringArray filterTypes {"Low Cut", "Low Shelf", "Peak", "High Shelf", "High Cut"};

        juce::AudioProcessorValueTreeState::ParameterLayout prmLayout
        {
            witte::makePrmBool   ("1On",   "1 On",                     false, "On"       ),
            witte::makePrmChoice ("1Type", "1 Type", filterTypes,          0, "Type"     ),
            witte::makePrmFreq   ("1Freq", "1 Freq",                   20.0f, "Frequency"),
            witte::makePrmDb     ("1Gain", "1 Gain",                    0.0f, "Gain"     ),
            witte::makePrmFloat  ("1Q",    "1 Q",    0.1f, 10.0f, 1.0f, 0.7f, "Q"        ),

            witte::makePrmBool   ("2On",   "2 On",                      true, "On"       ),
            witte::makePrmChoice ("2Type", "2 Type", filterTypes,          1, "Type"     ),
            witte::makePrmFreq   ("2Freq", "2 Freq",                  200.0f, "Frequency"),
            witte::makePrmDb     ("2Gain", "2 Gain",                    0.0f, "Gain"     ),
            witte::makePrmFloat  ("2Q",    "2 Q",    0.1f, 10.0f, 1.0f, 1.0f, "Q"        ),

            witte::makePrmBool   ("3On",   "3 On",                      true, "On"),
            witte::makePrmChoice ("3Type", "3 Type", filterTypes,          2, "Type"     ),
            witte::makePrmFreq   ("3Freq", "3 Freq",                 1000.0f, "Frequency"),
            witte::makePrmDb     ("3Gain", "3 Gain",                    0.0f, "Gain"     ),
            witte::makePrmFloat  ("3Q",    "3 Q",    0.1f, 10.0f, 1.0f, 1.0f, "Q"        ),

            witte::makePrmBool   ("4On",   "4 On",                      true, "On"),
            witte::makePrmChoice ("4Type", "4 Type", filterTypes,          3, "Type"     ),
            witte::makePrmFreq   ("4Freq", "4 Freq",                 5000.0f, "Frequency"),
            witte::makePrmDb     ("4Gain", "4 Gain",                    0.0f, "Gain"     ),
            witte::makePrmFloat  ("4Q",    "4 Q",    0.1f, 10.0f, 1.0f, 1.0f, "Q"        ),

            witte::makePrmBool   ("5On",   "5 On",                     false, "On"),
            witte::makePrmChoice ("5Type", "5 Type", filterTypes,          4, "Type"     ),
            witte::makePrmFreq   ("5Freq", "5 Freq",                20000.0f, "Frequency"),
            witte::makePrmDb     ("5Gain", "5 Gain",                    0.0f, "Gain"     ),
            witte::makePrmFloat  ("5Q",    "5 Q",    0.1f, 10.0f, 1.0f, 0.7f, "Q"        ),

            witte::makePrmDb     ("OutGain", "Output Gain", 0.0f, "OutGain")
        };
        juce::UndoManager                  undoManager;
        juce::AudioProcessorValueTreeState parameters;

        std::array<Band, 5> bands;

        std::atomic<bool> copyToFifo {false};
        void pushNextSampleToFifo (const juce::AudioBuffer<float>& buffer, int startChannel, int numChannels,
                                   juce::AbstractFifo& absFifo, juce::AudioBuffer<float>& fifo);

        double sampleRate  {48000.0};

        juce::Point<int> editorSize = { 768, 512 };


        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Processor)
};
