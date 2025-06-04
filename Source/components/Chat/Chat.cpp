#include "Chat.h"

Chat::Chat()
{
    chatDisplay.setMultiLine(true);
    chatDisplay.setReadOnly(true);
    chatDisplay.setScrollbarsShown(true);
    chatDisplay.setColour(juce::TextEditor::backgroundColourId, juce::Colours::darkgrey);
    chatDisplay.setColour(juce::TextEditor::textColourId, juce::Colours::white);
    addAndMakeVisible(chatDisplay);
    chatDisplay.insertTextAtCaret("Chat started. Type a message and click Send.\n");

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

        // Hook for external response logic
        sendRequest(userText);
        // chatInput.clear();
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
    juce::String encodedMessage = juce::URL::addEscapeChars(userMessage, true, false);

    juce::URL url("https://postman-echo.com/get?message=" + encodedMessage);
    
    chatDisplay.moveCaretToEnd();
    chatDisplay.insertTextAtCaret("Sending request to: " + url.toString(true) + "\n"); // Log URL

    juce::Thread::launch([this, url]() {
        auto response = url.readEntireTextStream();

        juce::MessageManager::callAsync([this, response]() {
            handleResponse(response);
            });
        });
}

void Chat::handleResponse(const juce::String& response)
{
    chatInput.clear(); // Clear input after trying to send

    chatDisplay.moveCaretToEnd(); // Ensure caret is at the end before new messages

    if (response.isEmpty())
    {

        chatDisplay.insertTextAtCaret("Bot: Error - No response from server.\n");
        return;
    }

    juce::var json; // Use juce::var for easier parsing and checking
    juce::Result parseResult = juce::JSON::parse(response, json);

    if (parseResult.wasOk())
    {
        // DBG("JSON Parsed OK: " + juce::JSON::toString(json));
        // juce::Logger::writeToLog("JSON Parsed OK: " + juce::JSON::toString(json));

        if (auto* obj = json.getDynamicObject())
        {
            if (auto* args = obj->getProperty("args").getDynamicObject())
            {
                auto message = args->getProperty("message").toString();
                if (message.isNotEmpty())
                {
                    chatDisplay.insertTextAtCaret("Bot: " + message + "\n");
                }
                else
                {
                    chatDisplay.insertTextAtCaret("Bot: Error - Malformed response (missing message).\n");
                }
            }
            else
            {
                chatDisplay.insertTextAtCaret("Bot: Error - Malformed response (missing args).\n");
            }
        }
        else
        {
            chatDisplay.insertTextAtCaret("Bot: Error - Response format is not a JSON object.\n");
        }
    }
    else
    {
        chatDisplay.insertTextAtCaret("Bot: Error - Failed to parse server response.\n");
        chatDisplay.insertTextAtCaret("Raw response was: " + response + "\n");
    }
}