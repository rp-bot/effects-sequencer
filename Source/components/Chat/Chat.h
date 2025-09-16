
#pragma once
#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_core/juce_core.h>


class Chat :public juce::Component
{
public:
    Chat();
    ~Chat() override = default;

    void resized() override;


private:
    void setLoading(bool shouldLoad);
    void handleSubmit();
    void sendRequest(const juce::String& userMessage);
    void handleResponse(const juce::String& response);
    void addMessage(const juce::String& text, bool isUser);

    // Bubble transcript
    juce::Viewport transcriptViewport;
    juce::Component transcriptContent;
    juce::OwnedArray<juce::Component> bubbles;
    juce::TextEditor chatInput;
    juce::TextButton submitButton;
    juce::Label loadingLabel;
    bool isLoading = false;



    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Chat)
};
