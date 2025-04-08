#include "Chat.h"

Chat::Chat()
{
    chatDisplay.setMultiLine(true);
    chatDisplay.setReadOnly(true);
    chatDisplay.setScrollbarsShown(true);
    chatDisplay.setColour(juce::TextEditor::backgroundColourId, juce::Colours::darkgrey);
    chatDisplay.setColour(juce::TextEditor::textColourId, juce::Colours::white);
    addAndMakeVisible(chatDisplay);

    chatInput.setMultiLine(false);
    chatInput.setTextToShowWhenEmpty("Type message...", juce::Colours::grey);
    chatInput.setColour(juce::TextEditor::backgroundColourId, juce::Colours::black);
    chatInput.setColour(juce::TextEditor::textColourId, juce::Colours::white);
    addAndMakeVisible(chatInput);

    submitButton.setButtonText("Send");
    submitButton.onClick = [this] { handleSubmit(); };
    addAndMakeVisible(submitButton);
}

void Chat::handleSubmit()
{
    const juce::String userText = chatInput.getText().trim();

    if (userText.isNotEmpty())
    {
        chatDisplay.moveCaretToEnd();
        chatDisplay.insertTextAtCaret("You: " + userText + "\n");
        chatInput.clear();

        // Hook for external response logic
    }
}

void Chat::resized(){
    auto area = getLocalBounds().reduced(10);
    chatDisplay.setBounds(area.removeFromTop(getHeight() - 60));
    chatInput.setBounds(area.removeFromLeft(getWidth() - 60));
    submitButton.setBounds(area);
}