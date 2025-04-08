#include "DistortionControls.h"

DistortionControls::DistortionControls(juce::AudioProcessorValueTreeState& apvts)
{
    configureSlider(driveSlider, driveLabel, "Drive m");
    configureSlider(mixSlider, mixLabel, "Mix");
    configureSlider(outputSlider, outputLabel, "Output");
    configureSlider(volumeSlider, volumeLabel, "Volume");

    driveAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts, "drive", driveSlider);
    mixAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts, "mix", mixSlider);
    outputAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts, "output", outputSlider);
    volumeAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts, "GAIN", volumeSlider);
}

void DistortionControls::configureSlider(juce::Slider& slider, juce::Label& label, const juce::String& name)
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

void DistortionControls::resized()
{
    const int sliderWidth = 80;
    const int sliderHeight = 100;
    const int margin = 20;

    driveSlider.setBounds(margin, margin, sliderWidth, sliderHeight);
    mixSlider.setBounds(margin + sliderWidth + margin, margin, sliderWidth, sliderHeight);
    outputSlider.setBounds(margin + 2 * (sliderWidth + margin), margin, sliderWidth, sliderHeight);
    volumeSlider.setBounds(margin + 3 * (sliderWidth + margin), margin, sliderWidth, sliderHeight);
}

DistortionControls::~DistortionControls()
{

}