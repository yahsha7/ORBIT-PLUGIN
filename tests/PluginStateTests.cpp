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

TEST_CASE ("Plugin accepts mono and stereo output layouts")
{
    OrbitAudioProcessor processor;
    auto layouts = processor.getBusesLayout();

    REQUIRE (processor.isBusesLayoutSupported (layouts));

    layouts.outputBuses.set (0, juce::AudioChannelSet::mono());
    REQUIRE (processor.isBusesLayoutSupported (layouts));

    layouts.outputBuses.set (0, juce::AudioChannelSet::stereo());
    REQUIRE (processor.isBusesLayoutSupported (layouts));

    layouts.outputBuses.set (0, juce::AudioChannelSet::quadraphonic());
    REQUIRE_FALSE (processor.isBusesLayoutSupported (layouts));
}
