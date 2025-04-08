
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
    void handleSubmit();
    void sendRequest(const juce::String& userMessage);
    void handleResponse(const juce::String& response);

    juce::TextEditor chatDisplay, chatInput;
    juce::TextButton submitButton;



    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Chat)
};
