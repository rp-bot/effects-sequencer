#include "DistortionControls.h"

DistortionControls::DistortionControls(juce::AudioProcessorValueTreeState& apvts)
{
    titleLabel.setJustificationType(juce::Justification::centredLeft);
    titleLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    titleLabel.setFont(juce::Font(16.0f, juce::Font::bold));
    addAndMakeVisible(titleLabel);
    configureSlider(driveSlider, driveLabel, "Drive");
    configureSlider(mixSlider, mixLabel, "Dry/Wet");
    configureSlider(outputSlider, outputLabel, "Output");
    configureSlider(volumeSlider, volumeLabel, "Volume");

    driveAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts, "drive", driveSlider);
    mixAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts, "mix", mixSlider);
    outputAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts, "output", outputSlider);
    volumeAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts, "GAIN", volumeSlider);
}

void DistortionControls::paint(juce::Graphics& g)
{
    auto bounds = getLocalBounds().toFloat();
    auto bg = bounds.reduced(2.0f);
    g.setColour(juce::Colours::darkslategrey.darker(0.6f));
    g.fillRoundedRectangle(bg, 8.0f);
    g.setColour(juce::Colours::white.withAlpha(0.15f));
    g.drawRoundedRectangle(bg, 8.0f, 1.5f);
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
    const int outerMargin = 10;
    const int headerHeight = 24;
    const int headerGap = 16;
    const int sliderWidth = 80;
    const int sliderHeight = 100;
    const int gap = 20;

    auto area = getLocalBounds().reduced(outerMargin);
    titleLabel.setBounds(area.removeFromTop(headerHeight));
    area.removeFromTop(headerGap);

    // Order: drive, output, gain (volume), dry/wet (last)
    driveSlider.setBounds(area.removeFromLeft(sliderWidth).removeFromTop(sliderHeight));
    area.removeFromLeft(gap);
    outputSlider.setBounds(area.removeFromLeft(sliderWidth).removeFromTop(sliderHeight));
    area.removeFromLeft(gap);
    volumeSlider.setBounds(area.removeFromLeft(sliderWidth).removeFromTop(sliderHeight));
    area.removeFromLeft(gap);
    mixSlider.setBounds(area.removeFromLeft(sliderWidth).removeFromTop(sliderHeight));
}

DistortionControls::~DistortionControls()
{

}