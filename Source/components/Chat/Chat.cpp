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
    submitButton.onClick = [this] { sendRequest(chatInput.getText().trim()); };
    addAndMakeVisible(submitButton);
}

void Chat::handleSubmit()
{
    const juce::String userText = chatInput.getText().trim();

    if (userText.isNotEmpty())
    {
        chatDisplay.moveCaretToEnd();
        chatDisplay.insertTextAtCaret("You: " + userText + "\n");

        // Hook for external response logic
    }
}

void Chat::resized() {
    auto area = getLocalBounds().reduced(10);
    chatDisplay.setBounds(area.removeFromTop(getHeight() - 60));
    chatInput.setBounds(area.removeFromLeft(getWidth() - 60));
    submitButton.setBounds(area);
}


void Chat::sendRequest(const juce::String& userMessage)
{

    juce::URL url("https://postman-echo.com/get?message=" + userMessage.replace(" ", "%20"));

    juce::Thread::launch([this, url]() {
        auto response = url.readEntireTextStream();

        juce::MessageManager::callAsync([this, response]() {
            handleResponse(response);
            });
        });
}

void Chat::handleResponse(const juce::String& response)
{
    chatInput.clear();

    auto json = juce::JSON::parse(response);
    if (auto* obj = json.getDynamicObject())
    {
        auto args = obj->getProperty("args").toString();
        chatDisplay.insertTextAtCaret("Bot: " + args + "\n");
    }
}