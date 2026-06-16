#include "Band.h"
#include "Processor.h"


Band::Band(Processor &eqProcessor, const int index)
    : idOn{std::to_string(index) + "On"},
      idType{std::to_string(index) + "Type"},
      idFreq{std::to_string(index) + "Freq"},
      idGain{std::to_string(index) + "Gain"},
      idQ{std::to_string(index) + "Q"},
      eqProcessor{eqProcessor},
      index{index}
{
}

Band::~Band() = default;

void Band::parameterChanged (const juce::String& parameter, const float newValue)
{
    const auto str = parameter.substring (1, parameter.length());

    if      (str == "On"  ) *prmOn   = newValue;
    else if (str == "Type") *prmType = newValue;
    else if (str == "Freq") *prmFreq = newValue;
    else if (str == "Gain") *prmGain = newValue;
    else                    *prmQ    = newValue;

    if (str != "On")
        updateFilter();

    active = *prmOn > 0.5f && (*prmGain == 0.0f? (*prmType == 0.0f || *prmType == 4.0f) : true);
}

void Band::updateFilter() const
{
    switch (static_cast<int>(*prmType))
    {
        case 0: *dspProcessor.state = *juce::dsp::IIR::Coefficients<float>::makeHighPass (eqProcessor.getSampleRate(),
                                   *prmFreq, *prmQ);
            break;
        case 1: *dspProcessor.state = *juce::dsp::IIR::Coefficients<float>::makeLowShelf (eqProcessor.getSampleRate(),
                                   *prmFreq, *prmQ, juce::Decibels::decibelsToGain (prmGain->load()));
            break;
        case 2: *dspProcessor.state = *juce::dsp::IIR::Coefficients<float>::makePeakFilter (eqProcessor.getSampleRate(),
                                   *prmFreq, *prmQ, juce::Decibels::decibelsToGain (prmGain->load()));
            break;
        case 3: *dspProcessor.state = *juce::dsp::IIR::Coefficients<float>::makeHighShelf (eqProcessor.getSampleRate(),
                                   *prmFreq, *prmQ, juce::Decibels::decibelsToGain (prmGain->load()));
            break;
        case 4: *dspProcessor.state = *juce::dsp::IIR::Coefficients<float>::makeLowPass (eqProcessor.getSampleRate(),
                                   *prmFreq, *prmQ);
            break;
        default:
            break;
    }
}
