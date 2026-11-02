#include "Parameters.h"

juce::AudioProcessorValueTreeState::ParameterLayout createOrbitParameterLayout()
{
    juce::AudioProcessorValueTreeState::ParameterLayout layout;
    layout.add (std::make_unique<juce::AudioParameterFloat> (
        juce::ParameterID { OrbitParameterIds::masterGain, 1 },
        "Master Gain",
        juce::NormalisableRange<float> { -60.0f, 6.0f },
        0.0f,
        "dB"));
    return layout;
}