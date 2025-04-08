#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
AudioPluginAudioProcessorEditor::AudioPluginAudioProcessorEditor(AudioPluginAudioProcessor& p)
    : AudioProcessorEditor(&p), processorRef(p)
{
    configureSlider(volumeSlider, volumeLabel, "Volume");

    configureSlider(driveSlider, driveLabel, "Drive");
    configureSlider(mixSlider, mixLabel, "Mix");
    configureSlider(outputSlider, outputLabel, "Output");

    auto& params = processorRef.apvts;
    driveAttachment = std::make_unique<SliderAttachment>(params, "drive", driveSlider);
    mixAttachment = std::make_unique<SliderAttachment>(params, "mix", mixSlider);
    outputAttachment = std::make_unique<SliderAttachment>(params, "output", outputSlider);

    volumeAttachment = std::make_unique<SliderAttachment>(processorRef.apvts, "GAIN", volumeSlider);

    // Chat display (read-only log)
    chatDisplay.setMultiLine(true);
    chatDisplay.setReadOnly(true);
    chatDisplay.setScrollbarsShown(true);
    chatDisplay.setColour(juce::TextEditor::backgroundColourId, juce::Colours::darkgrey);
    chatDisplay.setColour(juce::TextEditor::textColourId, juce::Colours::white);
    addAndMakeVisible(chatDisplay);

    // Chat input
    chatInput.setMultiLine(false);
    chatInput.setTextToShowWhenEmpty("Type message...", juce::Colours::grey);
    chatInput.setColour(juce::TextEditor::backgroundColourId, juce::Colours::black);
    chatInput.setColour(juce::TextEditor::textColourId, juce::Colours::white);
    addAndMakeVisible(chatInput);

    // Submit button
    submitButton.setButtonText("Send");
    submitButton.onClick = [this] { handleSubmit(); };
    addAndMakeVisible(submitButton);

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
    const int sliderWidth = 80;
    const int sliderHeight = 100;
    const int margin = 30;

    driveSlider.setBounds(margin, margin, sliderWidth, sliderHeight);
    mixSlider.setBounds(margin + sliderWidth + margin, margin, sliderWidth, sliderHeight);
    outputSlider.setBounds(margin + 2 * (sliderWidth + margin), margin, sliderWidth, sliderHeight);
    volumeSlider.setBounds(margin + 3 * (sliderWidth + margin), margin, sliderWidth, sliderHeight);

    const int chatX = margin + 3 * (sliderWidth + margin);
    const int chatWidth = getWidth() - chatX - margin;

    chatDisplay.setBounds(chatX, margin, chatWidth, getHeight() - 60);
    chatInput.setBounds(chatX, getHeight() - 50, chatWidth - 60, 30);
    submitButton.setBounds(chatX + chatWidth - 55, getHeight() - 50, 50, 30);

}

void AudioPluginAudioProcessorEditor::configureSlider(juce::Slider& slider, juce::Label& label, const juce::String& name)
{
    slider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    slider.setTextBoxStyle(juce::Slider::TextBoxBelow, true, 50, 20);
    addAndMakeVisible(slider);

    label.setText(name, juce::dontSendNotification);
    label.attachToComponent(&slider, false);
    label.setJustificationType(juce::Justification::centred);
    label.setColour(juce::Label::textColourId, juce::Colours::white);
    addAndMakeVisible(label);
}

void AudioPluginAudioProcessorEditor::handleSubmit()
{
    const juce::String userText = chatInput.getText().trim();

    if (userText.isNotEmpty())
    {
        chatDisplay.moveCaretToEnd();
        chatDisplay.insertTextAtCaret("You: " + userText + "\n");
        chatInput.clear();

        // TODO: Add response integration here later
    }
}