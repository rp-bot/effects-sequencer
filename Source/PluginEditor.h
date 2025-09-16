#pragma once

#include "PluginProcessor.h"
#include "components/DistortionControls/DistortionControls.h"
#include "components/ReverbControls/ReverbControls.h"
#include "components/MainControls/MainControls.h"
#include <juce_gui_basics/juce_gui_basics.h>
#include "components/Chat/Chat.h"
//==============================================================================
class AudioPluginAudioProcessorEditor : public juce::AudioProcessorEditor
{
public:
    explicit AudioPluginAudioProcessorEditor(AudioPluginAudioProcessor&);
    ~AudioPluginAudioProcessorEditor() override;

    //==============================================================================
    void paint(juce::Graphics&) override;
    void resized() override;

private:
    void updateOrderUI();
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    // AudioPluginAudioProcessor& processorRef;

    DistortionControls distortionControls;
    ReverbControls reverbControls;
    MainControls mainControls;
    juce::ToggleButton swapButton { "Click to swap" };
    juce::ComboBox effectOrderLabel;
    Chat chatComponent;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> swapAttachment;
    // juce::Slider driveSlider, mixSlider, outputSlider, volumeSlider;
    // juce::Label driveLabel, mixLabel, outputLabel, volumeLabel;

    // // juce::Slider volumeSlider;
    // // std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> volumeAttachment;


    // using SliderAttachment = juce::AudioProcessorValueTreeState::SliderAttachment;
    // std::unique_ptr<SliderAttachment> driveAttachment;
    // std::unique_ptr<SliderAttachment> mixAttachment;
    // std::unique_ptr<SliderAttachment> outputAttachment;
    // std::unique_ptr<SliderAttachment> volumeAttachment;



    // void configureSlider(juce::Slider& slider, juce::Label& label, const juce::String& name);

    // juce::TextEditor chatDisplay;
    // juce::TextEditor chatInput;
    // juce::TextButton submitButton;

    // void handleSubmit();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AudioPluginAudioProcessorEditor)
};
