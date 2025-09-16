#include "ReverbControls.h"

ReverbControls::ReverbControls(juce::AudioProcessorValueTreeState& apvts)
{
    titleLabel.setJustificationType(juce::Justification::centredLeft);
    titleLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    titleLabel.setFont(juce::Font(16.0f, juce::Font::bold));
    addAndMakeVisible(titleLabel);
    configureSlider(mixSlider, mixLabel, "Dry/Wet");
    configureSlider(roomSlider, roomLabel, "Room");
    configureSlider(dampingSlider, dampingLabel, "Damping");
    configureSlider(widthSlider, widthLabel, "Width");

    mixAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts, "reverbMix", mixSlider);
    roomAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts, "reverbRoom", roomSlider);
    dampingAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts, "reverbDamping", dampingSlider);
    widthAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts, "reverbWidth", widthSlider);
    addAndMakeVisible(freezeButton);
    freezeAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(apvts, "reverbFreeze", freezeButton);
}

void ReverbControls::paint(juce::Graphics& g)
{
    auto bounds = getLocalBounds().toFloat();
    auto bg = bounds.reduced(2.0f);
    g.setColour(juce::Colours::darkslategrey.darker(0.6f));
    g.fillRoundedRectangle(bg, 8.0f);
    g.setColour(juce::Colours::white.withAlpha(0.15f));
    g.drawRoundedRectangle(bg, 8.0f, 1.5f);
}

void ReverbControls::configureSlider(juce::Slider& slider, juce::Label& label, const juce::String& name)
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

void ReverbControls::resized()
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

    // Order: room, damping, width, dry/wet last
    roomSlider.setBounds(area.removeFromLeft(sliderWidth).removeFromTop(sliderHeight));
    area.removeFromLeft(gap);
    dampingSlider.setBounds(area.removeFromLeft(sliderWidth).removeFromTop(sliderHeight));
    area.removeFromLeft(gap);
    widthSlider.setBounds(area.removeFromLeft(sliderWidth).removeFromTop(sliderHeight));
    area.removeFromLeft(gap);
    mixSlider.setBounds(area.removeFromLeft(sliderWidth).removeFromTop(sliderHeight));
    freezeButton.setBounds(area.removeFromLeft(100).withTop(outerMargin + headerHeight + 30).withHeight(24));
}

ReverbControls::~ReverbControls()
{

}


