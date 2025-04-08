
#pragma once
#include <juce_gui_basics/juce_gui_basics.h>
class Chat :public juce::Component
{
public:
    Chat();
    ~Chat() override=default ;

    void resized() override;


private:
    void handleSubmit();
    juce::TextEditor chatDisplay, chatInput;
    juce::TextButton submitButton;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Chat)
};
