#pragma once
#include "JuceHeader.h"
#include "Helpers/ParameterHelpers.h"

class EqAudioProcessor : public AudioProcessor
{
    public:
        EqAudioProcessor();

        const String getName()          const override { return JucePlugin_Name; }

        bool   hasEditor()              const override { return true;  }
        bool   acceptsMidi()            const override { return false; }
        bool   producesMidi()           const override { return false; }
        bool   isMidiEffect()           const override { return false; }
        int    getNumPrograms()               override { return 1;     }
        int    getCurrentProgram()            override { return 0;     }
        void   setCurrentProgram (int)        override {               }
        double getTailLengthSeconds()   const override { return 0.0;   }

        const String getProgramName    (int)                override { return {}; }
        void         changeProgramName (int, const String&) override {}

        bool isBusesLayoutSupported (const BusesLayout& layouts) const override
        {
            return (layouts.getMainInputChannelSet() == layouts.getMainOutputChannelSet());
        }

        void prepareToPlay (double sampleRate, int samplesPerBlock)  override;
        void processBlock (AudioBuffer<float>&, MidiBuffer&)         override;
        void releaseResources()                                      override {}

        void getStateInformation (MemoryBlock& destData)             override;
        void setStateInformation (const void* data, int sizeInBytes) override;

        AudioProcessorEditor* createEditor()                         override;

        Point<int> getSavedEditorSize() const            { return editorSize; }
        void setSavedEditorSize (const Point<int>& size) { editorSize = size; }

        enum
        {
            fftOrder  = 11,
            fftSize   = 1 << fftOrder,
        };
        std::atomic<bool> nextFFTBlockReady = false;

        AbstractFifo abstractFifoInput { 2 * fftSize };
        AudioBuffer<float> audioFifoInput;

        AbstractFifo abstractFifoOutput { 2 * fftSize };
        AudioBuffer<float> audioFifoOutput;


        struct Band : public AudioProcessorValueTreeState::Listener
        {
            Band (EqAudioProcessor& eqProcessor, int index) : eqProcessor {eqProcessor}, index {index} {}

            bool active {true};

            float* prm_on   {nullptr};
            float* prm_type {nullptr};
            float* prm_freq {nullptr};
            float* prm_gain {nullptr};
            float* prm_q    {nullptr};

            void parameterChanged (const String& parameter, float newValue) override;

            void updateFilter();

            int getIndex() { return index; }

            dsp::ProcessorDuplicator<dsp::IIR::Filter<float>, dsp::IIR::Coefficients<float>> processor;

            private:
                EqAudioProcessor& eqProcessor;
                int index {0};
        };

        Band band1;
        Band band2;
        Band band3;
        Band band4;
        Band band5;

        std::function<void (int)> onBandParametersChange;


    private:
        const StringArray filterTypes {"Low Cut", "Low Shelf", "Peak", "High Shelf", "High Cut"};

        AudioProcessorValueTreeState::ParameterLayout prmLayout
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
        };
        UndoManager                  undoManager;
        AudioProcessorValueTreeState parameters;

        void pushNextSampleToFifo (const AudioBuffer<float>& buffer, int startChannel, int numChannels,
                                   AbstractFifo& absFifo, AudioBuffer<float>& fifo);

        double sampleRate  {48000.0};

        Point<int> editorSize = { 768, 512 };


        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (EqAudioProcessor)
};
