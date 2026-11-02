#include "PluginProcessor.h"

namespace
{
class TestOscillatorSound final : public juce::SynthesiserSound
{
public:
    bool appliesToNote (int) override { return true; }
    bool appliesToChannel (int) override { return true; }
};

class TestOscillatorVoice final : public juce::SynthesiserVoice
{
public:
    TestOscillatorVoice()
    {
        envelope.setParameters ({ 0.005f, 0.01f, 1.0f, 0.08f });
    }

    bool canPlaySound (juce::SynthesiserSound* sound) override
    {
        return dynamic_cast<TestOscillatorSound*> (sound) != nullptr;
    }

    void startNote (int midiNoteNumber, float velocity,
                    juce::SynthesiserSound*, int) override
    {
        phase = 0.0;
        level = velocity;
        phaseIncrement = juce::MathConstants<double>::twoPi
                       * juce::MidiMessage::getMidiNoteInHertz (midiNoteNumber)
                       / getSampleRate();
        envelope.noteOn();
    }

    void stopNote (float, bool allowTailOff) override
    {
        if (allowTailOff)
        {
            envelope.noteOff();
            return;
        }

        envelope.reset();
        clearCurrentNote();
    }

    void pitchWheelMoved (int) override {}
    void controllerMoved (int, int) override {}

    void renderNextBlock (juce::AudioBuffer<float>& output, int startSample,
                          int numberOfSamples) override
    {
        if (phaseIncrement <= 0.0)
            return;

        for (int sample = 0; sample < numberOfSamples; ++sample)
        {
            const auto value = static_cast<float> (std::sin (phase))
                             * level * envelope.getNextSample();
            phase += phaseIncrement;

            for (int channel = 0; channel < output.getNumChannels(); ++channel)
                output.addSample (channel, startSample + sample, value);

            if (! envelope.isActive())
            {
                clearCurrentNote();
                break;
            }
        }
    }

    void setCurrentPlaybackSampleRate (double sampleRate) override
    {
        juce::SynthesiserVoice::setCurrentPlaybackSampleRate (sampleRate);
        envelope.setSampleRate (sampleRate);
    }

private:
    juce::ADSR envelope;
    double phase = 0.0;
    double phaseIncrement = 0.0;
    float level = 0.0f;
};
}

OrbitAudioProcessor::OrbitAudioProcessor()
    : AudioProcessor (BusesProperties().withOutput ("Output", juce::AudioChannelSet::stereo(), true)),
      parameters (*this, nullptr, "ORBIT_STATE", createOrbitParameterLayout())
{
    masterGainParameter = parameters.getRawParameterValue (OrbitParameterIds::masterGain);

    for (int voice = 0; voice < 64; ++voice)
        synth.addVoice (new TestOscillatorVoice());

    synth.addSound (new TestOscillatorSound());
}

void OrbitAudioProcessor::prepareToPlay (double sampleRate, int maximumBlockSize)
{
    juce::ignoreUnused (maximumBlockSize);
    synth.setCurrentPlaybackSampleRate (sampleRate);
    masterGain.reset (sampleRate, 0.02);
    masterGain.setCurrentAndTargetValue (
        juce::Decibels::decibelsToGain (masterGainParameter->load()));
}

void OrbitAudioProcessor::releaseResources()
{
    synth.allNotesOff (0, false);
}

bool OrbitAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
    return layouts.getMainInputChannelSet().isDisabled()
        && layouts.getMainOutputChannelSet() == juce::AudioChannelSet::stereo();
}

void OrbitAudioProcessor::processBlock (juce::AudioBuffer<float>& audio, juce::MidiBuffer& midi)
{
    juce::ScopedNoDenormals noDenormals;
    audio.clear();
    synth.renderNextBlock (audio, midi, 0, audio.getNumSamples());

    masterGain.setTargetValue (juce::Decibels::decibelsToGain (masterGainParameter->load()));
    for (int channel = 0; channel < audio.getNumChannels(); ++channel)
        audio.applyGainRamp (channel, 0, audio.getNumSamples(),
                             masterGain.getNextValue(), masterGain.getTargetValue());

    for (int sample = 1; sample < audio.getNumSamples(); ++sample)
        masterGain.skip (1);
}

juce::AudioProcessorEditor* OrbitAudioProcessor::createEditor()
{
    return new juce::GenericAudioProcessorEditor (*this);
}

bool OrbitAudioProcessor::hasEditor() const { return true; }
const juce::String OrbitAudioProcessor::getName() const { return JucePlugin_Name; }
bool OrbitAudioProcessor::acceptsMidi() const { return true; }
bool OrbitAudioProcessor::producesMidi() const { return false; }
bool OrbitAudioProcessor::isMidiEffect() const { return false; }
double OrbitAudioProcessor::getTailLengthSeconds() const { return 0.0; }
int OrbitAudioProcessor::getNumPrograms() { return 1; }
int OrbitAudioProcessor::getCurrentProgram() { return 0; }
void OrbitAudioProcessor::setCurrentProgram (int index) { juce::ignoreUnused (index); }
const juce::String OrbitAudioProcessor::getProgramName (int index)
{
    juce::ignoreUnused (index);
    return {};
}
void OrbitAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
    juce::ignoreUnused (index, newName);
}

void OrbitAudioProcessor::getStateInformation (juce::MemoryBlock& destination)
{
    const auto state = parameters.copyState();
    if (const auto xml = state.createXml())
        copyXmlToBinary (*xml, destination);
}

void OrbitAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    if (const auto xml = getXmlFromBinary (data, sizeInBytes))
    {
        if (xml->hasTagName (parameters.state.getType().toString()))
            parameters.replaceState (juce::ValueTree::fromXml (*xml));
    }
}

juce::AudioProcessorValueTreeState& OrbitAudioProcessor::getParameters() noexcept
{
    return parameters;
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new OrbitAudioProcessor();
}