#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_audio_processors/juce_audio_processors.h>

class ReverbControls : public juce::Component
{
public:
    ReverbControls(juce::AudioProcessorValueTreeState& apvts);
    ~ReverbControls() override;

    void paint(juce::Graphics&) override;
    void resized() override;

private:
    void configureSlider(juce::Slider&, juce::Label&, const juce::String&);

    juce::Slider mixSlider, roomSlider, dampingSlider, widthSlider;
    juce::Label mixLabel, roomLabel, dampingLabel, widthLabel;
    juce::Label titleLabel { {}, "Reverb" };
    juce::ToggleButton freezeButton { "Freeze" };

    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> mixAttachment, roomAttachment, dampingAttachment, widthAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> freezeAttachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ReverbControls)
};


