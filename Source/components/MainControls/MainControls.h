#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_audio_processors/juce_audio_processors.h>

class MainControls : public juce::Component
{
public:
    explicit MainControls(juce::AudioProcessorValueTreeState& apvts);
    ~MainControls() override;

    void paint(juce::Graphics&) override;
    void resized() override;

private:
    void configureSlider(juce::Slider&, juce::Label&, const juce::String&);

    juce::Label titleLabel { {}, "Main Controls" };
    juce::Slider masterMixSlider;
    juce::Label masterMixLabel;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> masterMixAttachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainControls)
};


