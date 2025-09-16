#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
AudioPluginAudioProcessor::AudioPluginAudioProcessor()
    : AudioProcessor(BusesProperties()
        .withInput("Input", juce::AudioChannelSet::stereo(), true)
        .withOutput("Output", juce::AudioChannelSet::stereo(), true)),
    apvts(*this, nullptr, "Parameters", createParams()) {
    driveParam = apvts.getRawParameterValue("drive");
    mixParam = apvts.getRawParameterValue("mix");
    outputParam = apvts.getRawParameterValue("output");
    reverbMixParam = apvts.getRawParameterValue("reverbMix");
    reverbRoomParam = apvts.getRawParameterValue("reverbRoom");
    reverbDampingParam = apvts.getRawParameterValue("reverbDamping");
    reverbWidthParam = apvts.getRawParameterValue("reverbWidth");
    reverbFreezeParam = apvts.getRawParameterValue("reverbFreeze");
    chainSwapParam = apvts.getRawParameterValue("chainOrderSwap");
    masterDryWetParam = apvts.getRawParameterValue("masterDryWet");
}

juce::AudioProcessorValueTreeState::ParameterLayout AudioPluginAudioProcessor::createParams() {
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        "GAIN", "Gain", juce::NormalisableRange<float>(0.0f, 1.0f), 0.5f));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        "drive", "Drive", juce::NormalisableRange<float>(1.0f, 30.0f, 0.1f), 1.0f));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        "mix", "Mix", juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f), 1.0f));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        "output", "Output", juce::NormalisableRange<float>(-24.0f, 6.0f, 0.1f), 0.0f));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        "reverbMix", "Reverb Mix", juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f), 0.2f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        "reverbRoom", "Reverb Room", juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f), 0.5f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        "reverbDamping", "Reverb Damping", juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f), 0.5f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        "reverbWidth", "Reverb Width", juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f), 1.0f));
    params.push_back(std::make_unique<juce::AudioParameterBool>(
        "reverbFreeze", "Reverb Freeze", false));

    params.push_back(std::make_unique<juce::AudioParameterBool>(
        "chainOrderSwap", "Swap Order (Reverb -> Distortion)", false));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        "masterDryWet", "Master Dry/Wet", juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f), 1.0f));

    return { params.begin(), params.end() };
}

AudioPluginAudioProcessor::~AudioPluginAudioProcessor()
{
}

//==============================================================================
const juce::String AudioPluginAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool AudioPluginAudioProcessor::acceptsMidi() const
{
#if JucePlugin_WantsMidiInput
    return true;
#else
    return false;
#endif
}

bool AudioPluginAudioProcessor::producesMidi() const
{
#if JucePlugin_ProducesMidiOutput
    return true;
#else
    return false;
#endif
}

bool AudioPluginAudioProcessor::isMidiEffect() const
{
#if JucePlugin_IsMidiEffect
    return true;
#else
    return false;
#endif
}

double AudioPluginAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int AudioPluginAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
    // so this should be at least 1, even if you're not really implementing programs.
}

int AudioPluginAudioProcessor::getCurrentProgram()
{
    return 0;
}

void AudioPluginAudioProcessor::setCurrentProgram(int index)
{
    juce::ignoreUnused(index);
}

const juce::String AudioPluginAudioProcessor::getProgramName(int index)
{
    juce::ignoreUnused(index);
    return {};
}

void AudioPluginAudioProcessor::changeProgramName(int index, const juce::String& newName)
{
    juce::ignoreUnused(index, newName);
}

//==============================================================================
void AudioPluginAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
    juce::ignoreUnused(samplesPerBlock);
    reverb.reset();
}

void AudioPluginAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

bool AudioPluginAudioProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
{
#if JucePlugin_IsMidiEffect
    juce::ignoreUnused(layouts);
    return true;
#else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
        && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
#if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
#endif

    return true;
#endif
}

void AudioPluginAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer,
    juce::MidiBuffer& midiMessages)
{
    juce::ignoreUnused(midiMessages);

    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    const float drive = *driveParam;
    const float distMix = *mixParam;
    const float outputGain = juce::Decibels::decibelsToGain(outputParam->load());
    const float revMix = *reverbMixParam;
    const float revRoom = *reverbRoomParam;
    const float revDamping = *reverbDampingParam;
    const float revWidth = *reverbWidthParam;
    const bool revFreeze = (*reverbFreezeParam) > 0.5f;
    const bool swap = (*chainSwapParam) > 0.5f; // false: Dist->Rev, true: Rev->Dist




    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    // This is here to avoid people getting screaming feedback
    // when they first compile a plugin, but obviously you don't need to keep
    // this code if your algorithm always overwrites all the output channels.
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear(i, 0, buffer.getNumSamples());

    // This is the place where you'd normally do the guts of your plugin's
    // audio processing...
    // Make sure to reset the state if your inner loop is processing
    // the samples and the outer loop is handling the channels.
    // Alternatively, you can process the samples with the channels
    // interleaved by keeping the same state.
    juce::Reverb::Parameters rp;
    rp.dryLevel = 1.0f - revMix;
    rp.wetLevel = revMix;
    rp.roomSize = revRoom;
    rp.damping = revDamping;
    rp.width = revWidth;
    rp.freezeMode = revFreeze ? 1.0f : 0.0f;
    reverb.setParameters(rp);

    auto numSamples = buffer.getNumSamples();
    // Save dry copy for master dry/wet
    juce::AudioBuffer<float> dryBuffer;
    dryBuffer.makeCopyOf(buffer);

    auto applyDistortion = [&](int channelIndex)
    {
        auto* channelData = buffer.getWritePointer(channelIndex);
        for (int i = 0; i < numSamples; ++i)
        {
            float input = channelData[i];
            float driven = std::tanh(input * drive);
            channelData[i] = (1.0f - distMix) * input + distMix * driven;
        }
    };

    if (swap)
    {
        // Reverb → Distortion
        if (totalNumInputChannels >= 2)
            reverb.processStereo(buffer.getWritePointer(0), buffer.getWritePointer(1), numSamples);
        else if (totalNumInputChannels == 1)
            reverb.processMono(buffer.getWritePointer(0), numSamples);

        for (int ch = 0; ch < totalNumInputChannels; ++ch)
            applyDistortion(ch);
    }
    else
    {
        // Distortion → Reverb
        for (int ch = 0; ch < totalNumInputChannels; ++ch)
            applyDistortion(ch);

        if (totalNumInputChannels >= 2)
            reverb.processStereo(buffer.getWritePointer(0), buffer.getWritePointer(1), numSamples);
        else if (totalNumInputChannels == 1)
            reverb.processMono(buffer.getWritePointer(0), numSamples);
    }

    // Master dry/wet blend
    const float masterMix = masterDryWetParam != nullptr ? masterDryWetParam->load() : 1.0f;
    for (int ch = 0; ch < totalNumOutputChannels; ++ch)
    {
        auto* wet = buffer.getWritePointer(ch);
        auto* dry = dryBuffer.getReadPointer(ch);
        for (int i = 0; i < numSamples; ++i)
        {
            wet[i] = (1.0f - masterMix) * dry[i] + masterMix * wet[i];
        }
    }

    // Final output gain
    for (int channel = 0; channel < totalNumOutputChannels; ++channel)
    {
        auto* channelData = buffer.getWritePointer(channel);
        for (int i = 0; i < numSamples; ++i)
            channelData[i] *= outputGain;
    }

    float gain = apvts.getRawParameterValue("GAIN")->load();
    buffer.applyGain(gain);
}

//==============================================================================
bool AudioPluginAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* AudioPluginAudioProcessor::createEditor()
{
    return new AudioPluginAudioProcessorEditor(*this);
}

//==============================================================================
void AudioPluginAudioProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
    juce::ignoreUnused(destData);
}

void AudioPluginAudioProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
    juce::ignoreUnused(data, sizeInBytes);
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new AudioPluginAudioProcessor();
}
