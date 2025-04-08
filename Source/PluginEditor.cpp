#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
AudioPluginAudioProcessorEditor::AudioPluginAudioProcessorEditor(AudioPluginAudioProcessor& p)
    : AudioProcessorEditor(&p), distortionControls(p.apvts)
{
    addAndMakeVisible(distortionControls);
    // addAndMakeVisible(chatComponent);
    // setSize(1000, 500);

    addAndMakeVisible(chatComponent);

    setSize(1000, 500);


}

AudioPluginAudioProcessorEditor::~AudioPluginAudioProcessorEditor()
{
}

//==============================================================================
void AudioPluginAudioProcessorEditor::paint(juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll(juce::Colours::black);
    // g.setColour(juce::Colours::white);
    // g.setFont(15.0f);
    // g.drawFittedText("Volume Knob", getLocalBounds(), juce::Justification::centredTop, 1);


}

void AudioPluginAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
    // const int sliderWidth = 80;
    // const int sliderHeight = 100;
    // const int margin = 30;

    auto area = getLocalBounds().reduced(10);
    distortionControls.setBounds(area.removeFromTop(140));
    chatComponent.setBounds(area);

}
