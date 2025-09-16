#include "MainControls.h"

MainControls::MainControls(juce::AudioProcessorValueTreeState& apvts)
{
    titleLabel.setJustificationType(juce::Justification::centredLeft);
    titleLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    titleLabel.setFont(juce::Font(16.0f, juce::Font::bold));
    addAndMakeVisible(titleLabel);

    configureSlider(masterMixSlider, masterMixLabel, "Master Dry/Wet");
    masterMixAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts, "masterDryWet", masterMixSlider);
}

MainControls::~MainControls() = default;

void MainControls::paint(juce::Graphics& g)
{
    auto bounds = getLocalBounds().toFloat();
    auto bg = bounds.reduced(2.0f);
    g.setColour(juce::Colours::darkslategrey.darker(0.6f));
    g.fillRoundedRectangle(bg, 8.0f);
    g.setColour(juce::Colours::white.withAlpha(0.15f));
    g.drawRoundedRectangle(bg, 8.0f, 1.5f);
}

void MainControls::configureSlider(juce::Slider& slider, juce::Label& label, const juce::String& name)
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

void MainControls::resized()
{
    const int outerMargin = 10;
    const int headerHeight = 24;
    const int headerGap = 16;
    const int sliderWidth = 80;
    const int sliderHeight = 100;

    auto area = getLocalBounds().reduced(outerMargin);
    titleLabel.setBounds(area.removeFromTop(headerHeight));
    area.removeFromTop(headerGap);
    masterMixSlider.setBounds(area.removeFromLeft(sliderWidth).removeFromTop(sliderHeight));
}


