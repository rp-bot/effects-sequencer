#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_audio_processors/juce_audio_processors.h>

class DistortionControls : public juce::Component
{
public:
    DistortionControls(juce::AudioProcessorValueTreeState& apvts);
    ~DistortionControls() override;

    void paint(juce::Graphics&) override;
    void resized() override;

private:
    void configureSlider(juce::Slider&, juce::Label&, const juce::String&);

    juce::Slider driveSlider, mixSlider, outputSlider, volumeSlider;
    juce::Label driveLabel, mixLabel, outputLabel, volumeLabel;
    juce::Label titleLabel { {}, "Distortion" };

    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> driveAttachment, mixAttachment, outputAttachment, volumeAttachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DistortionControls)
};
