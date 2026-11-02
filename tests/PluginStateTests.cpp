#include "PluginProcessor.h"

#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>

TEST_CASE ("Plugin parameter state survives save and restore")
{
    OrbitAudioProcessor source;
    source.getParameters().getParameter (OrbitParameterIds::masterGain)
        ->setValueNotifyingHost (0.25f);

    juce::MemoryBlock state;
    source.getStateInformation (state);

    OrbitAudioProcessor restored;
    restored.setStateInformation (state.getData(), static_cast<int> (state.getSize()));

    const auto restoredValue = restored.getParameters()
                                   .getRawParameterValue (OrbitParameterIds::masterGain)->load();
    REQUIRE (restoredValue == Catch::Approx (-43.5f).margin (0.01f));
}