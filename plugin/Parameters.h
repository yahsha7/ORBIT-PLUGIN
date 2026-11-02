#pragma once

#include <juce_audio_processors/juce_audio_processors.h>

namespace OrbitParameterIds
{
inline constexpr auto masterGain = "master.gain";
}

juce::AudioProcessorValueTreeState::ParameterLayout createOrbitParameterLayout();